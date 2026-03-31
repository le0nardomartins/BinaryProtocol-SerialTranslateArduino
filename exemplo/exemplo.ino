#include "BinaryProtocol.h"

BinaryProtocol protocol(Serial);

const int X_STEP_PIN = 2;
const int X_DIR_PIN  = 5;

const int Y_STEP_PIN = 3;
const int Y_DIR_PIN  = 6;

const int Z_STEP_PIN = 4;
const int Z_DIR_PIN  = 7;

void pulseStepPin(int stepPin) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(300);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(300);
}

void moveAxisSteps(int dirPin, int stepPin, bool dir, uint16_t steps) {
    digitalWrite(dirPin, dir ? HIGH : LOW);

    for (uint16_t i = 0; i < steps; i++) {
        pulseStepPin(stepPin);
    }
}

void executeMotion(const BinaryProtocol::MotionCommand& cmd) {
    if (!cmd.valid) {
        return;
    }

    // Exemplo simples:
    // executa cada eixo em sequência.
    // Depois você pode trocar por interpolação simultânea estilo Bresenham.

    if (cmd.stepX > 0) {
        moveAxisSteps(X_DIR_PIN, X_STEP_PIN, cmd.dirX, cmd.stepX);
    }

    if (cmd.stepY > 0) {
        moveAxisSteps(Y_DIR_PIN, Y_STEP_PIN, cmd.dirY, cmd.stepY);
    }

    if (cmd.stepZ > 0) {
        moveAxisSteps(Z_DIR_PIN, Z_STEP_PIN, cmd.dirZ, cmd.stepZ);
    }
}

void printCommand(const BinaryProtocol::MotionCommand& cmd) {
    Serial.print("VALID=");
    Serial.print(cmd.valid ? "1" : "0");

    Serial.print(" TYPE=");
    Serial.print(cmd.isG1 ? "G1" : "G0");

    Serial.print(" DIRX=");
    Serial.print(cmd.dirX ? "+" : "-");

    Serial.print(" DIRY=");
    Serial.print(cmd.dirY ? "+" : "-");

    Serial.print(" DIRZ=");
    Serial.print(cmd.dirZ ? "+" : "-");

    Serial.print(" STEPX=");
    Serial.print(cmd.stepX);

    Serial.print(" STEPY=");
    Serial.print(cmd.stepY);

    Serial.print(" STEPZ=");
    Serial.print(cmd.stepZ);

    Serial.print(" FLAGS=");
    Serial.print(cmd.flags, BIN);

    Serial.print(" CHK=0x");
    Serial.println(cmd.checksum, HEX);
}

void setup() {
    pinMode(X_STEP_PIN, OUTPUT);
    pinMode(X_DIR_PIN, OUTPUT);

    pinMode(Y_STEP_PIN, OUTPUT);
    pinMode(Y_DIR_PIN, OUTPUT);

    pinMode(Z_STEP_PIN, OUTPUT);
    pinMode(Z_DIR_PIN, OUTPUT);

    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    digitalWrite(Z_STEP_PIN, LOW);

    protocol.begin(115200);

    Serial.println("READY");
}

void loop() {
    BinaryProtocol::MotionCommand cmd;

    if (protocol.update(cmd)) {
        printCommand(cmd);
        executeMotion(cmd);
        Serial.println("OK");
    }
}
