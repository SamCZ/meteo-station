#include "MySigfox.h"

SigFox::SigFox(int8_t rxPin, int8_t txPin = -1) : m_rxPin(rxPin), m_txPin(txPin) {
    _SoftSerial = new SoftwareSerial(rxPin, txPin);
}

SigFox::SigFox() : SigFox(13, 15) {

}

SigFox::~SigFox() {
    delete _SoftSerial;
}

void SigFox::begin() {
    pinMode(m_rxPin, INPUT);
    pinMode(m_txPin, OUTPUT);
    _SoftSerial->begin(9600);
}

String SigFox::getID () {
  String deviceId = "";
  char sigfoxBuffer;

  // Send AT$I=10 to WISOL to GET ID number
  _SoftSerial->print("AT$I=10\r");

  while (!_SoftSerial->available()){
     delay(10);
  }
  
  while(_SoftSerial->available()){
    sigfoxBuffer = _SoftSerial->read();
    deviceId += sigfoxBuffer;
    delay(10);
  }
  return deviceId;
}

String SigFox::getPAC (){
  String pacNumber = "";
  char sigfoxBuffer;

  // Send AT$I=11 to WISOL to GET PAC number
  _SoftSerial->print("AT$I=11\r");
  while (!_SoftSerial->available()){
     delay(10);
  }
  while(_SoftSerial->available()){
    sigfoxBuffer = _SoftSerial->read();
    pacNumber += sigfoxBuffer;
    delay(10);
  }
  return pacNumber;
}

String SigFox::sendMessage(uint8_t sigfoxMsg[], int bufferSize) {
  String status = "";
  char sigfoxBuffer;

  // Send AT$SF=xx to WISOL to send XX (payload data of size 1 to 12 bytes)
  _SoftSerial->print("AT$SF=");
  for(int i= 0;i<bufferSize;i++){
    if (sigfoxMsg[i]<0x10) {
      _SoftSerial->print("0");
    }
    _SoftSerial->print(String(sigfoxMsg[i], HEX));
  }

  _SoftSerial->print("\r");

  while (!_SoftSerial->available()){
     delay(10);
  }

  while(_SoftSerial->available()){
    sigfoxBuffer = (char)_SoftSerial->read();
    status += sigfoxBuffer;
    delay(10);
  }

  return status;
}