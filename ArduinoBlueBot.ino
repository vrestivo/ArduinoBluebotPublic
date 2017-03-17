#include <SoftwareSerial.h>

#define SOFT_RX 10 // to bluetoots unit's TX
#define SOFT_TX 11 // to bluetoots unit's RX

//define motor controll pins
#define PIN_FWD_A 8
#define PIN_REV_A 9
#define PIN_FWD_B 6
#define PIN_REV_B 7
#define PIN_PWM_B 5
#define PIN_PWM_A 3


//speed definition 0-255 value
#define SPD_FULL 254
#define SPD_TURN 70 //run inside wheels slower for turning


#define BAUD 115200
//#define BAUD 9600

#define W_DELAY 25  //delay for reading from serial
#define R_DELAY 50  //delay for writing to serial
#define DELAY 25
#define SS_DELAY 25
#define MOVE_DELAY 40

//define constants for actions
#define M_FWD 'W' //move forward
#define M_REV 'S' //move in reverse
#define M_LFT 'A' //turn left
#define M_RGT 'D' //turn right



//inBuffer for Seria.available() only holds 64 bytes
char inBuffer[64] = {0}; 
char command[64] = {0};
char setup_msg[12] = {0};
int bytes_ready = 0;

SoftwareSerial mySerial(SOFT_RX,SOFT_TX);



void setup() {
  // put your setup code here, to run once:

  //set up hardware serial connection
  //this is used for debugging
  Serial.begin(BAUD);
  while(!Serial){
    clear_inBuffers();
  }


  //configure pins 
  pinMode(PIN_FWD_A, OUTPUT);
  pinMode(PIN_FWD_B, OUTPUT);
  pinMode(PIN_REV_A, OUTPUT);
  pinMode(PIN_REV_B, OUTPUT);

  pinMode(PIN_PWM_A, OUTPUT);
  pinMode(PIN_PWM_B, OUTPUT);

  //set up software serial to read from bluetooth module
  mySerial.begin(BAUD);
  delay(2000);

//the segment below used for diagnostics of
//the bluetooth module.
//
//it is used when the module is not connected
//to anothere bluetooth device.
//The code sends an "AT" command and receives "OK"
//response if the module is functioning properly

/*
  //*****DIAGNOSTICS SEGMENT****
  
  //mySerial.write(":");
  mySerial.println("AT");
  //delay(DELAY);

  int i = 0;

  while(mySerial.available()){
    //delay(DELAY);
    setup_msg[i] = mySerial.read();
    i++;
    //if(i==10) break;
  }
  
  Serial.println(setup_msg);
  
   //*****DIAGNOSTICS SEGMENT****

*/


}

void loop() { // run over and over

  //Read bluetooth data
  if (mySerial.available()>0) {
    //wait for data to buffer
    //without delay data is fragmented
    //delay(SS_DELAY);
    bytes_ready = read_soft_ser();
    
    
    Serial.write(inBuffer);
    Serial.println(bytes_ready);
    Serial.print(" : ");
    Serial.println(inBuffer);

    manage_message(inBuffer);

    //print buffer contents, uncomment for debugging
    //printBuffer();

  }

  //Read regular serial data  
  if (Serial.available()) {
    //wait for data to buffer
    //without delay data is fragmented
    delay(R_DELAY);
    
    bytes_ready = read_ser();

    //debugging statements
    Serial.print(bytes_ready);
    Serial.print(" : ");
    Serial.println(inBuffer);
    Serial.println("sending data...");
    
    //print buffer contents, uncomment for debugging
    printBuffer();

    /* a hacky way to fix data fragmentation where
       the first character gets send ahead of the
       rest of the buffer

       fix: send a single character through,
       then the rest of the buffer
    */
    mySerial.write(":");
    mySerial.write(inBuffer);


    //print buffer contents, uncomment for debugging
    //printBuffer();
    
    memset(inBuffer, 0, sizeof(inBuffer));
    bytes_ready=0;
    //delay(50);

  }
}


