//
// Created by Hojin Ghim on 13/03/2020.
//

#ifndef HAWKTRACER_NODE_JS_CONVERTER_HPP
#define HAWKTRACER_NODE_JS_CONVERTER_HPP

#include "converter.hpp"
#include <napi.h>

namespace HawkTracer
{
namespace client {

class NodeJsConverter : public Converter {};

}
}

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
  // Create a new instance
  Napi::Object obj = Napi::Object::New(env);

  // Assign values to properties
  obj.Set("hello", "world");
  obj.Set(uint32_t(42), "The Answer to Life, the Universe, and Everything");
  obj.Set("Douglas Adams", true);
  exports.Set(Napi::String::New(env, "hello"), obj);
  return exports;
}

NODE_API_MODULE(hawktracer_client, Init)

#endif // HAWKTRACER_NODE_JS_CONVERTER_HPP
