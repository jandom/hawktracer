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

Object Client::init_bindings(class Env env, Object exports)
{
    Function constructor = DefineClass(
        env,
        "HawkTracerClient",
        {
            InstanceMethod("start", &Client::start)
        });
    Persistent(constructor).SuppressDestruct();
    exports.Set("HawkTracerClient", constructor);
    return exports;
}

Value Client::start(const CallbackInfo &info)
{
    _context = ClientContext::create(_source);
    return Boolean::New(info.Env(), static_cast<bool>(_context));
}

} // namespace Nodejs
} // namespace HawkTracer
