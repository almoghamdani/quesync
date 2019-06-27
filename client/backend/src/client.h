#pragma once
#include <napi.h>
#include <uv.h>
#include <functional>

#include "voice-chat.h"
#include "communicator.h"

#include "../../../shared/user.h"

#define SERVER_PORT 61110
#define MAX_DATA_LEN 10000

using asio::ip::tcp;

class Client : public Napi::ObjectWrap<Client>
{
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  Client(const Napi::CallbackInfo &info);
  ~Client();

  Napi::Value connect(const Napi::CallbackInfo &info);

  Napi::Value login(const Napi::CallbackInfo &info);
  Napi::Value signup(const Napi::CallbackInfo &info);
  Napi::Value getUser(const Napi::CallbackInfo &info);

  Napi::Value getUserProfile(const Napi::CallbackInfo &info);
  Napi::Value search(const Napi::CallbackInfo &info);
  Napi::Value sendFriendRequest(const Napi::CallbackInfo &info);
  Napi::Value setFriendshipStatus(const Napi::CallbackInfo &info);

  Napi::Value getPrivateChannel(const Napi::CallbackInfo &info);

  Napi::Value sendMessage(const Napi::CallbackInfo &info);
  Napi::Value getChannelMessages(const Napi::CallbackInfo &info);

  Napi::Value registerEventHandler(const Napi::CallbackInfo &info);

private:
  static Napi::FunctionReference constructor;

  User *_user;
  VoiceChat *_voice_chat_manager;

  Communicator _communicator;

  void clean();
};