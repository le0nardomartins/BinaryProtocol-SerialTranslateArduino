#ifndef BINARY_PROTOCOL_H
#define BINARY_PROTOCOL_H

#include <Arduino.h>

class BinaryProtocol {
public:
    static const uint8_t SYNC_BYTE = 0xAA;
    static const size_t FRAME_SIZE = 9;

    struct MotionCommand {
        bool valid;
        bool isG1;
        bool dirX;
        bool dirY;
        bool dirZ;
        uint16_t stepX;
        uint16_t stepY;
        uint16_t stepZ;
        uint8_t flags;
        uint8_t checksum;
    };

    explicit BinaryProtocol(HardwareSerial& serialPort);

    void begin(unsigned long baudRate);
    bool update(MotionCommand& outCommand);

    static uint8_t computeChecksum(const uint8_t* frame);
    static bool decodeFrame(const uint8_t* frame, MotionCommand& outCommand);

private:
    HardwareSerial& serial;

    uint8_t rxBuffer[FRAME_SIZE];
    size_t rxIndex;
    bool syncing;

    bool readFrame(uint8_t* outFrame);
};

#endif