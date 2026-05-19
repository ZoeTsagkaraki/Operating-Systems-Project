#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#define PORT 8080

using namespace std;

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error: Socket creation failed");
        return 1;
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Error: Bind failed");
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        perror("Error: Listen failed");
        return 1;
    }
    
    cout << "Server started successfully!" << endl;
    cout << "Waiting for clients on port " << PORT << "..." << endl;

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
        perror("Error: Accept failed");
        return 1;
    }

    cout << "\nSUCCESS: A new client just connected!" << endl;
    
    const char *hello = "Welcome to the airline booking system!";
    send(new_socket, hello, strlen(hello), 0);
    
    close(new_socket);
    close(server_fd);
    
    cout << "Server closed." << endl;
    return 0;
}