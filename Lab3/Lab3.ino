#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

int Strips=36;
int dbnc=3;

int goal=180;
int Position=3; // 3 just so we don't have to run current through arduino every fucking time
float Velocity=0;
float Acceleration=0;
float oldVelocity=0;
int count=0;
int debounce=0;
int old_colour=0;
int colour=0;
int last_change=0;
boolean forward=true;
int diff=0;

String inputString="";
String cmd="";
int val=0;
boolean stringComplete=true;

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);

int convert(int raw) {
    if(raw<300) {return 0;}
    else {return 1;}
}

void encoder() {
    colour=convert(analogRead(A0));
    
    if(colour!=old_colour) {debounce++;}
    else {debounce=0;}
    
    if(debounce>dbnc) {
        debounce=0;
        if(forward) {count=(count+1)%Strips;}
        else {count=(count-1)%Strips;}
        old_colour=colour;
        Position=(count*360/Strips);
        oldVelocity=Velocity;
        Velocity=(360000.0/Strips)/(millis()-last_change);
        Acceleration=(Velocity-oldVelocity)/(millis()-last_change);
        last_change=millis();
    }
}

void PID() {
    diff=(goal-Position)%360;
    if(diff<180){myMotor->run(BACKWARD);}
    else if(diff>180){myMotor->run(FORWARD);}
    else {myMotor->run(RELEASE);}
}

void setup() {
    AFMS.begin(880);
    myMotor->setSpeed(255);
    myMotor->run(FORWARD);
    myMotor->run(RELEASE);
    
    old_colour=convert(analogRead(A0));
    last_change=millis();
    
    Serial.begin(9600);
    while (!Serial) {;} //some Arduinos take a while...
    inputString.reserve(128); //allows adding one byte at a time (it needs to be a buffer of sorts)
}

void loop() {
    encoder();
    PID();
    
    if(stringComplete) {
        cmd=inputString.substring(0,inputString.indexOf('@')); //Split the input into command and value. The only reason this isn't '=' is because I like '@'
        val=inputString.substring(inputString.indexOf('@')+1).toInt();
        
        if (cmd.equals(String("dbnc"))) {
            dbnc=val;
            Serial.println() && Serial.print("Debounce set: ") && Serial.println(dbnc) && Serial.println();
        }
        
        stringComplete=false;
        inputString = "";
        cmd="";
        val=0;
        
        Serial.print("Position: ");
        Serial.print(Position);
        Serial.print(" Velocity: ");
        Serial.print(Velocity);
        Serial.print( " Acceleration: ");
        Serial.println(Acceleration);
        Serial.print("Count: ");
        Serial.print(count);
        Serial.print(" Raw: ");
        Serial.print(analogRead(A0));
        Serial.print(" Converted: ");
        Serial.println(colour);
        Serial.println();
    }
}

void serialEvent() { //This gets run every time after loop
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        if (inChar == '\n') {
            inputString.trim(); //Get rid of the \n
            stringComplete = true; //The loop will run next time!
        }
        else { //Put all the input bytes in a buffer
            inputString += inChar;
        }
    }
}
