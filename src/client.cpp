#include "client.hpp"
#include "flecs/addons/cpp/c_types.hpp"
#include "flecs/addons/cpp/mixins/pipeline/decl.hpp"
#include "input.hpp"
#include "server.hpp"
#include "system_helper.hpp"
#include "zmq.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum { CONTROL, GRAPH, AUTH, REPLY };
void init_zmq(flecs::iter &it) {
  auto &client = *it.world().get_mut<ZmqClient>();
  client.connect(it.world().get<ServerAddress>()->c_str());
}

void control_cmd(flecs::iter &it, Direction * directions,SnakeController * controller) {
    auto &client = *it.world().get_mut<ZmqClient>();
    json cmd;
    cmd["type"] = CONTROL;
    cmd["id"] = controller[0].player_id;
    cmd["cmd"] = directions[0];
    client.socket().send(zmq::message_t{cmd.dump()},zmq::send_flags::none);
}

void ZmqClientPlugin::build(flecs::world &world) {
    init_zmq_client_system(world).depends_on(flecs::OnStart);
}

auto init_zmq_client_system(flecs::world &world) -> flecs::system {
  IntoSystemBuilder system(init_zmq);
  return system.build(world);
}
