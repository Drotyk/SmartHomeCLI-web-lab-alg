#include "../include/Hub.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void runServer() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int port = 8080;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) exit(EXIT_FAILURE);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << "Smart Home Server (Structured) running on 8080..." << std::endl;

    SmartHomeHub hub;
    hub.loadFromFile();

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) continue;

        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);
        std::string request(buffer);
        std::string response = "OK";
        
        std::stringstream ss(request);
        std::string action, p1, p2, p3;
        std::getline(ss, action, '|');
        std::getline(ss, p1, '|');
        std::getline(ss, p2, '|');
        std::getline(ss, p3, '|');

        if (action == "status") {
            response = hub.getAllData();
        } 
        else if (action == "create") {
            auto newDev = DeviceFactory::createDevice(p1, p2, p3); 
            if (newDev) {
                hub.addDevice(newDev);
                response = "Created";
                std::cout << "Added: " << p2 << " - " << p3 << std::endl;
            } else { response = "Error"; }
        }
        else if (action == "delete") {
            try { hub.removeDevice(std::stoi(p1)); } catch(...) {}
            response = "Deleted";
        }
        else {
            try {
                int id = std::stoi(p1);
                auto device = hub.getDevice(id);
                if (device) {
                    if (action == "switch") {
                        if (p2 == "on") device->turnOn(); else device->turnOff();
                        hub.updateDeviceState();
                    } 
                    else if (action == "set") {
                        try { 
                            device->setValue(std::stoi(p2)); 
                            hub.updateDeviceState();
                        } catch(...) {}
                    }
                } else { response = "Device not found"; }
            } catch (...) { response = "Invalid ID"; }
        }
        send(new_socket, response.c_str(), response.length(), 0);
        close(new_socket);
    }
}

int main() { runServer(); return 0; }