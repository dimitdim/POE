int Strips=48;

int Position=0;
float Velocity=0;
float Acceleration=0;
int count=0;
int debounce=0;
int old_colour=0;
int colour=0;
int last_change=0;

String inputString="";
String cmd="";
int val=0;
boolean stringComplete=false;

int convert(int raw) {
    if(raw<256) {return 0;}
    else if(raw>767) {return 2;}
    else {return 1;}
}

void encoder() {
    colour=convert(analogRead(A0));
    
    if(colour!=old_colour) {debounce++;}
    else {debounce=0;}
    
    if(debounce>2) {
        debounce=0;
        int dif=(colour-old_colour)%3;
        if (dif==1) {count++%Strips;}
        else {count--%Strips;}
        old_colour=colour;
        Position=count*360/Strips;
        Velocity=(360/Strips)/(millis()-last_change);
        last_change=millis();
    }
}

void setup() {
    old_colour=convert(analogRead(A0));
    last_change=millis();

    Serial.begin(9600);
    while (!Serial) {;} //some Arduinos take a while...
    inputString.reserve(128); //allows adding one byte at a time (it needs to be a buffer of sorts)
}

void loop() {
    encoder();
    if(stringComplete) {
        Serial.print("Position: ");
        Serial.print(Position);
        Serial.print("  Velocity: ");
        Serial.print(Velocity);
        Serial.print( "Acceleration: ");
        Serial.println(Acceleration);
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
