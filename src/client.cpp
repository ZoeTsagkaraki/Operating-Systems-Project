#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "\n[!] Connection Failed to Server." << endl;
        return -1;
    }

    int choice = 0;
    cout << "========================================" << endl;
    cout << "   Welcome to the Airline Reservation   " << endl;
    cout << "========================================" << endl;

    while (true) {
        cout << "\n---------------- MENU ------------------" << endl;
        cout << " 1. Search Direct Flights" << endl;
        cout << " 2. Book a Flight" << endl;
        cout << " 3. Find Connections" << endl;
        cout << " 4. Exit" << endl;
        cout << "----------------------------------------" << endl;
        cout << " Choice: ";
        cin >> choice;
        
        if (choice == 4) {
            cout << ">> Thank you for using our service. Goodbye!" << endl;
            break;
        }

        string message;
        if (choice == 1) {
            string c1, c2;
            cout << " > Enter Departure and Arrival city: "; cin >> c1 >> c2;
            message = "SEARCH " + c1 + " " + c2;
        } else if (choice == 2) {
            string id, pass, country, name;
            cout << " > Enter FlightID, Passport, Country, Name: "; 
            cin >> id >> pass >> country >> name;
            message = "BOOK " + id + " " + pass + " " + country + " " + name;
        } else if (choice == 3) {
            string c1, c2;
            cout << " > Enter Start and End City: "; cin >> c1 >> c2;
            message = "CONN " + c1 + " " + c2;
        } else {
            cout << " [!] Invalid choice. Please try again." << endl;
            continue;
        }

        send(sock, message.c_str(), message.length(), 0);
        
        char buffer[1024] = {0};
        read(sock, buffer, 1024);
        cout << "\n[Server Response]\n" << buffer << endl;
    }
    
    close(sock);
    return 0;
}