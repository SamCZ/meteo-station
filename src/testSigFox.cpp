#if 0
#include <SoftwareSerial.h>


#define RxNodePin 13
#define TxNodePin 15

// Setup UART Communication with 
SoftwareSerial Sigfox(RxNodePin, TxNodePin);

// 12 bytes message buffer
uint8_t sigfoxMsg[12];

String getID ();
String getPAC ();
String sendMessage(uint8_t sigfoxMsg[], int bufferSize);

void setup () {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n***** START *****");


  pinMode(RxNodePin, INPUT);
  pinMode(TxNodePin, OUTPUT);
  Sigfox.begin(9600);
  delay(100);

  Serial.print("Device ID: " + getID()); 
  Serial.print("Device PAC Number: " + getPAC());
}

void loop () {
  sigfoxMsg[0]=0x01;
  sigfoxMsg[1]=0x23;
  sigfoxMsg[2]=0x45;
  sigfoxMsg[3]=0x67;
  sigfoxMsg[4]=0x89;
  Serial.println("Send message: " + sendMessage(sigfoxMsg, 5));

  // Send every 10 minutes
  delay(60000);
}

// Get device ID
String getID () {
  String deviceId = "";
  char sigfoxBuffer;

  // Send AT$I=10 to WISOL to GET ID number
  Sigfox.print("AT$I=10\r");

  while (!Sigfox.available()){
     delay(10);
  }
  
  while(Sigfox.available()){
    sigfoxBuffer = Sigfox.read();
    deviceId += sigfoxBuffer;
    delay(10);
  }
  return deviceId;
}


// Get PAC number
String getPAC (){
  String pacNumber = "";
  char sigfoxBuffer;

  // Send AT$I=11 to WISOL to GET PAC number
  Sigfox.print("AT$I=11\r");
  while (!Sigfox.available()){
     delay(10);
  }
  while(Sigfox.available()){
    sigfoxBuffer = Sigfox.read();
    pacNumber += sigfoxBuffer;
    delay(10);
  }
  return pacNumber;
}

String sendMessage(uint8_t sigfoxMsg[], int bufferSize) {
  String status = "";
  char sigfoxBuffer;

  // Send AT$SF=xx to WISOL to send XX (payload data of size 1 to 12 bytes)
  Sigfox.print("AT$SF=");
  for(int i= 0;i<bufferSize;i++){
    if (sigfoxMsg[i]<0x10) {
      Sigfox.print("0");
    }
    Sigfox.print(String(sigfoxMsg[i], HEX));
  }

  Sigfox.print("\r");

  while (!Sigfox.available()){
     delay(10);
  }

  while(Sigfox.available()){
    sigfoxBuffer = (char)Sigfox.read();
    status += sigfoxBuffer;
    delay(10);
  }

  return status;
}
#endif