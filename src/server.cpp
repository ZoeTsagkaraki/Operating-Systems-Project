#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <pthread.h>
#include <cstdlib>
#include <fstream> 
#include <vector>
#include <sstream>

#define PORT 8080

using namespace std;

// Structure to represent a flight
struct Flight {
    string id;
    string from;
    string to;
    string dep_time;
    string arr_time;
    int total_seats;
    int available_seats;
};

// Global mutex for thread-safe file operations
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

// Search for direct flights
string search_flights(string from, string to) {
    ifstream infile("flights.txt");
    string line, results = "";
    while (getline(infile, line)) {
        stringstream ss(line); string segment; vector<string> seglist;
        while(getline(ss, segment, ';')) seglist.push_back(segment);
        if (seglist.size() >= 7 && seglist[1] == from && seglist[2] == to) {
            results += "Flight: " + seglist[0] + " | Seats: " + seglist[6] + "\n";
        }
    }
    infile.close();
    return results.empty() ? "No flights found." : results;
}

// Load all flights from file into a vector of Flight structs
vector<Flight> load_flights() {
    vector<Flight> all_flights;
    ifstream infile("flights.txt");
    string line;
    while (getline(infile, line)) {
        stringstream ss(line); string segment; vector<string> seglist;
        while(getline(ss, segment, ';')) seglist.push_back(segment);
        if (seglist.size() >= 7) {
            all_flights.push_back({seglist[0], seglist[1], seglist[2], seglist[3], seglist[4], stoi(seglist[5]), stoi(seglist[6])});
        }
    }
    infile.close();
    return all_flights;
}

// Search for connecting flights (A -> B -> C)
string find_connections(string start, string end) {
    vector<Flight> flights = load_flights();
    string results = "";
    
    // Iterate to find a connection path
    for (auto& f1 : flights) {
        if (f1.from == start) {
            for (auto& f2 : flights) {
                if (f2.from == f1.to && f2.to == end) {
                    results += "Connection found: " + f1.id + " (" + f1.from + "->" + f1.to + ") + " 
                               + f2.id + " (" + f2.from + "->" + f2.to + ")\n";
                }
            }
        }
    }
    return results.empty() ? "No connecting flights found." : results;
}

void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc; free(socket_desc);
    char buffer[1024] = {0}; read(sock, buffer, 1024);
    
    stringstream ss(buffer); string cmd; ss >> cmd;
    string response;

    if (cmd == "SEARCH") {
        string c1, c2; ss >> c1 >> c2;
        response = search_flights(c1, c2);
    } 
    // Handle connecting flight request
    else if (cmd == "CONN") {
        string c1, c2; ss >> c1 >> c2;
        response = find_connections(c1, c2);
    } 
    // Handle booking request
    else if (cmd == "BOOK") {
        string id, pass, country, name; ss >> id >> pass >> country >> name;
        
        pthread_mutex_lock(&file_mutex);
        
        vector<string> all_lines;
        string line;
        bool found = false;
        ifstream infile("flights.txt");
        
        while (getline(infile, line)) {
            stringstream ss_line(line);
            string segment; vector<string> seglist;
            while(getline(ss_line, segment, ';')) seglist.push_back(segment);
            
            if (seglist[0] == id && stoi(seglist[6]) > 0) {
                int seats = stoi(seglist[6]) - 1;
                seglist[6] = to_string(seats);
                found = true;
            }
            
            string newline = seglist[0];
            for(size_t i=1; i<seglist.size(); i++) newline += ";" + seglist[i];
            all_lines.push_back(newline);
        }
        infile.close();

        if (found) {
            ofstream outfile_f("flights.txt");
            for(auto &l : all_lines) outfile_f << l << endl;
            outfile_f.close();

            ofstream outfile_b("bookings.txt", ios::app);
            outfile_b << id << ";" << pass << ";" << country << ";" << name << endl;
            outfile_b.close();
            response = "Booking confirmed for flight " + id + ". Seats reduced.";
        } else {
            response = "Booking failed: Flight not found or sold out.";
        }
        
        pthread_mutex_unlock(&file_mutex);
    }

    send(sock, response.c_str(), response.length(), 0);
    close(sock); pthread_exit(NULL);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1; socklen_t addrlen = sizeof(address);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    address.sin_family = AF_INET; address.sin_addr.s_addr = INADDR_ANY; address.sin_port = htons(PORT);
    
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);
    
    cout << "Airline Server running and ready for bookings..." << endl;
    
    while (true) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        int *ptr = (int *)malloc(sizeof(int)); *ptr = new_socket;
        
        pthread_t tid; pthread_create(&tid, NULL, handle_client, (void*)ptr);
        pthread_detach(tid);
    }
    return 0;
}