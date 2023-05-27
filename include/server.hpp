#pragma once

#include "flecs.h"
#include <string>
#include <zmq.hpp>

struct ServerAddress : std::string {};    

class ZmqServer {
public:
  explicit ZmqServer(int thread = 1)
      : context_(thread), socket_(context_, zmq::socket_type::rep) {} 
  void bind(const std::string &endpoint) {
    socket_.set(zmq::sockopt::linger, 1); 
    socket_.bind(endpoint); 
  }
  void close() { socket_.close(); }
  auto socket() -> zmq::socket_t & { return socket_; }  

private:
  zmq::context_t context_;
  zmq::socket_t socket_;
};
struct ZmqServerPlugin {
  void build(flecs::world &world);
};

class ZmqServerRef : public std::shared_ptr<ZmqServer> {};

auto init_zmq_server_system(flecs::world &world) -> flecs::system;

auto reply_commands_system(flecs::world &world) -> flecs::system;