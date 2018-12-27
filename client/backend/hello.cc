// hello.cc using N-API
#include <iostream>
#include <node_api.h>
#include "include/socket-manager.hpp"
#include "include/voice-chat.hpp"

void on_read(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) {
  std::cout << "Yovel tembel" << std::endl;
    
  if (nread == -1) {
    fprintf(stderr, "Read error!\n");
    uv_close((uv_handle_t*) handle, NULL);
    free(buf->base);
    return;
  }

  char sender[17] = { 0 };
  uv_ip4_name((struct sockaddr_in*) addr, sender, 16);
  fprintf(stderr, "Recv from %s\n", sender);
}

void on_send(uv_udp_send_t *req, int status)
{
  std::cout << "Sima" << std::endl;
}

namespace demo {

napi_value Method(napi_env env, napi_callback_info args) {
  napi_value greeting;
  napi_status status;
  VoiceChat *voiceChatManager;
  uv_loop_t *loop;
  uv_udp_t socket;

  //SocketManager::initWinsock();

  SocketManager::InitSocketManager();
  voiceChatManager = new VoiceChat("127.0.0.1");

  /*std::cout << napi_get_uv_event_loop(env, &loop) << std::endl;

  uv_udp_init(loop, &socket);

  uv_udp_send_t send_req;
  uv_buf_t buffer; 
  alloc_buffer((uv_handle_t*)&send_req, 256, &buffer);
  memset(buffer.base, 0, buffer.len);

  struct sockaddr_in addr;
  uv_ip4_addr("0.0.0.0", 0, (sockaddr_in *)&addr);
  uv_udp_bind(&socket, (const struct sockaddr *)&addr, 0);

  std::cout << "Hello" << std::endl;

  struct sockaddr_in send_addr;
  uv_ip4_addr("127.0.0.1", 61111, (sockaddr_in *)&send_addr);
  uv_udp_send(&send_req, &socket, &buffer, 1, (const sockaddr *)&send_addr, (uv_udp_send_cb)on_send);

  std::cout << uv_udp_recv_start(&socket, alloc_buffer, on_read) << std::endl;

  uv_run(loop, UV_RUN_DEFAULT);*/

  status = napi_create_string_utf8(env, "hello", NAPI_AUTO_LENGTH, &greeting);
  if (status != napi_ok) return nullptr;
  return greeting;
}

napi_value init(napi_env env, napi_value exports) {
  napi_status status;
  napi_value fn;

  status = napi_create_function(env, nullptr, 0, Method, nullptr, &fn);
  if (status != napi_ok) return nullptr;

  status = napi_set_named_property(env, exports, "hello", fn);
  if (status != napi_ok) return nullptr;
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init)

}  // namespace demo