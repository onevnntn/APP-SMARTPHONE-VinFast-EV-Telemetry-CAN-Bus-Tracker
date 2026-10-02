#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <driver/twai.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

// Biến theo dõi vận hành
uint32_t tripStartTime = 0;
bool ignitionOn = false;
uint8_t soc = 0;
float packVoltage = 0.0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) { deviceConnected = true; };
    void onDisconnect(BLEServer* pServer) { deviceConnected = false; }
};

void setupBLE() {
    BLEDevice::init("VinFast_EV_Tracker");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
    pCharacteristic->addDescriptor(new BLE2902());
    pService->start();
    pServer->getAdvertising()->start();
}

void processCAN() {
    twai_message_t msg;
    if (twai_receive(&msg, pdMS_TO_TICKS(5)) == ESP_OK) {
        if (msg.identifier == 0x101 && msg.data_length_code >= 1) {
            bool prevIgnition = ignitionOn;
            ignitionOn = (msg.data[0] == 0x11);
            if (!prevIgnition && ignitionOn) tripStartTime = millis(); // Bắt đầu chuyến đi
        }
        if (msg.identifier == 0x309 && msg.data_length_code >= 4) {
            uint16_t raw = (msg.data[2] << 8) | msg.data[3];
            packVoltage = (raw / 1000.0f) * 2.0f;
        }
        if (msg.identifier == 0x322 && msg.data_length_code >= 2) {
            soc = (msg.data[0] + msg.data[1]) / 2;
        }
    }
}

void sendTelemetryToApp() {
    if (!deviceConnected) return;
    
    uint32_t driveDurationSec = ignitionOn ? (millis() - tripStartTime) / 1000 : 0;
    
    // Chuỗi JSON gửi lên Mobile App
    String payload = "{\"ign\":" + String(ignitionOn ? 1 : 0) +
                     ",\"dur\":" + String(driveDurationSec) +
                     ",\"soc\":" + String(soc) +
                     ",\"v\":" + String(packVoltage, 2) + "}";
                     
    pCharacteristic->setValue(payload.c_str());
    pCharacteristic->notify();
}