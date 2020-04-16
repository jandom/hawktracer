#ifndef HAWKTRACER_CLIENT_NODEJS_HPP
#define HAWKTRACER_CLIENT_NODEJS_HPP

#include "client_context.hpp"

#include <napi.h>

namespace HawkTracer
{
namespace Nodejs
{

using namespace Napi;

class Client: public ObjectWrap<Client>
{
public:
    static Object init_bindings(class Env env, Object exports);

    explicit Client(const CallbackInfo &info);
    ~Client() override;

    class Value start(const CallbackInfo &info);
    void stop(const CallbackInfo &info);
    void set_on_events(const CallbackInfo &info);

private:
    std::unique_ptr<std::vector<parser::Event>>
    handle_events(std::unique_ptr<std::vector<parser::Event>> events, ClientContext::ConsumeMode consume_mode);
    static class Value convert_field_value(class Env env, const parser::Event::Value &value);
    static Object convert_event(class Env env, const parser::Event &event);

    using CallbackDataType = std::pair<Client *, std::unique_ptr<std::vector<parser::Event>>>;
    static void convert_and_callback(class Env env, Function real_callback, CallbackDataType *data);

    std::string _source{};

    class State
    {
        struct FunctionData
        {
            std::unique_ptr<ClientContext> client_context;
        };
        struct FunctionHolder
        {
            ThreadSafeFunction function;
            FunctionData *function_data;    // always deallocated by ThreadSafeFunction finalizer
            FunctionHolder(ThreadSafeFunction tsf, std::unique_ptr<FunctionData> fd)
                : function(tsf), function_data(fd.release())
            {
                assert(function_data);
            }
            ~FunctionHolder()
            {
                function.Release();
            }
        };
        // _client_context states
        // * started: non-null value
        // * delegated: null value. _function_holder has the actual value.
        // * stopped: null value
        std::unique_ptr<ClientContext> _client_context{};
        // _function_holder states
        // * has_callback: non-null value with non-null CallbackData. callback_data->client_context could still be null.
        // * no_callback: null value
        std::unique_ptr<FunctionHolder> _function_holder{};
        std::mutex _function_holder_mutex{};
        template<typename A>
        static void finalize(A /* provided by node-addon-api */, FunctionData *data, void *)
        {
            delete data;
        }
        // need _function_holder_mutex acquired before calling actual_client_context()
        std::unique_ptr<ClientContext> &actual_client_context()
        {
            return _function_holder
                   // has_callback
                   ? _function_holder->function_data->client_context
                   // no_callback
                   : _client_context;
        }
    public:
        bool is_started()
        {
            std::lock_guard<std::mutex> lock{_function_holder_mutex};
            return static_cast<bool>(actual_client_context());
        }
        // ?         X ?            => started   X ?
        void start(std::unique_ptr<ClientContext> cc)
        {
            std::lock_guard<std::mutex> lock{_function_holder_mutex};
            actual_client_context() = std::move(cc);
            assert(!(_client_context && _function_holder && _function_holder->function_data->client_context));
        }
        // started   X ?            => stopped   X ?
        // stopped   X ?            => stopped   X ?
        // delegated X ?            => delegated X ?
        void stop()
        {
            {
                std::lock_guard<std::mutex> lock{_function_holder_mutex};
                if (_function_holder) {
                    _function_holder->function.Abort();
                    _function_holder.reset();
                }
            }
            _client_context.reset();
        }
        template<typename A>
        using Finalizer = decltype(finalize<A>);
        using FinalizerDataType = FunctionData;
        // ?         X ?            => delegated X has_callback
        template<typename A>
        void set_function(std::function<ThreadSafeFunction(Finalizer<A>, FinalizerDataType *)> create)
        {
            std::lock_guard<std::mutex> lock{_function_holder_mutex};
            std::unique_ptr<FunctionData> data{new FunctionData{std::move(actual_client_context())}};
            ThreadSafeFunction function = create(finalize, data.get());
            _function_holder.reset(new FunctionHolder{function, std::move(data)});
            assert(!(_client_context && _function_holder && _function_holder->function_data->client_context));
        }
        // started   X has_callback => started   X has_callback
        napi_status use_function(const std::function<napi_status(ThreadSafeFunction)>& use)
        {
            std::lock_guard<std::mutex> lock{_function_holder_mutex};
            if (!_function_holder)
                return napi_queue_full;

            return use(_function_holder->function);
        }
    };
    State _state;
};

} // namespace Nodejs
} // namespace HawkTracer

static Napi::Object init_bindings(Napi::Env env, Napi::Object exports)
{
    return HawkTracer::Nodejs::Client::init_bindings(env, exports);
}

NODE_API_MODULE(hawk_tracer_client, init_bindings)

#endif // HAWKTRACER_CLIENT_NODEJS_HPP
