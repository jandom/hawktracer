//
// Created by Hojin Ghim on 13/03/2020.
//

#ifndef HAWKTRACER_CLIENT_NODEJS_HPP
#define HAWKTRACER_CLIENT_NODEJS_HPP

#include <napi.h>

namespace HawkTracer
{
namespace Nodejs
{

using namespace Napi;

class Client: public ObjectWrap<Client>
{
public:
    static Object Init(class Env env, Object exports);

    explicit Client(const CallbackInfo &info);
    ~Client() override = default;

    class Value start(const CallbackInfo &info);

private:
    std::string _source;
};

} // namespace Nodejs
} // namespace HawkTracer

static Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    return HawkTracer::Nodejs::Client::Init(env, exports);
}

NODE_API_MODULE(hawk_tracer_client, Init)

#endif // HAWKTRACER_CLIENT_NODEJS_HPP
