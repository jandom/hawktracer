//
// Created by Hojin Ghim on 13/03/2020.
//

#ifndef HAWKTRACER_CLIENT_NODEJS_HPP
#define HAWKTRACER_CLIENT_NODEJS_HPP

#include "converter.hpp"
#include <napi.h>

namespace HawkTracer
{
namespace client
{

class NodeJsConverter: public Converter, public Napi::ObjectWrap<NodeJsConverter>
{
public:
    ~NodeJsConverter() override = default;

    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    explicit NodeJsConverter(const Napi::CallbackInfo &info);

    bool init(const std::string &file_name) override {return true; /* TODO */}
    void process_event(const parser::Event &event) override { /* TODO */}
    void stop() override { /* TODO */}

private:
    Napi::Value start(const Napi::CallbackInfo &info);

    bool _started = false;
};

} // client
} // HawkTracer

static Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    return HawkTracer::client::NodeJsConverter::Init(env, exports);
}

NODE_API_MODULE(hawk_tracer_client, Init)

#endif // HAWKTRACER_CLIENT_NODEJS_HPP
