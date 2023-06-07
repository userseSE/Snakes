#include "server.hpp"

#include "input.hpp"
#include "snake.hpp"
#include "system_helper.hpp"
#include "zmq.hpp"
#include "zstdcpp.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <vector>

using json = nlohmann::json;
using CollisionQuery =
    flecs::query<const raylib::Rectangle, const raylib::Color>;

enum { CONTROL, GRAPH, AUTH, REPLY, GRAPH_REPLY, AUTH_REPLY };

// 获取ip
void init_ip(flecs::iter &it) {
  // 构建JSON文件路径，相对于exe文件所在目录
  std::string jsonFilePath = "config_server.json";

  // 读取JSON文件
  std::ifstream file(jsonFilePath);

  // 解析JSON文件内容
  json data = json::parse(file);

  // 将解析后的JSON对象传递给UserDatabase，将其转换为UserDatabase对象
  auto ip = data["ip"];

  std::cout << ip << std::endl;

  file.close();

  it.world()
      .entity()
      .set<ZmqServerRef>(ZmqServerRef{std::make_shared<ZmqServer>()})
      .set<ServerAddress>(ip);
}

// 初始化server，绑定地址
void init_zmq_server(flecs::iter &it, ZmqServerRef *server,
                     ServerAddress *bind_address) {
  // printf("test server up:\n");
  for (auto i = 0; i < it.count(); i++) {
    // 通过循环遍历服务器的数量，依次将服务器绑定到对应的地址，并打印服务器地址
    server[i]->bind(bind_address[i].c_str());
    printf("test server up at %s\n", bind_address[i].c_str());
  }
}

auto handle_message(const json &message, flecs::iter &it) -> json {
  //->表示一个函数的返回类型
  // 首先将接收到的消息解析为 JSON 格式，然后打印消息内容

  auto &&json_msg = message; // 解析json

  auto cmdtype = json_msg["type"].get<int>(); // 获取json中的type
  json reply_msg;                             // 创建回复json
  reply_msg["type"] = REPLY;                  // 设置回复json的type
  reply_msg["code"] = "GOOD";                 // 设置回复json的code.
  switch (cmdtype) {                          // 根据type进行处理

  case (int)CONTROL: { // 如果是控制命令
    auto player_id = json_msg["id"].get<flecs::entity_t>(); // 获取玩家id
    auto cmd = json_msg["cmd"].get<int>();                  // 获取命令
    it.world().entity(player_id).set<Direction>(cmd); // 设置玩家的方向

    break;
  }

  case (int)GRAPH: {
    printf("prepare graph\n");
    // auto player_id = json_msg["id"].get<flecs::entity_t>(); //获取玩家id
    auto queryRect = it.system().get<CollisionQuery>();

    // auto queryRect = it.ctx<CollisionQuery>();
    printf("query graph\n");
    reply_msg["type"] = GRAPH_REPLY;
    reply_msg["rect"] = std::vector<raylib::Rectangle>();
    reply_msg["color"] = std::vector<raylib::Color>();

    queryRect->each(
        [&](const raylib::Rectangle &rect, const raylib::Color &color) {
          reply_msg["rect"].push_back(rect);
          reply_msg["color"].push_back(color);
        });
    break;
  }

  case (int)AUTH: {
    auto enteredUsername = json_msg["username"].get<std::string>();
    auto enteredPassword = json_msg["password"].get<std::string>();

    std::cout << enteredUsername << enteredPassword << std::endl;

    // 从world中获取UserDatabase单例
    const UserDatabase *ud = it.world().get<UserDatabase>();
    const json &accounts = (*ud)["accounts"];

    bool isAuthorized = false;

    // 从JSON数据中提取账户名和密码
    // 遍历账户列表
    for (const auto &account : accounts) {
      std::string username = account["username"];
      std::string password = account["password"];

      // 进行账号密码的判断逻辑
      if (username == enteredUsername && password == enteredPassword) {
        isAuthorized = true;
        break;
      }
    }
    if (isAuthorized) {
      // 账号密码匹配成功
      reply_msg["code"] = "SUCCESS";

      flecs::entity_t snake_id =
          it.world()
              .entity()
              .set<SnakeSpawn>(
                  SnakeSpawn{{TilePos{1, 3}, TilePos{1, 2}, TilePos{1, 1}}})
              .set<Direction>(Direction::DOWN);

      // printf("%llu\n", snake_id.id());

      reply_msg["id"] = static_cast<int>(snake_id);

      std::cout << reply_msg["id"] << std::endl;

    } else {
      // 账号密码匹配失败
      reply_msg["code"] = "FAILURE";
    }
    break;
  }
  }
  return reply_msg;
}

