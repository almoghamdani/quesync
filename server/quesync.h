#pragma once

#include <asio.hpp>
#include <map>
#include <memory>

#include <mysqlx/xdevapi.h>
namespace sql = mysqlx;

#include "user_manager.h"
#include "event_manager.h"
#include "channel_manager.h"

#define MAIN_SERVER_PORT 61110

using asio::ip::tcp;

class Quesync : public std::enable_shared_from_this<Quesync>
{
public:
  Quesync(asio::io_context &io_context);
  ~Quesync();

  void start();

  std::shared_ptr<UserManager> userManager();
  std::shared_ptr<EventManager> eventManager();
  std::shared_ptr<ChannelManager> channelManager();

  sql::Schema &db();

private:
  tcp::acceptor _acceptor;

  sql::Session _sess;
  sql::Schema _db;

  std::shared_ptr<UserManager> _user_manager;
  std::shared_ptr<EventManager> _event_manager;
  std::shared_ptr<ChannelManager> _channel_manager;

  void acceptClient();
};