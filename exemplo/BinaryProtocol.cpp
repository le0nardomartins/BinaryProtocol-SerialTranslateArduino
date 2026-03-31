#include "BinaryProtocol.h"

BinaryProtocol::BinaryProtocol(HardwareSerial& serialPort)
    : serial(serialPort), rxIndex(0), syncing(false) {
    for (size_t i = 0; i < FRAME_SIZE; i++) {
        rxBuffer[i] = 0;
    }
}

void BinaryProtocol::begin(unsigned long baudRate) {
    serial.begin(baudRate);
}

uint8_t BinaryProtocol::computeChecksum(const uint8_t* frame) {
    uint8_t chk = 0;
    for (size_t i = 1; i <= 7; i++) {
        chk ^= frame[i];
    }
    return chk;
}

bool BinaryProtocol::decodeFrame(const uint8_t* frame, MotionCommand& outCommand) {
    outCommand.valid = false;

    if (frame[0] != SYNC_BYTE) {
        return false;
    }

    const uint8_t expectedChk = computeChecksum(frame);
    if (expectedChk != frame[8]) {
        return false;
    }

    const uint8_t flags = frame[1];
    const uint16_t stepX = (uint16_t)frame[2] | ((uint16_t)frame[3] << 8);
    const uint16_t stepY = (uint16_t)frame[4] | ((uint16_t)frame[5] << 8);
    const uint16_t stepZ = (uint16_t)frame[6] | ((uint16_t)frame[7] << 8);

    outCommand.valid = true;
    outCommand.isG1 = (flags & (1 << 0)) != 0;
    outCommand.dirX = (flags & (1 << 1)) != 0;
    outCommand.dirY = (flags & (1 << 2)) != 0;
    outCommand.dirZ = (flags & (1 << 3)) != 0;
    outCommand.stepX = stepX;
    outCommand.stepY = stepY;
    outCommand.stepZ = stepZ;
    outCommand.flags = flags;
    outCommand.checksum = frame[8];

    return true;
}

bool BinaryProtocol::readFrame(uint8_t* outFrame) {
    while (serial.available() > 0) {
        const uint8_t byteIn = (uint8_t)serial.read();

        if (!syncing) {
            if (byteIn == SYNC_BYTE) {
                rxBuffer[0] = byteIn;
                rxIndex = 1;
                syncing = true;
            }
            continue;
        }

        rxBuffer[rxIndex++] = byteIn;

        if (rxIndex >= FRAME_SIZE) {
            for (size_t i = 0; i < FRAME_SIZE; i++) {
                outFrame[i] = rxBuffer[i];
            }

            rxIndex = 0;
            syncing = false;
            return true;
        }
    }

    return false;
}

bool BinaryProtocol::update(MotionCommand& outCommand) {
    uint8_t frame[FRAME_SIZE];

    if (!readFrame(frame)) {
        return false;
    }

    return decodeFrame(frame, outCommand);
}