void reply_commands(flecs::iter &it, ZmqServerRef *servers) {
  // 用于接收消息并发送回复

  for (auto i = 0; i < it.count(); i++) {
    auto &server = servers[i];       // 获取server
    auto &socket = server->socket(); // 获取socket
    zstd buff;
    zmq::message_t message;    // 创建消息
    bool continue_loop = true; // 添加退出条件
    while (continue_loop) {
      // 接收一个消息，如果接收成功，则调用 handle_message
      // 函数处理消息并生成回复，最后将回复消息发送给客户端
      auto success = socket.recv(message, zmq::recv_flags::dontwait);
      // 接收消息,设置zmq_recv()函数在非阻塞模式下执行,如果在指定的socket中没有消息，zmq_recv()函数会执行失败
      // 阻塞模式下，当一个进程在执行输入/输出操作时，如果没有准备好数据，那么该进程将被挂起，直到数据准备好为止。
      // 非阻塞模式下，当一个进程在执行输入/输出操作时，如果没有准备好数据，那么该进程将立即返回一个错误代码，而不是被挂起。
      if (success.has_value()) { // 如果接收成功

        auto decompressed_data = buff.decompress(message.to_string_view());
        auto msg_json = json::parse(decompressed_data);
        auto reply = handle_message(msg_json, it); // 处理消息
        auto rmsg = reply.dump();
        auto compressed_data = buff.compress(rmsg, 3);
        zmq::message_t reply_msg{compressed_data};

        socket.send(reply_msg, zmq::send_flags::none); // 发送回复
      } else {
        continue_loop = false; // 没有消息时退出循环
      }
    }
  }
}

void init_user(flecs::iter &it) {
  UserDatabase ud;

  // 构建JSON文件路径，相对于exe文件所在目录
  std::string jsonFilePath = "key.json";

  // 读取JSON文件
  std::ifstream file(jsonFilePath);

  // 解析JSON文件内容
  json data = json::parse(file);

  // 将解析后的JSON对象传递给UserDatabase，将其转换为UserDatabase对象
  ud["accounts"] = data["accounts"];

  std::cout << ud << std::endl;

  file.close();

  // it.world().set(ud);
  it.world().set<UserDatabase>(std::move(ud));
}

auto init_user_system(flecs::world &world) -> flecs::system {
  IntoSystemBuilder system(init_user);
  return system.build(world);
}

auto init_ip_system(flecs::world &world) -> flecs::system {
  IntoSystemBuilder system(init_ip);
  return system.build(world);
}

auto init_zmq_server_system(flecs::world &world) -> flecs::system {
  // 创建初始化 ZMQ 服务器的系统
  IntoSystemBuilder system(init_zmq_server); // 创建系统
  return system.build(world);                // 构建系统
}

auto reply_commands_system(flecs::world &world) -> flecs::system {
  // 创建接收消息并发送回复的系统
  IntoSystemBuilder system(reply_commands); // 创建系统
  auto a = world.query<const raylib::Rectangle, const raylib::Color>();
  auto sys = system.build(world);
  sys.set(a);

  return sys;
}

void ZmqServerPlugin::build(flecs::world &world) {
  // 创建 ZMQ 服务器
  // 接受一个 flecs::world
  // 类型的参数，在该世界中创建初始化服务器系统和回复命令系统，分别指定它们在
  // flecs::OnStart 和 flecs::PreUpdate 阶段运行

  auto sys = init_ip_system(world).depends_on(flecs::OnStart);
  init_user_system(world).depends_on(flecs::OnStart);
  init_zmq_server_system(world).depends_on(sys);
  reply_commands_system(world).depends_on(flecs::PreUpdate);
}