//read data from serial console
int read_ser(){
  //zero out the inBuffer before reading into it
  memset(inBuffer, 0, sizeof(inBuffer));
  int i = 0;
  int bytes = 0;
  
  while(Serial.available()){
  bytes = Serial.available();
  Serial.print ("bytes available: ");
  Serial.println(bytes);  
  if(i < 64){
      inBuffer[i] = Serial.read();
      i++;
    }
    else{
      break;
    }
  }

  return i;
  
}

//read data from bluetooth
int read_soft_ser(){
  //zero out the inBuffer before reading into it
  memset(inBuffer, 0, sizeof(inBuffer));
  int i = 0;
  
  while(mySerial.available()){
    if(i < 64){
      inBuffer[i] = mySerial.read();
      i++;
      }
      else{
        break;
      }
    }

  return i;

  }
  

void clear_inBuffers(){
  memset(inBuffer, 0, sizeof(inBuffer));
  memset(command, 0, sizeof(command));
  memset(setup_msg, 0, sizeof(setup_msg));
}


int printBuffer(){
  int buffSize = sizeof(inBuffer);
  int i=0;
  for(i=0; i<buffSize; i++){
    Serial.print(i);
    Serial.print(" ");
    Serial.print((int) inBuffer[i]);
    Serial.print(":");
    Serial.println (inBuffer[i]);
  }

  return i;
  
}


int manage_message(char buff[]){

   int i =0 ;
   int len = strlen(buff);
   Serial.print("buffer length:");
   Serial.println(len);
  
  for(i=0; i<len; i++){
    switch(buff[i]){
      case M_FWD:
        move_forward();
        break;
      case M_REV:
        move_back();
        break;
      case M_LFT:
        turn_left();
        break;
      case M_RGT:
        turn_right();
        break;
      case 0:
        Serial.write("encountered null character at pos:%d \n", i);
        return i;
      default:
        break;
      }
  }
  return i;
  
}

void move_forward(){
  Serial.write("driving forward\n");
  digitalWrite(PIN_FWD_A, HIGH);
  digitalWrite(PIN_FWD_B, HIGH);
  analogWrite(PIN_PWM_A, SPD_FULL);
  analogWrite(PIN_PWM_B, SPD_FULL);

  
  delay(MOVE_DELAY);

  analogWrite(PIN_PWM_A, 0);
  analogWrite(PIN_PWM_B, 0);
  digitalWrite(PIN_FWD_A, LOW);
  digitalWrite(PIN_FWD_B, LOW);

}

void move_back(){
  //delete logging
  Serial.write("driving back\n");
  digitalWrite(PIN_REV_A, HIGH);
  digitalWrite(PIN_REV_B, HIGH);
  analogWrite(PIN_PWM_A, SPD_FULL);
  analogWrite(PIN_PWM_B, SPD_FULL);
  
  delay(MOVE_DELAY);

  analogWrite(PIN_PWM_A, 0);
  analogWrite(PIN_PWM_B, 0);  
  digitalWrite(PIN_REV_A, LOW);
  digitalWrite(PIN_REV_B, LOW);

}

void turn_right(){
  //delete logging
  Serial.write("turning left\n");
  digitalWrite(PIN_FWD_A, HIGH);
  digitalWrite(PIN_FWD_B, HIGH);
  analogWrite(PIN_PWM_A, SPD_FULL);
  analogWrite(PIN_PWM_B, SPD_TURN);

  
  delay(MOVE_DELAY);

  analogWrite(PIN_PWM_A, 0);
  analogWrite(PIN_PWM_B, 0);
  digitalWrite(PIN_FWD_A, LOW);
  digitalWrite(PIN_FWD_B, LOW);
}

void turn_left(){
  //delete logging
  Serial.write("turning right\n");
  digitalWrite(PIN_FWD_A, HIGH);
  digitalWrite(PIN_FWD_B, HIGH);
  analogWrite(PIN_PWM_A, SPD_TURN);
  analogWrite(PIN_PWM_B, SPD_FULL);

  
  delay(MOVE_DELAY);

  analogWrite(PIN_PWM_A, 0);
  analogWrite(PIN_PWM_B, 0);
  digitalWrite(PIN_FWD_A, LOW);
  digitalWrite(PIN_FWD_B, LOW);
}


