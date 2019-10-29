/*
// This #include statement was automatically added by the Particle IDE.
#include <New_Ping.h>

#define TRIGGER_PIN D0
#define ECHO_PIN D1
#define MAX_DISTANCE 500
int Distance;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
*/
SYSTEM_THREAD(ENABLED);

int Duration, Distance_CM, Distance_IN, Min_Distance;

#define TrigPin D1
#define EchoPin D0

int Trigger_Distance = 90;

void setup() {
    Particle.variable("Distance", Distance_CM);
    Particle.variable("Min_Distance", Min_Distance);
    
    pinMode(TrigPin, OUTPUT);
    pinMode(EchoPin, INPUT);
    
    //Arbitrary high value, greater than any the sensor will read (otherwise it will start at 0)
    Min_Distance = 1000;
}

void loop() {
    //Distance = sonar.ping_in();
    
    //Necessary, otherwise your values are all over the place. 
    delay(500);
    
    digitalWrite(TrigPin, LOW);
    delayMicroseconds(4);
    digitalWrite(TrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TrigPin, LOW);
    Duration = pulseIn(EchoPin, HIGH);
    Distance_CM = (Duration/2) / 29.1; //Centimeters
    Distance_IN = Distance_CM * 0.393701; //Inches
    
    if (Distance_CM < Min_Distance) {
        Min_Distance=Distance_CM;
    }
    
    if (Distance_CM <= Trigger_Distance) {
        //Particle.publish("Motion", "", 60, PRIVATE, WITH_ACK);
        Mesh.publish("Motion", "");
        delay (20000); //15 second delay so it doesn't just constantly keep running if constantly triggered
    }
}