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
{}

Object Client::Init(class Env env, Object exports)
{
    Function func = DefineClass(
        env,
        "HawkTracerClient",
        {
            InstanceMethod("start", &Client::start)
        });
    exports.Set("HawkTracerClient", func);
    return exports;
}

Value Client::start(const CallbackInfo &info)
{
    return Boolean::New(info.Env(), true);
}

} // namespace Nodejs
} // namespace HawkTracer
