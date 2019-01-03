#include <nan.h>
#include "include/client.hpp"

void Init(v8::Local<v8::Object> exports) {
  Client::Init(exports);
}

NODE_MODULE(quesync, Init)