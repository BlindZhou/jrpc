//
// Created by frank on 17-12-31.
//

#include <iostream>

#include "HelloClientStub.h"

using namespace jrpc;

void run(HelloClientStub& client)
{
    static int counter = 0;
    counter++;
    client.Add(0, counter, [=](json::Value response, bool isError, bool timeout) {
        if (isError) {
            std::cout << "response: " << response["message"].getStringView();
        } else {
            int32_t result = response.getInt32();
            assert(result == counter);
            std::cout << "response: " << result << "\n";
        }
    });

    client.Hello("苟利国家生死以", [=](json::Value response, bool isError, bool timeout) {
        if (isError) {
            std::cout << "response: " << response["message"].getStringView();
        } else {
            std::cout << "response: " << response.getStringView() << "\n";
        }
    });
}

int main()
{
    EventLoop loop;
    InetAddress serverAddr(9877);
    HelloClientStub client(&loop, serverAddr);

    client.setConnectionCallback([&](const TcpConnectionPtr& conn) {
        if (conn->disconnected()) {
            loop.quit();
        }
        else {
            loop.runEvery(1s, [&] {
                run(client);
            });
        }
    });

    client.start();
    loop.loop();
}