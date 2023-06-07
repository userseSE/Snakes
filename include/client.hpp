#pragma once
#include "input.hpp"
#include "server.hpp"
#include "system_helper.hpp"
#include<nlohmann/json.hpp>
#include "zmq.hpp"
#include <memory>

class ZmqClient { 
public:
  explicit ZmqClient(int thread = 1)  
      : context_(thread), socket_(context_, zmq::socket_type::req) {}
  void connect(const std::string &endpoint) {
    socket_.set(zmq::sockopt::linger, 1);
    socket_.connect(endpoint);
  }
  void close() { socket_.close(); }
  auto socket() -> zmq::socket_t & { return socket_; }

private:
  zmq::context_t context_;
  zmq::socket_t socket_;
};
class ZmqClientRef : public std::shared_ptr<ZmqClient> {};

struct ZmqClientPlugin {
  void build(flecs::world &world);
};
auto init_zmq_client_system(flecs::world &world) -> flecs::system;

auto request_system(flecs::world &world) -> flecs::system;