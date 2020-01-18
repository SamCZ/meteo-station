#pragma once

#include <SoftwareSerial.h>

class SigFox {
private:
    SoftwareSerial* _SoftSerial;
    int8_t m_rxPin = -1;
    int8_t m_txPin = -1;
public:
    SigFox(int8_t rxPin, int8_t txPin = -1);
    SigFox();
    ~SigFox();

    void begin();

    String getID();
    String getPAC();
    String sendMessage(uint8_t sigfoxMsg[], int bufferSize);
};