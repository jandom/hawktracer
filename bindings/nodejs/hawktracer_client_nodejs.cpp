//
// Created by Hojin Ghim on 13/03/2020.
//

#include "hawktracer_client_nodejs.hpp"

namespace HawkTracer
{
namespace Nodejs
{

Client::Client(const CallbackInfo &info)
    : ObjectWrap<Client>(info)
{
    _source = info[0].As<String>();
}

Client::~Client()
{
    std::for_each(_callbacks.begin(), _callbacks.end(), [](ThreadSafeFunction &cb) { cb.Release(); });
}
Object Client::init_bindings(class Env env, Object exports)
{
    Function constructor = DefineClass(
        env,
        "HawkTracerClient",
        {
            InstanceMethod("start", &Client::start),
            InstanceMethod("onData", &Client::add_on_event)
        });
    Persistent(constructor).SuppressDestruct();
    exports.Set("HawkTracerClient", constructor);
    return exports;
}

Value Client::start(const CallbackInfo &info)
{
    _context = ClientContext::create(
        _source,
        [this](std::vector<const parser::Event *> data)
        {
            handle_event(std::move(data));
        }
    );
    return Boolean::New(info.Env(), static_cast<bool>(_context));
}

void Client::add_on_event(const CallbackInfo &info)
{
    _callbacks
        .push_back(ThreadSafeFunction::New(info.Env(), info[0].As<Napi::Function>(), "HawkTracerClientOnData", 1, 1));
}

void Client::handle_event(std::vector<const parser::Event *> data)
{
    for (const auto &callback: _callbacks) {
        napi_status status = callback.BlockingCall(&data, &Client::transform_and_callback);
        if (status != napi_ok) {
            Error::Fatal("ThreadEntry", "ThreadSafeFunction.BlockingCall() failed");
        }
    }
}

void Client::transform_and_callback(class Env env, Function real_callback, std::vector<const parser::Event *> *data)
{
    real_callback.Call({Object::New(env)}); // TODO
}

} // namespace Nodejs
} // namespace HawkTracer
