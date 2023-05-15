#include "server.hpp"
#include "flecs/addons/cpp/c_types.hpp"
#include "input.hpp"
#include "system_helper.hpp"
#include "zmq.hpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

enum { CONTROL, GRAPH, AUTH, REPLY };
void init_zmq_server(flecs::iter &it, ZmqServer *server,
                     ServerAddress *bind_address) {
  for (auto i = 0; i < it.count(); i++) {
    server[i].bind(bind_address[i].c_str());
  }
}

auto handle_message(zmq::message_t &message, flecs::iter &it)
    -> zmq::message_t {
  auto json_msg = json::parse(message.to_string_view());
  auto cmdtype = json_msg["type"].get<int>();
  json reply_msg;
  reply_msg["type"] = REPLY;
  reply_msg["code"] = "GOOD";
  switch (cmdtype) {
  case (int)CONTROL: {
    auto player_id = json_msg["id"].get<flecs::entity_t>();
    auto cmd = json_msg["cmd"].get<int>();
    it.world().entity(player_id).set<Direction>(cmd);

    break;
  }
  case (int)GRAPH:

    break;
  case (int)AUTH:

    break;
  }
  return zmq::message_t{reply_msg.dump()};
}
void reply_commands(flecs::iter &it, ZmqServer *servers) {
  for (auto i = 0; i < it.count(); i++) {
    auto &server = servers[i];
    auto &socket = server.socket();
    zmq::message_t message;
    while (true) {
      auto success = socket.recv(message, zmq::recv_flags::dontwait);
      if (success.has_value()) {
        auto reply = handle_message(message, it);
        socket.send(reply, zmq::send_flags::none);
      } else {
        break;
      }
    }
  }
}
auto init_zmq_server_system(flecs::world &world) -> flecs::system {
  IntoSystemBuilder system(init_zmq_server);
  return system.build(world);
}

auto reply_commands_system(flecs::world &world) -> flecs::system {
  IntoSystemBuilder system(reply_commands);
  return system.build(world);
}

void ZmqServerPlugin::build(flecs::world &world) {

    
}