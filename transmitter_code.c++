// this code is written in arduino IDE
// download essential modules, e.g, RF24 module, and add files to sketch
// added files become accessible to be imported into sketch
// import the essential files as below
#include <SPI.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

const uint64_t pipeOut = 0xE8E8F0F0E1LL;
// this same address should be used in the receiver code, otherwise 
// connection will not be established

RF24 radio(9, 10); 
// CE_7 and CS_53 for arduino mega 
// CE_9 and CS_10 for arduino nano, as is in our case

struct MyData{

byte throttle; // separate potentiometer
byte yaw;      // joystick_A potentiometer1 
byte pitch;    // joystick_A potentiometer2
byte roll;     // joystick_B potentiometer1
byte hlds;     // joystick_B potentiometer2
byte AUX1;     // toggle switch left
//byte AUX2;     // toggle switch right

};

MyData data; // we create our data structure format
int select_pin = 4;
void resetData(){
// we set the initial values
data.throttle = 0;  // throttle set to 0 to stop the motor
data.yaw = 127;     // 127 is the middle value of the IO ADC
data.pitch = 127; 
data.roll = 127;
data.hlds = 127;
data.AUX1 = 0;
//data.AUX2 = 0;

}

void setup() {
// this code runs once:
pinMode(select_pin, INPUT);

// we configure our radio channel
radio.begin(); 
radio.setAutoAck(true);// may be left out
radio.setDataRate(RF24_250KBPS);//may be left out
radio.openWritingPipe(pipeOut);
radio.setPALevel(RF24_PA_MAX);
radio.setChannel(108);
radio.stopListening();
resetData();

}

void loop() {
// we read our values from toggle switch, potentiometer, 
// and joystick, convert them to digital values and assign them 
// to our data structure, MyData
data.AUX1 = digitalRead(4);
data.throttle = map(analogRead(A2), 0, 1024, 0, 255);
data.yaw = map(analogRead(A7), 0, 1024, 0, 255);
data.pitch = map(analogRead(A6), 0, 1024, 0, 255);
data.roll = map(analogRead(A4), 0, 1024, 0, 255);
data.hlds = map(analogRead(A3), 0, 1024, 0, 255); 

// write what is in the data structure
// onto our radio transmision channel
radio.write(&data, sizeof(MyData));

}