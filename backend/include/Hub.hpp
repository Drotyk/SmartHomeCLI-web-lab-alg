#pragma once
#include "Devices.hpp"
#include <map>
#include <memory>
#include <vector>

// Factory
class DeviceFactory {
public:
    static std::shared_ptr<IDevice> createDevice(const std::string& type, const std::string& room, const std::string& name);
};

// Hub
class SmartHomeHub {
    std::map<int, std::shared_ptr<IDevice>> devices;
    int nextId = 1;
    const std::string dbFile = "database/smarthome_db.txt"; // Шлях змінено

public:
    void addDevice(std::shared_ptr<IDevice> device);
    void loadDevice(int id, std::shared_ptr<IDevice> device);
    void removeDevice(int id);
    std::shared_ptr<IDevice> getDevice(int id);
    void updateDeviceState();
    std::string getAllData();
    
    void saveToFile();
    void loadFromFile();
};