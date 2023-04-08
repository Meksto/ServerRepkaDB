#include "server.h"

int main(int argc, char* argv[]) {
    //Log config section
    spdlog::set_level(spdlog::level::debug); // Установка уровня отладки

    if (argc != 3) {
        spdlog::warn("Usage: {0} <port> <maximum_clients>", argv[0]);
        return 0;
    }

    IRepka repka1;
    int16_t port = std::stoi(argv[1]);
    uint32_t MAX_CONNECTIONS = std::atoi(argv[2]);


    // create socket on server (allocate memory for socket and set type);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        spdlog::critical("Failed to create socket on server-side");
        return 1;
    }

    //bind socket to port
    struct sockaddr_in server_address {};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port); //transform int to TCP/IP bytes number
    if (bind(server_fd, reinterpret_cast<sockaddr*>(&server_address),sizeof(server_address)) == -1) {
        spdlog::critical("Failed to bind socket to port {}", port);
        return 1;
    }

    //listen for incoming connections
    if (listen(server_fd, MAX_CONNECTIONS) == -1) {
        spdlog::critical("Failed to listen for connections");
        return 1;
    }
    spdlog::info("Server started");
    spdlog::info("Listenning for incoming connections on port: {}", port);
    
    //accept incoming connections and create a new thread for handling
    while (true) {

        struct sockaddr_in client_address {};
        socklen_t client_address_len = sizeof(client_address);

        int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_address),&client_address_len);
        if (client_fd == -1) {
            spdlog::error("Failed to accept incoming connection");
            continue;
        }

        if (active_connections >= MAX_CONNECTIONS) {
            too_many_connections(client_fd);
            continue;
        }
        create_thread_handler(client_fd,repka1);
    }

    

close(server_fd);
return 0;
}