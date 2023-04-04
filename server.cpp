#include <iostream>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <condition_variable>
#include <atomic>
#include "IRepka.h"
#include "command_responsive.h"

#define MAX_MSG 256
static std::atomic<int> active_connections = 0;

void too_many_connections(int &client_fd) {
    const char buf[16] = "E:TMC";
    std::string msg = "E:TMC";
    if (send(client_fd, buf, sizeof(buf), 0) == -1) {
            std::cerr << "Failed to send data to client " << client_fd << std::endl;
            close(client_fd);
            return;
        }
    usleep(100000);
    close(client_fd);
    return;
}

bool send_connection_succsess(int& client_fd) {
    std::string cs_msg = "OK";
    int bytes_sent = send(client_fd,cs_msg.c_str(), cs_msg.length(),0);
        if (bytes_sent == -1) {
            std::cerr << "Failed to sent message to server" << std::endl;
            close(client_fd);
            return false;
        }
    return true;
}

// function that handles communications with a every single client
void client_handler(int client_fd, IRepka& repka1) {
    std::cout << "Client " << client_fd << " connected" << std::endl;
    char buf[MAX_MSG] {}; // to recieve data from client
    char msg[MAX_MSG] {}; // to send data to client

    // send connection_succsess
    send_connection_succsess(client_fd);

    while (true) {
            //recieve data from client
        memset(buf, 0 , sizeof(buf));
        size_t num_bytes = recv(client_fd, buf, sizeof(buf),0);
        if (num_bytes == -1) {
            std::cerr << "Failed to receive data from client: " << client_fd << std::endl;
            close(client_fd);
            return;
        }
        std::cout << " =Received " << num_bytes << " bytes from client: " << client_fd << std::endl;
        std::cout << " =" << buf << std::endl;
        
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
            std::cerr << "Failed to send data to client " << client_fd << std::endl;
            close(client_fd);
            return;
        }
        std::cout << "Succsessfully sent response to client: " << client_fd << std::endl;
        
    }

    // code to close Connection
    close(client_fd);
    --active_connections;
    std::cout << "Client(socket) " << client_fd << " has been disconnected" << std::endl;
    return;


}



int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <maximum_clients>" << std::endl;
        return 0;
    }
    // Create database
    IRepka repka1;
    // get port and MAX_CONNECTIONS from argv[] and transform it to uint;
    int16_t port = std::stoi(argv[1]);
    uint32_t MAX_CONNECTIONS = std::atoi(argv[2]);

    // create socket on server (allocate memory for socket and set type);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket on server-side" << std::endl;
        return 1;
    }

    //bind socket to port
    struct sockaddr_in server_address {};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port); //transform int to TCP/IP bytes number

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&server_address),sizeof(server_address)) == -1) {
        std::cerr << "Failed to bind socket to port " << port << std::endl;
        return 1;
    }

    //listen for incoming connections

    if (listen(server_fd, MAX_CONNECTIONS) == -1) {
        std::cerr << "Failed to listen for connections" << std::endl;
        return 1;
    }
    std::cout << "Server started" << std::endl;
    std::cout << "Listenning for incoming connections on port: " << port << std::endl;
    
    //accept incoming connections and create a new thread for handling

    while (true) {

        struct sockaddr_in client_address {};
        socklen_t client_address_len = sizeof(client_address);

        int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_address),&client_address_len);
        if (client_fd == -1) {
            std::cerr << "Failed to accept incoming connection" << std::endl;
            continue;
        }

        if (active_connections >= MAX_CONNECTIONS) {
            too_many_connections(client_fd);
            continue;
        }



        //create thread
        std::thread client_thread(client_handler, client_fd, std::ref(repka1));
        client_thread.detach();
        ++active_connections;
    }

    




close(server_fd);
return 0;
}