// download the RF24 module, add its files to arduino IDE
// then import the file into this sketch
// this code is written in arduino IDE
// this code was successfully uploaded onto 
// arduino nano board, and it worked well

#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <Servo.h>
#include <SPI.h>

const uint64_t pipe = 0xE8E8F0F0E1LL; // create the remote address
RF24 radio(7, 8); // CE 7, CSN 8, create the radio object


// define the structure of data
struct MyData{
  
byte throttle;
byte yaw;
byte pitch;
byte roll;
byte hlds;
byte AUX1;
//byte AUX2;

};
MyData data;

// define servo variables essential for holding 
// PWM signals
Servo throttle_bldc;
Servo yaw_servo;
Servo pitch_servo;
Servo left_wing_servo;
Servo right_wing_servo;
//Servo alarm_buzzer;
//Servo disco_lights;

// initialize variables whose values will be used to create
// PWM signals
int throttle_Value = 0;
int yaw_Value = 0;
int pitch_Value = 0;
int roll_left_Value = 0;
int roll_right_Value = 0;
int hlds_Value = 0;
//int AUX1_Value = 0;
//int AUX2_Value = 0;

// these will be used in implementing a filter
float EMA_a = 0.6;
int EMA_throttle_value = 0;
int EMA_yaw_value = 0;
int EMA_pitch_value = 0;
int EMA_roll_left_value = 0;
int EMA_roll_right_value = 0;
int EMA_hlds_value = 0;
int select_value = 0;

// create a reset fuction that resets data whenever the 
// signal is lost for one second
void resetData(){
// reset values
data.throttle = 0;
data.yaw = 127;
data.pitch = 127;
data.roll = 127;
data.hlds = 127;
data.AUX1 = 0;
//data.AUX2 = 0;

}

void setup() {
// this setup code, runs once:

// we set the control pins 
throttle_bldc.attach(3);
yaw_servo.attach(9);
pitch_servo.attach(6);
left_wing_servo.attach(5);
right_wing_servo.attach(10);
//alarm_buzzer.attach(7);
//disco_lights.attach(8);

// here we implement our filter to clean out any noise from 
// the received signal
EMA_throttle_value = map(data.throttle, 0, 255, 1000, 2000);
EMA_yaw_value = map(data.yaw, 0, 255, 1000, 2000);
EMA_pitch_value = map(data.pitch, 0, 255, 1000, 2000);
EMA_roll_left_value = map(data.roll, 0, 255, 1000, 2000);
EMA_roll_right_value = map(data.roll, 255, 0, 1000, 2000);
EMA_hlds_value= map(data.hlds, 0, 255, 1000, 2000);
resetData();

// we configure our receiver, in relation 
// to our transmitter
radio.begin();
radio.setAutoAck(true);
radio.setDataRate(RF24_250KBPS);
radio.openReadingPipe(1, pipe);
radio.setChannel(108 );
radio.startListening();

}

// we create a function that will effect receiption of data every
// particular time instance
unsigned long lastRecvTime = 0;

void recvData(){

while(radio.available()){

  radio.read(&data, sizeof(MyData));
  lastRecvTime = millis();
  
}

}

void loop() {
// this code runs repeatedly:

recvData(); // we call the receive fucntion
unsigned long now = millis(); // we track timing of received signal

// then we check if we have lost signal, if so, we reset the
//values
if(now - lastRecvTime > 1000){
resetData();
}

select_value = data.AUX1;
throttle_Value = map(data.throttle, 0, 255, 1000, 2000);
yaw_Value = map(data.yaw, 0, 255, 1000, 2000);
pitch_Value = map(data.pitch, 0, 255, 1000, 2000);
roll_left_Value = map(data.roll, 0, 255, 1000, 2000);
roll_right_Value = map(data.roll, 255, 0, 1000, 2000);
hlds_Value = map(data.hlds, 0, 255, 1000, 2000);
//AUX1_Value = map(data.AUX1, 0, 1, 0, 1000);
//AUX2_Value = map(data.AUX2, 0, 1, 0, 1000);

//using received values, we calculate the EMA filter values
EMA_throttle_value = (EMA_a*throttle_Value)+((1-EMA_a)*EMA_throttle_value);
EMA_yaw_value = (EMA_a*yaw_Value)+((1-EMA_a)*EMA_yaw_value);
EMA_pitch_value = (EMA_a*pitch_Value)+((1-EMA_a)*EMA_pitch_value);
EMA_roll_left_value = (EMA_a*roll_left_Value)+((1-EMA_a)*EMA_roll_left_value);
EMA_roll_right_value = (EMA_a*roll_right_Value)+((1-EMA_a)*EMA_roll_right_value);
EMA_hlds_value = (EMA_a*hlds_Value)+((1-EMA_a)*EMA_hlds_value);

// then we create the PWM signals and send them to 
// the appropriate control pins
throttle_bldc.write(EMA_throttle_value);
yaw_servo.write(EMA_yaw_value);
pitch_servo.write(EMA_pitch_value);

if(select_value == 0){
 left_wing_servo.write(EMA_roll_left_value);
 right_wing_servo.write(EMA_roll_right_value); 
  }
if(select_value == 1){
 left_wing_servo.write(EMA_hlds_value);
 right_wing_servo.write(EMA_hlds_value);  
  } 

//digitalWrite(7, data.AUX1);
//digitalWrite(8, data.AUX2);

}