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
  //初始化 ZMQ 客户端并连接到服务器
  printf("connect to server\n");
  auto &client = *it.world().get_mut<ZmqClientRef>(); //获取客户端
  client->connect(it.world().get<ServerAddress>()->c_str());  //连接服务器
  printf("server connected\n");
}

void control_cmd(flecs::iter &it, Direction *directions,
                 SnakeController *controller) {
  //发送控制命令到服务器
  printf("start cmd parsing\n");
  auto &client = *it.world().get_mut<ZmqClientRef>();
  json cmd;
  cmd["type"] = CONTROL;
  cmd["id"] = controller[0].player_id;
  cmd["cmd"] = directions[0];

    auto & socket = client->socket(); //获取socket
    zmq::message_t msg{cmd.dump()}; //创建消息
    socket.send(msg, zmq::send_flags::none);  //发送消息
    printf("send %s\n", cmd.dump().c_str());
    auto recv = socket.recv(msg, zmq::recv_flags::none);  //接收消息


}

void ZmqClientPlugin::build(flecs::world &world) {
  //构建函数，用于在ECS世界中构建 ZMQ 客户端
  printf("zmq plugin\n");
  init_zmq_client_system(world).depends_on(flecs::OnStart);
  IntoSystemBuilder system(control_cmd);
  system.build(world);
}

auto init_zmq_client_system(flecs::world &world) -> flecs::system {
  //创建初始化 ZMQ 客户端的系统
  IntoSystemBuilder system(init_zmq);
  return system.build(world);
}
