//
// Created by Hojin Ghim on 13/03/2020.
//

#include "node_js_converter.hpp"

namespace HawkTracer
{
namespace client
{

NodeJsConverter::NodeJsConverter(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<NodeJsConverter>(info)
{}

Napi::Object NodeJsConverter::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func = DefineClass(
        env,
        "HawkTracerClient",
        {
            InstanceMethod("start", &NodeJsConverter::start)
        });
    exports.Set("HawkTracerClient", func);
    return exports;
}

Napi::Value NodeJsConverter::start(const Napi::CallbackInfo &info)
{
    _started = true;
    return Napi::Boolean::New(info.Env(), _started);
}

} // client
} // HawkTracer
