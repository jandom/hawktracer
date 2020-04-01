#include <iostream>
#include "hawktracer_client_nodejs.hpp"

namespace HawkTracer
{
namespace Nodejs
{

Object Client::init_bindings(class Env env, Object exports)
{
    HandleScope scope(env);

    Function constructor = DefineClass(
        env,
        "HawkTracerClient",
        {
            InstanceMethod("start", &Client::start),
            InstanceMethod("onEvents", &Client::set_on_events),
            InstanceMethod("stop", &Client::stop)
        });
    Persistent(constructor).SuppressDestruct();
    exports.Set("HawkTracerClient", constructor);
    return exports;
}

Client::Client(const CallbackInfo &info)
    : ObjectWrap<Client>(info)
{
    _source = info[0].As<String>();
}

Client::~Client()
{
    _stop();
}

Value Client::start(const CallbackInfo &info)
{
    _context = ClientContext::create(
        _source,
        [this](std::unique_ptr<std::vector<parser::Event>> data, ClientContext::ConsumeMode consume_mode)
        {
            return handle_event(std::move(data), consume_mode);
        }
    );
    return Boolean::New(info.Env(), static_cast<bool>(_context));
}

void Client::stop(const CallbackInfo &)
{
    _stop();
}

void Client::_stop()
{
    _context.reset();
    std::lock_guard<std::mutex> lock{_callback_lock};
    if (_callback) {
        _callback->function.Abort();
        _callback->function.Release();
        _callback.reset();
    }
}

void Client::set_on_events(const CallbackInfo &info)
{
    std::lock_guard<std::mutex> lock{_callback_lock};
    _callback.reset(new ThreadSafeFunctionHolder{ThreadSafeFunction::New(info.Env(),
                                                                         info[0].As<Napi::Function>(),
                                                                         "HawkTracerClientOnEvent",
                                                                         1,
                                                                         1)});
}

// This method is called from reader thread, while all other methods are called from js main thread.
std::unique_ptr<std::vector<parser::Event>>
Client::handle_event(std::unique_ptr<std::vector<parser::Event>> events, ClientContext::ConsumeMode consume_mode)
{
    std::lock_guard<std::mutex> lock{_callback_lock};
    if (!_callback) {
        return events;
    }

    auto events_raw_ptr = events.release();
    napi_status status;
    if (consume_mode == ClientContext::ConsumeMode::FORCE_CONSUME) {
        status = _callback->function.BlockingCall(events_raw_ptr, &Client::transform_and_callback);
    }
    else {
        status = _callback->function.NonBlockingCall(events_raw_ptr, &Client::transform_and_callback);
    }

    if (status == napi_queue_full) {
        return std::unique_ptr<std::vector<parser::Event>>{events_raw_ptr};
    }
    if (status != napi_ok) {
        std::cerr << "NonBlockingCall failed with " << status << std::endl;
    }
    return std::unique_ptr<std::vector<parser::Event>>{};
}

Value Client::convert_field_value(class Env env, const parser::Event::Value &value)
{
    switch (value.field->get_type_id()) {
        case parser::FieldTypeId::UINT8:
            return Number::New(env, value.value.f_UINT8);
        case parser::FieldTypeId::INT8:
            return Number::New(env, value.value.f_INT8);
        case parser::FieldTypeId::UINT16:
            return Number::New(env, value.value.f_UINT16);
        case parser::FieldTypeId::INT16:
            return Number::New(env, value.value.f_INT16);
        case parser::FieldTypeId::UINT32:
            return Number::New(env, value.value.f_UINT32);
        case parser::FieldTypeId::INT32:
            return Number::New(env, value.value.f_INT32);
        case parser::FieldTypeId::UINT64:
            return Number::New(env, value.value.f_UINT64);
        case parser::FieldTypeId::INT64:
            return Number::New(env, value.value.f_INT64);
        case parser::FieldTypeId::STRING:
            return String::New(env, value.value.f_STRING);
        case parser::FieldTypeId::POINTER:
            return String::New(env, "(pointer)");
        case parser::FieldTypeId::STRUCT:
            return convert_event(env, *value.value.f_EVENT);
        default:
            assert(0);
    }
}

Object Client::convert_event(class Env env, const parser::Event &event)
{
    auto o = Object::New(env);
    for (const auto &it: event.get_values()) {
        o.Set(it.first, convert_field_value(env, it.second));
    }
    return o;
}

void Client::transform_and_callback(class Env env, Function real_callback, std::vector<parser::Event> *events)
{
    std::unique_ptr<std::vector<parser::Event>> events_deallocation_guard{events};

    Array array = Array::New(env);
    int i = 0;
    std::for_each(events->cbegin(),
                  events->cend(),
                  [env, &array, &i](const parser::Event &e) { array[i++] = convert_event(env, e); });
    real_callback.Call({array});
}

} // namespace Nodejs
} // namespace HawkTracer
