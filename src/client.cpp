#include "client.hpp"
#include "Color.hpp"
#include "flecs/addons/cpp/c_types.hpp"
#include "flecs/addons/cpp/mixins/pipeline/decl.hpp"
#include "input.hpp"
#include "server.hpp"
#include "system_helper.hpp"
#include "zmq.hpp"
#include "zstdcpp.hpp"
#include <corecrt_malloc.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

using json = nlohmann::json;

enum { CONTROL, GRAPH, AUTH, REPLY, GRAPH_REPLY, AUTH_REPLY };

void init_zmq(flecs::iter &it) {
  // 初始化 ZMQ 客户端并连接到服务器
  printf("connect to server\n");
  auto &client = *it.world().get_mut<ZmqClientRef>(); // 获取客户端

  client->connect(it.world().get<ServerAddress>()->c_str()); // 连接服务器

  std::cout << "connected\n" << std::endl;
}

void control_cmd(flecs::iter &it) {
  auto controller = it.world().get<SnakeController>();
  auto directions = it.world().get<Direction>();
  // 发送控制命令到服务器

  auto &client = *it.world().get_mut<ZmqClientRef>(); // 获取客户端
  json cmd;
  cmd["type"] = CONTROL;
  // 获取SnakeController组件
  cmd["id"] = controller[0].player_id;
  cmd["cmd"] = directions[0];

  zstd buff;
  auto msg_data = buff.compress(cmd.dump(), 3);
  zmq::message_t msg{msg_data};    // 创建消息
  auto &socket = client->socket(); // 获取socket

  socket.send(msg, zmq::send_flags::none); // 发送消息

  auto recv = socket.recv(msg, zmq::recv_flags::none); // 接收消息
}

void graph_show(flecs::iter &it) {
  // 请求服务器发送图形数据
  auto &client = *it.world().get_mut<ZmqClientRef>();
  json graph;
  graph["type"] = GRAPH;
  graph["id"] = it.world().get<SnakeController>()->player_id;

  zstd buff;
  auto msg_data = buff.compress(graph.dump(), 3);
  zmq::message_t msg{msg_data};

  auto &socket = client->socket();
  socket.send(msg, zmq::send_flags::none);

  zmq::message_t recv_msg;
  auto recv = socket.recv(recv_msg, zmq::recv_flags::none);

  json received_json = json::parse(buff.decompress(recv_msg.to_string_view()));

  if (received_json["type"] == GRAPH_REPLY) {
    std::vector<raylib::Rectangle> received_rects =
        received_json["rect"].get<std::vector<raylib::Rectangle>>();
    std::vector<raylib::Color> received_colors =
        received_json["color"].get<std::vector<raylib::Color>>();

    // 在插入之前clear掉所有的rect和color
    it.world().delete_with<raylib::Color>();

    // 检查接收到的矩形和颜色数量是否匹配
    if (received_rects.size() == received_colors.size() &&
        !received_rects.empty()) {
      it.world().defer_begin();
      for (size_t i = 0; i < received_rects.size(); ++i) {
        auto e = it.world().entity();
        e.set<raylib::Rectangle>(received_rects[i]);
        e.set<raylib::Color>(received_colors[i]);
      }
      it.world().defer_end();
    }
  }
}

void user_verify(flecs::iter &it) {
  std::cout << "user verify" << std::endl;
  UserDatabase ud;

  // 构建JSON文件路径，相对于exe文件所在目录
  std::string jsonFilePath = "clientInfo.json";
  // 读取JSON文件
  std::ifstream file(jsonFilePath);
  // 解析JSON文件内容
  json data = json::parse(file);

  // 将解析后的JSON对象传递给UserDatabase，将其转换为UserDatabase对象
  ud["username"] = data["username"];
  ud["password"] = data["password"];

  file.close();
  // it.world().set(ud);
  it.world().set<UserDatabase>(ud);

  // 向服务端发送userdatabase对象
  auto &client = *it.world().get_mut<ZmqClientRef>(); // 获取客户端
  zstd buff;
  ud["type"] = AUTH;

  auto &socket = client->socket();         // 获取socket
  zmq::message_t msg{ud.dump()};           // 创建消息
  socket.send(msg, zmq::send_flags::none); // 发送消息

  // 接收消息
  zmq::message_t recv_msg;
  auto recv = socket.recv(recv_msg, zmq::recv_flags::none); // 接收消息

  auto decoded = buff.decompress(recv_msg.to_string_view());

  json received_json = json::parse(decoded);
  std::cout << received_json << std::endl;

  if (received_json["code"] == "SUCCESS") {

    SnakeController controller;
    controller.player_id = received_json["id"].get<int>();
    std::cout << controller.player_id << std::endl;

    // 将SnakeController组件设置到全局实体上
    it.world().set<SnakeController>(controller);
    it.world().set<Direction>(Direction::RIGHT);

  } else {
  }
}

auto user_verify_system(flecs::world &world) -> flecs::system {
  IntoSystemBuilder system(user_verify);
  return system.build(world);
}

auto init_zmq_client_system(flecs::world &world) -> flecs::system {
  // 创建初始化 ZMQ 客户端的系统
  IntoSystemBuilder system(init_zmq);
  return system.build(world);
}

void ZmqClientPlugin::build(flecs::world &world) {
  // 构建函数，用于在ECS世界中构建 ZMQ 客户端

  init_zmq_client_system(world).depends_on(flecs::OnStart);
  user_verify_system(world).depends_on(flecs::OnStart);
  IntoSystemBuilder system(control_cmd);
  IntoSystemBuilder system2(graph_show);

  system.build(world);
  system2.build(world);
}
