#pragma once
#include <memory>
#include <asio.hpp>

#include "quesync.h"
#include "../shared/user.h"

#define MAX_DATA_LEN 1000

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
  Session(tcp::socket socket, Quesync *server);
  ~Session();

  void start();
  void recv();
  void send(std::string data);

  Quesync *server() const;
  std::shared_ptr<Session> getShared();

  bool authenticated() const;
  void setUser(User *user);
  User *user() const;

private:
  User *_user;
  tcp::socket _socket;
  Quesync *_server;
  char _data[MAX_DATA_LEN];
};