#pragma once
#include <spdlog/spdlog.h>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include "IRepka.h"
#include "command_responsive.h"
#ifndef SERVER_H
#define SERVER_H
#define MAX_MSG 256

static std::atomic<int> active_connections = 0;


void client_handler(int client_fd, IRepka& repka1);
bool send_connection_succsess(int& client_fd);
void too_many_connections(int &client_fd);
bool create_thread_handler(int& client_fd, IRepka& repka1);

bool send_connection_succsess(int& client_fd) {
    std::string cs_msg = "OK";
    int bytes_sent = send(client_fd,cs_msg.c_str(), cs_msg.length(),0);
        if (bytes_sent == -1) {
            spdlog::error("Failed to send data to client {}", client_fd);
            close(client_fd);
            return false;
        }
    return true;
}

void too_many_connections(int &client_fd) {
    const char buf[16] = "E:TMC";
    std::string msg = "E:TMC";
    if (send(client_fd, buf, sizeof(buf), 0) == -1) {
            spdlog::error("Failed to send data to client {}", client_fd);
            close(client_fd);
            return;
        }
    usleep(100000);
    close(client_fd);
    return;
}

bool create_thread_handler(int& client_fd, IRepka& repka1) {
        //create thread
        std::thread client_thread(client_handler, client_fd, std::ref(repka1));
        client_thread.detach();
        ++active_connections;
        return true;
}

void client_handler(int client_fd, IRepka& repka1) {
    spdlog::info("Client: {} connected", client_fd);
    char buf[MAX_MSG] {}; // to recieve data from client
    char msg[MAX_MSG] {}; // to send data to client

    // send connection_succsess
    send_connection_succsess(client_fd);

    while (true) {
            //recieve data from client
        memset(buf, 0 , sizeof(buf));
        size_t num_bytes = recv(client_fd, buf, sizeof(buf),0);
        if (num_bytes == -1) {
            spdlog::error("Failed to receive data from client: {}", client_fd);
            close(client_fd);
            return;
        }
        spdlog::info("=Received: {0} bytes from client: {1}", num_bytes, client_fd);
        spdlog::info("={}", buf);
        
        //chek if client is disconnectd
        if (num_bytes == 0) {
            break;
        }

        //send response to client
        // # to do: answer to client from DB
        
        memset(msg,0,sizeof(msg));
        std::string result = command_response(repka1,buf);
        if (!result.size() > MAX_MSG) {
            result = "Error: Message too long";
        }
        std::strcpy(msg,result.c_str());


        if (send(client_fd, msg, sizeof(msg), 0) == -1) {
            spdlog::error("Failed to send data to client {}", client_fd);
            close(client_fd);
            return;
        }
        spdlog::info("Succsessfully sent response to client: {}", client_fd);
    }

    // code to close Connection
    close(client_fd);
    --active_connections;
    spdlog::info("Client(socket): {} has been disconnected", client_fd);
    return;


}







#endif //SERVER_H