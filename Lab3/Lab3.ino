#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

/*PID*/
float goal=90; //Desired final position

float kp=1.5; //PID Settings
float ki=.001;
float kd=.2;

/*Measured Data and Settings*/
int Strips=36; //Number of Strips
int cutoff=300; //Analog cutoff value (any value always between white and black)
int dbnc=5; //Number of debounce retries
int deltatime=5; //Frequency of Integral/Derivative calculation (ms)

/*Current Status*/
int colour=0; //Colour of encoder stripe under sensor
int count=0; //Position in stripes
float Position=0; //Position in degrees
boolean forward=true; //Direction

float diff=0; //Signed distance from goal (P)
float sum=0; //Running sum of distances (I)
float ddiff=0; //Derivative of change in distance (D)
float spd=0; //Calculated speed

/*Temporary variables*/
float spda=0; //Speed before flooring (for debugging purposes)
int olddiff=0; //Previous distance for calculation of derivative
long lastm=0; //Previous calculation time for timing of next calculation
int debounce=0; //Debounce counter
int old_colour=0; //Previous colour for detecting change (more useful if grey is used)

/*Serial*/
String inputString=""; //Initiate Serial input command variables
String cmd="";
float val=0;
char valtemp[33]; //Reserve space for intermediate step in converting String to float
boolean stringComplete=false; //Don't run debuging info until a command is entered

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);

int convert(int raw) {
    if(raw<cutoff) {return 0;} //If the color is light, consider it white
    else {return 1;}
}

void encoder() {
    colour=convert(analogRead(A0)); //Measure color
    
    if(colour!=old_colour) {debounce++;} //If it's different, don't change right away
    else {debounce=0;}
    
    if(debounce>dbnc) { //If it's different for a few measurements, it's different for real
        debounce=0;
        if(forward) {count=(count+1)%Strips;} //Increment the step, looping over
        else {count=(Strips+count-1)%Strips;} //C can't do mods of negatives
        old_colour=colour;
        Position=(count*360/Strips); //Update current Position
 //       Serial.print(Position)  && Serial.print(" : ") && Serial.print(diff) && Serial.print(" : ") && Serial.println(spda);
    }
}

void PID() {
    if(abs(Position-goal)<180){diff=goal-Position;} //If the distance is closer without passing through 0, the difference is positive if Position is before goal
    else{diff=Position-goal;} //And vise-versa
    
    if (millis()+lastm>deltatime) { //Only take integral and derivative data every 5ms
        lastm=millis();
        sum+=diff;
        olddiff=diff;
        ddiff=(diff-olddiff)/deltatime;
    }

    spda=(diff*kp+ki*sum+kd*ddiff)*(255/180); //The PID Equation!!
    spd=max(-255,min(255,spda)); //Just in case speed is more than the maximum allowed
    if(spd>0){ //Run the motor at the calculated speed
        myMotor->setSpeed(spd);
        myMotor->run(FORWARD);
        forward=true;
    }
    else if(spd<0){ //Since the motor can't run at a negative spped, we need to run its opposite backwards
        myMotor->setSpeed(-spd);
        myMotor->run(BACKWARD);
        forward=false;
    }
    else {myMotor->run(RELEASE);} //If no more change is necessary, turn off the motor
}

void communicate() { //Serial control!!
        cmd=inputString.substring(0,inputString.indexOf('@')); //Split the input into command and value. The only reason this isn't '=' is because I like '@'
        inputString.substring(inputString.indexOf('@')+1).toCharArray(valtemp, sizeof(valtemp)); //Jankety conversion to floats
        val=atof(valtemp);
        
        if (cmd.equals(String("dbnc"))) {
            dbnc=val;
            Serial.println() && Serial.print("Debounce set: ") && Serial.println(dbnc) && Serial.println();
        }
        if (cmd.equals(String("kp"))) {
            kp=val;
            Serial.println() && Serial.print("Kp Set: ") && Serial.println(kp) && Serial.println();
        }
        if (cmd.equals(String("ki"))) {
            ki=val;
            Serial.println() && Serial.print("Ki Set: ") && Serial.println(ki) && Serial.println();
        }
        if (cmd.equals(String("kd"))) {
            kd=val;
            Serial.println() && Serial.print("Kd Set: ") && Serial.println(kd) && Serial.println();
        }
        if (cmd.equals(String("gains"))) {
            Serial.println();
            Serial.print(" Kd: ");
            Serial.print(kd);
            Serial.print(" Ki: ");
            Serial.print(ki);
            Serial.print(" Kd: ");
            Serial.println(kd);
            Serial.println();
        }
        
        else { //If it's not an explicit command, throw some debug info
            Serial.println();
            Serial.print("Position: ");
            Serial.print(Position);
            Serial.print(" Count: ");
            Serial.print(count);
            Serial.print(" Raw: ");
            Serial.print(analogRead(A0));
            Serial.print(" Converted: ");
            Serial.println(colour);
            Serial.print("Difference: ");
            Serial.print(diff);
            Serial.print(" Sum: ");
            Serial.print(sum);
            Serial.print(" Derivative: ");
            Serial.print(ddiff);
            Serial.print(" Speed: ");
            Serial.println(spda);
            Serial.println();
        }
        
        stringComplete=false;
        inputString = "";
}

void setup() {
    AFMS.begin(880); //880Hz sounds nicer than 1kHz
    myMotor->setSpeed(255); //Turn on motor
    myMotor->run(FORWARD);
    myMotor->run(RELEASE);
    
    old_colour=convert(analogRead(A0)); //The first value shouldn't be a "change"
    
    Serial.begin(9600);
    while (!Serial) {;} //some Arduinos take a while...
    inputString.reserve(128); //allows adding one byte at a time (it needs to be a buffer of sorts)
}

void loop() {
    encoder();
    PID();
    if(stringComplete) {communicate();}
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
