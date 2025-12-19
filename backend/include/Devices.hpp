#pragma once
#include <string>
#include <iostream>

// --- Abstract Interface ---
class IDevice {
protected:
    std::string room;
    std::string name;
public:
    IDevice(std::string r, std::string n) : room(r), name(n) {}
    virtual void turnOn() = 0;
    virtual void turnOff() = 0;
    virtual void setValue(int val) = 0;
    virtual std::string getStatus() const = 0;
    virtual std::string getUIType() const = 0;
    virtual std::string getModel() const = 0;
    virtual std::string getFactoryType() const = 0;
    virtual int getRawValue() const = 0;
    virtual bool getIsOn() const = 0;
    
    std::string getRoom() const { return room; }
    std::string getName() const { return name; }
    virtual ~IDevice() = default;
};

// --- Concrete Devices ---
class SimpleLight : public IDevice {
    bool isOn = false;
public:
    using IDevice::IDevice;
    void turnOn() override { isOn = true; }
    void turnOff() override { isOn = false; }
    void setValue(int val) override { 
        if(val > 0) isOn = true; 
        else isOn = false; 
    }
    std::string getStatus() const override { return isOn ? "ON" : "OFF"; }
    std::string getUIType() const override { return "switch"; }
    std::string getModel() const override { return "Standard Lamp"; }
    std::string getFactoryType() const override { return "light"; }
    int getRawValue() const override { return isOn ? 1 : 0; }
    bool getIsOn() const override { return isOn; }
};

class DimmableLight : public IDevice {
    bool isOn = false;
    int brightness = 100;
public:
    using IDevice::IDevice;
    void turnOn() override { isOn = true; }
    void turnOff() override { isOn = false; }
    
    // --- ВИПРАВЛЕНО ТУТ ---
    void setValue(int val) override { 
        if (val < 0) val = 0; 
        if (val > 100) val = 100;
        
        brightness = val;
        if (brightness > 0) isOn = true;
    }
    
    std::string getStatus() const override { return isOn ? std::to_string(brightness) + "%" : "OFF"; }
    std::string getUIType() const override { return "slider"; }
    std::string getModel() const override { return "Dimmer"; }
    std::string getFactoryType() const override { return "dimmer"; }
    int getRawValue() const override { return brightness; }
    bool getIsOn() const override { return isOn; }
};

class Fan : public IDevice {
    bool isRunning = false;
public:
    using IDevice::IDevice;
    void turnOn() override { isRunning = true; }
    void turnOff() override { isRunning = false; }
    void setValue(int val) override { }
    std::string getStatus() const override { return isRunning ? "Spinning" : "Stopped"; }
    std::string getUIType() const override { return "switch"; }
    std::string getModel() const override { return "Ventilation"; }
    std::string getFactoryType() const override { return "fan"; }
    int getRawValue() const override { return isRunning ? 1 : 0; }
    bool getIsOn() const override { return isRunning; }
};

class AirConditioner : public IDevice {
    bool isOn = false;
    int temperature = 22;
public:
    using IDevice::IDevice;
    void turnOn() override { isOn = true; }
    void turnOff() override { isOn = false; }
    
    // --- ВИПРАВЛЕНО ТУТ ---
    void setValue(int val) override {
        if (val < 16) val = 16; 
        if (val > 30) val = 30;
        temperature = val;
    }
    
    std::string getStatus() const override { return !isOn ? "OFF" : "Cooling " + std::to_string(temperature) + "C"; }
    std::string getUIType() const override { return "thermostat"; }
    std::string getModel() const override { return "Climate Control"; }
    std::string getFactoryType() const override { return "ac"; }
    int getRawValue() const override { return temperature; }
    bool getIsOn() const override { return isOn; }
};