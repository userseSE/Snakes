#include "client.hpp"
#include "Color.hpp"
#include "flecs/addons/cpp/c_types.hpp"
#include "flecs/addons/cpp/mixins/pipeline/decl.hpp"
#include "input.hpp"
#include "json_conversion.hpp"
#include "server.hpp"
#include "system_helper.hpp"
#include "zmq.hpp"
#include <corecrt_malloc.h>
#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include "zstdcpp.hpp"
using json = nlohmann::json;

enum { CONTROL, GRAPH, AUTH, REPLY, GRAPH_REPLY, AUTH_REPLY };
void init_zmq(flecs::iter &it) {
  // 初始化 ZMQ 客户端并连接到服务器
  printf("connect to server\n");
  auto &client = *it.world().get_mut<ZmqClientRef>();        // 获取客户端
  client->connect(it.world().get<ServerAddress>()->c_str()); // 连接服务器

}

void control_cmd(flecs::iter &it, Direction *directions,
                 SnakeController *controller) {
  // 发送控制命令到服务器

  auto &client = *it.world().get_mut<ZmqClientRef>(); // 获取客户端
  json cmd;
  cmd["type"] = CONTROL;
  cmd["id"] = controller[0].player_id;
  cmd["cmd"] = directions[0];

  zstd buff;
  auto msg_data = buff.compress(cmd.dump(), 3);
  zmq::message_t msg{ msg_data};          // 创建消息
  auto &socket = client->socket();         // 获取socket
  

  socket.send(msg, zmq::send_flags::none); // 发送消息

  auto recv = socket.recv(msg, zmq::recv_flags::none); // 接收消息
}

void graph_show(flecs::iter &it) {

  // 请求服务器发送图形数据
  auto &client = *it.world().get_mut<ZmqClientRef>(); // 获取客户端
  json graph;
  graph["type"] = GRAPH;
  zstd buff;
  auto msg_data = buff.compress(graph.dump(), 3);
  zmq::message_t msg{msg_data};
  auto &socket = client->socket();         // 获取socket
        // 创建消息
  socket.send(msg, zmq::send_flags::none); // 发送消息

  std::string msg_str(static_cast<const char *>(msg.data()), msg.size());


  // 接收消息
  zmq::message_t recv_msg;
  auto recv = socket.recv(recv_msg, zmq::recv_flags::none); // 接收消息
   
  // 这次返回的结果需要进行处理，然后从接受的数据反序列化出json数据
  //  将接收到的消息解析为JSON对象
  json received_json = json::parse(buff.decompress(recv_msg.to_string_view()));

  // 从解析后的JSON对象中获取矩形和颜色信息
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
        auto e = it.world().entity();                // 创建一个实体
        e.set<raylib::Rectangle>(received_rects[i]); // 插入矩形组件
        e.set<raylib::Color>(received_colors[i]);    // 插入颜色组件
      }
      it.world().defer_end();
    }
  }
}

void ZmqClientPlugin::build(flecs::world &world) {
  // 构建函数，用于在ECS世界中构建 ZMQ 客户端

  init_zmq_client_system(world).depends_on(flecs::OnStart);
  IntoSystemBuilder system(control_cmd);
  IntoSystemBuilder system2(graph_show);

  system.build(world);
  system2.build(world);
}

auto init_zmq_client_system(flecs::world &world) -> flecs::system {
  // 创建初始化 ZMQ 客户端的系统
  IntoSystemBuilder system(init_zmq);
  return system.build(world);
}
