#include "client.hpp"
#include "flecs/addons/cpp/c_types.hpp"
#include "flecs/addons/cpp/mixins/pipeline/decl.hpp"
#include "input.hpp"
#include "server.hpp"
#include "system_helper.hpp"
#include "zmq.hpp"
#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdio.h>

using json = nlohmann::json;

enum { CONTROL, GRAPH, AUTH, REPLY };
void init_zmq(flecs::iter &it) {
  printf("connect to server\n");
  auto &client = *it.world().get_mut<ZmqClientRef>();
  client->connect(it.world().get<ServerAddress>()->c_str());
  printf("server connected\n");
}

void control_cmd(flecs::iter &it, Direction *directions,
                 SnakeController *controller) {
  printf("start cmd parsing\n");
  auto &client = *it.world().get_mut<ZmqClientRef>();
  json cmd;
  cmd["type"] = CONTROL;
  cmd["id"] = controller[0].player_id;
  cmd["cmd"] = directions[0];

    auto & socket = client->socket();
    zmq::message_t msg{cmd.dump()};
    socket.send(msg, zmq::send_flags::none);
    printf("send %s\n", cmd.dump().c_str());
    auto recv = socket.recv(msg, zmq::recv_flags::none);


}

void ZmqClientPlugin::build(flecs::world &world) {
  printf("zmq plugin\n");
  init_zmq_client_system(world).depends_on(flecs::OnStart);
  IntoSystemBuilder system(control_cmd);
  system.build(world);
}

auto init_zmq_client_system(flecs::world &world) -> flecs::system {
  IntoSystemBuilder system(init_zmq);
  return system.build(world);
}
