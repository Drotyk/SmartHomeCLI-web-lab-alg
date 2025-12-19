#include "../include/Hub.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// --- Factory Implementation ---
std::shared_ptr<IDevice> DeviceFactory::createDevice(const std::string& type, const std::string& room, const std::string& name) {
    if (type == "light") return std::make_shared<SimpleLight>(room, name);
    if (type == "dimmer") return std::make_shared<DimmableLight>(room, name);
    if (type == "fan") return std::make_shared<Fan>(room, name);
    if (type == "ac") return std::make_shared<AirConditioner>(room, name);
    return nullptr;
}

// --- Hub Implementation ---
void SmartHomeHub::addDevice(std::shared_ptr<IDevice> device) {
    devices[nextId++] = device;
    saveToFile();
}

void SmartHomeHub::loadDevice(int id, std::shared_ptr<IDevice> device) {
    devices[id] = device;
    if (id >= nextId) nextId = id + 1;
}

void SmartHomeHub::removeDevice(int id) {
    devices.erase(id);
    saveToFile();
}

std::shared_ptr<IDevice> SmartHomeHub::getDevice(int id) {
    if (devices.find(id) != devices.end()) return devices[id];
    return nullptr;
}

void SmartHomeHub::updateDeviceState() {
    saveToFile();
}

std::string SmartHomeHub::getAllData() {
    std::string data;
    for (const auto& pair : devices) {
        data += std::to_string(pair.first) + "|" + 
                pair.second->getRoom() + "|" +
                pair.second->getName() + "|" +
                pair.second->getUIType() + "|" + 
                pair.second->getStatus() + "|" + 
                pair.second->getModel() + ";";
    }
    return data.empty() ? "empty" : data;
}

void SmartHomeHub::saveToFile() {
    std::ofstream file(dbFile);
    if (file.is_open()) {
        for (const auto& pair : devices) {
            file << pair.first << "|"
                 << pair.second->getFactoryType() << "|"
                 << pair.second->getRoom() << "|"
                 << pair.second->getName() << "|"
                 << pair.second->getRawValue() << "|"
                 << (pair.second->getIsOn() ? "1" : "0") << "\n";
        }
        file.close();
        std::cout << "[DB] Saved to " << dbFile << std::endl;
    } else {
        std::cerr << "[Error] Cannot save to " << dbFile << ". Check if 'database' folder exists!" << std::endl;
    }
}

void SmartHomeHub::loadFromFile() {
    std::ifstream file(dbFile);
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> parts;
            while(std::getline(ss, segment, '|')) parts.push_back(segment);

            if (parts.size() >= 6) {
                try {
                    int id = std::stoi(parts[0]);
                    auto newDev = DeviceFactory::createDevice(parts[1], parts[2], parts[3]);
                    if (newDev) {
                        newDev->setValue(std::stoi(parts[4]));
                        if (parts[5] == "1") newDev->turnOn(); else newDev->turnOff();
                        loadDevice(id, newDev);
                    }
                } catch (...) {}
            }
        }
        file.close();
        std::cout << "[DB] Loaded data." << std::endl;
    }
}