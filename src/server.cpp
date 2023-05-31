#include "server.hpp"
#include "flecs/addons/cpp/c_types.hpp"
#include "flecs/addons/cpp/mixins/pipeline/decl.hpp"
#include "input.hpp"
#include "json_conversion.hpp"
#include "system_helper.hpp"
#include "zmq.hpp"
#include <nlohmann/json.hpp>
#include <stdio.h>

using json = nlohmann::json;
using CollisionQuery = flecs::query<const Rectangle, const Color>;

enum { CONTROL, GRAPH, AUTH, REPLY, GRAPH_REPLY, AUTH_REPLY };
// 初始化server，绑定地址
void init_zmq_server(flecs::iter &it, ZmqServerRef *server,
                     ServerAddress *bind_address) {
  printf("test server up:\n");
  for (auto i = 0; i < it.count(); i++) {
    // 通过循环遍历服务器的数量，依次将服务器绑定到对应的地址，并打印服务器地址
    server[i]->bind(bind_address[i].c_str());
    printf("test server up at %s\n", bind_address[i].c_str());
  }
}

auto handle_message(zmq::message_t &message, flecs::iter &it, CollisionQuery* q)
    -> zmq::message_t {
  //->表示一个函数的返回类型
  // 首先将接收到的消息解析为 JSON 格式，然后打印消息内容
  printf("test0");
  auto json_msg = json::parse(message.to_string_view()); // 解析json
  printf("test1");
  printf("%s\n", message.to_string_view().data());       // 打印json
  auto cmdtype = json_msg["type"].get<int>();            // 获取json中的type
  json reply_msg;                                        // 创建回复json
  reply_msg["type"] = REPLY;  // 设置回复json的type
  reply_msg["code"] = "GOOD"; // 设置回复json的code.
  switch (cmdtype) {          // 根据type进行处理

  case (int)CONTROL: { // 如果是控制命令
    auto player_id = json_msg["id"].get<flecs::entity_t>(); // 获取玩家id
    auto cmd = json_msg["cmd"].get<int>();                  // 获取命令
    it.world().entity(player_id).set<Direction>(cmd); // 设置玩家的方向

    break;
  }

  case (int)GRAPH: {
    printf("prepare graph\n");
    // auto player_id = json_msg["id"].get<flecs::entity_t>(); //获取玩家id
    // auto queryRect = it.world().query<raylib::Rectangle, raylib::Color>();
    auto queryRect = q->query<raylib::Rectangle, raylib::Color>();
    printf("query graph\n");
    queryRect.each([&](raylib::Rectangle &rect, raylib::Color &color) {
      reply_msg["type"] = GRAPH_REPLY;
      reply_msg["rect"].push_back(rect);
      reply_msg["color"].push_back(color);
    });
    break;
  }

  case (int)AUTH:

    break;
  }
  return zmq::message_t{reply_msg.dump()};
}

void reply_commands(flecs::iter &it, ZmqServerRef *servers) {
  // 用于接收消息并发送回复

  for (auto i = 0; i < it.count(); i++) {
    auto &server = servers[i];       // 获取server
    auto &socket = server->socket(); // 获取socket
    zmq::message_t message;          // 创建消息
    while (true) {
      // 接收一个消息，如果接收成功，则调用 handle_message
      // 函数处理消息并生成回复，最后将回复消息发送给客户端
      auto success = socket.recv(message, zmq::recv_flags::dontwait);
      // 接收消息,设置zmq_recv()函数在非阻塞模式下执行,如果在指定的socket中没有消息，zmq_recv()函数会执行失败
      // 阻塞模式下，当一个进程在执行输入/输出操作时，如果没有准备好数据，那么该进程将被挂起，直到数据准备好为止。
      // 非阻塞模式下，当一个进程在执行输入/输出操作时，如果没有准备好数据，那么该进程将立即返回一个错误代码，而不是被挂起。
      if (success.has_value()) {                   // 如果接收成功
        auto reply = handle_message(message, it);  // 处理消息
        socket.send(reply, zmq::send_flags::none); // 发送回复
      } else {
        break;
      }
    }
  }
}
auto init_zmq_server_system(flecs::world &world) -> flecs::system {
  // 创建初始化 ZMQ 服务器的系统
  IntoSystemBuilder system(init_zmq_server); // 创建系统
  return system.build(world);                // 构建系统
}

auto reply_commands_system(flecs::world &world) -> flecs::system {
  // 创建接收消息并发送回复的系统
  IntoSystemBuilder system(reply_commands); // 创建系统
  return system.build(world);
}

void ZmqServerPlugin::build(flecs::world &world) {
  // 创建 ZMQ 服务器
  // 接受一个 flecs::world
  // 类型的参数，在该世界中创建初始化服务器系统和回复命令系统，分别指定它们在
  // flecs::OnStart 和 flecs::PreUpdate 阶段运行
  init_zmq_server_system(world).depends_on(flecs::OnStart);
  reply_commands_system(world).depends_on(flecs::PreUpdate);
}