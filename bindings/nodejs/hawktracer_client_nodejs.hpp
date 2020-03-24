//
// Created by Hojin Ghim on 13/03/2020.
//

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
    void add_on_event(const CallbackInfo &info);

private:
    void handle_event(std::vector<const parser::Event *> data);
    static void transform_and_callback(class Env env, Function real_callback, std::vector<const parser::Event *> *data);

    std::string _source;
    std::vector<ThreadSafeFunction> _callbacks;
    std::unique_ptr<ClientContext> _context;
};

} // namespace Nodejs
} // namespace HawkTracer

static Napi::Object init_bindings(Napi::Env env, Napi::Object exports)
{
    return HawkTracer::Nodejs::Client::init_bindings(env, exports);
}

NODE_API_MODULE(hawk_tracer_client, init_bindings)

#endif // HAWKTRACER_CLIENT_NODEJS_HPP
