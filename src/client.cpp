#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed" << endl;
        return -1;
    }

    // Display menu
    cout << "1. Search Flights\n2. Book Flight\n3. Find Connections\nChoice: ";
    int choice; 
    cin >> choice;
    string message;

    // Build the message based on user choice
    if (choice == 1) {
        string c1, c2;
        cout << "Enter Departure and Arrival: "; cin >> c1 >> c2;
        message = "SEARCH " + c1 + " " + c2;
    } else if (choice == 2) {
        string id, pass, country, name;
        cout << "Enter FlightID Passport Country Name: "; 
        cin >> id >> pass >> country >> name;
        message = "BOOK " + id + " " + pass + " " + country + " " + name;
    } else if (choice == 3) {
        string c1, c2;
        cout << "Enter Start and End City for connections: "; cin >> c1 >> c2;
        message = "CONN " + c1 + " " + c2;
    } else {
        cout << "Invalid choice." << endl;
        close(sock);
        return 0;
    }

    // Send the request to server
    send(sock, message.c_str(), message.length(), 0);
    
    // Read the response
    read(sock, buffer, 1024);
    cout << "Server response:\n" << buffer << endl;
    
    close(sock);
    return 0;
}