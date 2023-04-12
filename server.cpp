#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include "IRepka.h"
#include "command_responsive.h"
#define MAX_MSG 256
static std::atomic<int> active_connections = 0;

// spdlog init
auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/logSinked.txt", true);
auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>("server", spdlog::sinks_init_list({file_sink, console_sink}));

void client_handler(int client_fd, IRepka& repka1);
bool send_connection_succsess(int& client_fd);
void too_many_connections(int &client_fd);
bool create_thread_handler(int& client_fd, IRepka& repka1);

//spd_logger logger;

int main(int argc, char* argv[]) {
    // Log config section
    logger->set_level(spdlog::level::debug);
    logger->flush_on(spdlog::level::debug);
    // End log config section

    if (argc != 3) {
        spdlog::warn("Usage: {0} <port> <maximum_clients>", argv[0]);
        return 0;
    }

    IRepka repka1;
    int16_t port = std::stoi(argv[1]);
    uint32_t MAX_CONNECTIONS = std::atoi(argv[2]);
    logger->debug("================ PROGRAM LAUNCH ================");
    logger->info("Port {}", port);
    logger->info("Maximum connection is {}", MAX_CONNECTIONS);

    // create socket on server (allocate memory for socket and set type);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        logger->critical("Failed to create socket on server-side");
        return 1;
    }

    //bind socket to port
    struct sockaddr_in server_address {};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port); //transform int to TCP/IP bytes number
    if (bind(server_fd, reinterpret_cast<sockaddr*>(&server_address),sizeof(server_address)) == -1) {
        logger->critical("Failed to bind socket to port {}", port);
        return 1;
    }

    //listen for incoming connections
    if (listen(server_fd, MAX_CONNECTIONS) == -1) {
        logger->critical("Failed to listen for connections");
        return 1;
    }
    logger->info("Server started");
    logger->info("Listenning for incoming connections on port: {}", port);
    
    //accept incoming connections and create a new thread for handling
    while (true) {

        struct sockaddr_in client_address {};
        socklen_t client_address_len = sizeof(client_address);

        int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_address),&client_address_len);
        if (client_fd == -1) {
            logger->error("Failed to accept incoming connection");
            continue;
        }

        if (active_connections >= MAX_CONNECTIONS) {
            too_many_connections(client_fd);
            continue;
        }
        create_thread_handler(client_fd,repka1);
    }

    

close(server_fd);
logger->flush();
spdlog::drop_all(); // Закрытие всех логгеров
return 0;
}


bool send_connection_succsess(int& client_fd) {
    std::string cs_msg = "OK";
    int bytes_sent = send(client_fd,cs_msg.c_str(), cs_msg.length(),0);
        if (bytes_sent == -1) {
            logger->error("Failed to send data to client {}", client_fd);
            close(client_fd);
            return false;
        }
    return true;
}

void too_many_connections(int &client_fd) {
    const char buf[16] = "E:TMC";
    std::string msg = "E:TMC";
    if (send(client_fd, buf, sizeof(buf), 0) == -1) {
            logger->error("Failed to send data to client {}", client_fd);
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
        logger->info("Client thread created succsessful for client {}", client_fd);
        return true;
}

void client_handler(int client_fd, IRepka& repka1) {
    logger->set_level(spdlog::level::debug);
    logger->info("Client: {} connected", client_fd);
    char buf[MAX_MSG] {}; // to recieve data from client
    char msg[MAX_MSG] {}; // to send data to client

    // send connection_succsess
    send_connection_succsess(client_fd);

    while (true) {
            //recieve data from client
        memset(buf, 0 , sizeof(buf));
        size_t num_bytes = recv(client_fd, buf, sizeof(buf),0);
        if (num_bytes == -1) {
            logger->error("Failed to receive data from client: {}", client_fd);
            close(client_fd);
            return;
        }
        logger->info("=Received: {0} bytes from client: {1}", num_bytes, client_fd);
        logger->info("buffer={}", buf);
        
        //chek if client is disconnectd
        if (num_bytes == 0) {
            break;
        }

        //send response to client
        memset(msg,0,sizeof(msg));
        std::string result = command_response(repka1,buf);
        if (!result.size() > MAX_MSG) {
            result = "Error: Message too long";
            logger->error("Message from Server to Client is too big");
        }
        std::strcpy(msg,result.c_str());


        if (send(client_fd, msg, sizeof(msg), 0) == -1) {
            logger->error("Failed to send data to client {}", client_fd);
            close(client_fd);
            return;
        }
        logger->info("Succsessfully sent response to client: {}", client_fd);
    }

    // code to close Connection
    close(client_fd);
    --active_connections;
    logger->info("Client(socket): {} has been disconnected", client_fd);
    return;


}