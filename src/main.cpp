#include <Arduino.h>

#define Triger 4
#define Echo 5
#define reverseRight 12
#define reverseLeft 7
#define SpeedRight 11
#define SpeedLeft 6
#define Switch 2
#define CollisionRight 9
#define CollisionLeft 8

#define DefaultRight 255
#define DefaultLeft 225

int distance;
int mindistance = 25;
int maxdistance = 10000;
int fails = 0;
int predistance = 0;
int prepredistance = 0;
long Premillis = 0;
long Time = 0;
int direction = 0;
int defaultHighRight = DefaultRight;
int defaultHighLeft = DefaultLeft;


void turnOff(int x){
    defaultHighRight = 0;
    defaultHighLeft = 0;
    analogWrite(SpeedRight, defaultHighRight);
    analogWrite(SpeedLeft, defaultHighLeft);
    digitalWrite(reverseLeft, 0);
    digitalWrite(reverseRight, 0);
    while (true){
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(x);
    }
}

float measure(){
    digitalWrite(Triger, LOW);
    delay(5);
    digitalWrite(Triger, HIGH);
    delay(10);
    digitalWrite(Triger, LOW);
    float Cycletime = pulseIn(Echo, HIGH);
    return ((0.0343 * Cycletime) / 2);
}

void goAway(int Mode = 1, int site = random(2)){ // 0:Right 1:Left
    fails++;
    Time = millis();
    switch (site){
    case 0:
        while (true){
            digitalWrite(reverseRight, Mode);
            digitalWrite(reverseLeft, 0);
            analogWrite(SpeedLeft, defaultHighLeft);
            analogWrite(SpeedRight, defaultHighRight * Mode);
            delay(200);
            while (distance < mindistance + 50 || maxdistance < distance){
                distance = int(measure());
                if(millis()-Time > 10000){
                    turnOff(250);
                }
                if (digitalRead(CollisionRight) == 0 || digitalRead(CollisionLeft) == 0){
                    break;
                }
            }
            // Last check
            analogWrite(SpeedLeft, 0);
            analogWrite(SpeedRight, 0);
            delay(300);
            distance = int(measure());
            if (distance > mindistance + 50 || maxdistance > distance || digitalRead(CollisionRight) == 0 || digitalRead(CollisionLeft) == 0){
                break;
            }
        }

        break;

    case 1:
        while (true){
            digitalWrite(reverseRight, 0);
            digitalWrite(reverseLeft, Mode);
            analogWrite(SpeedLeft, defaultHighLeft * Mode);
            analogWrite(SpeedRight, defaultHighRight);
            delay(200);
            while (distance < mindistance + 50 || maxdistance < distance){
                distance = int(measure());
                if(millis()-Time > 10000){
                    turnOff(250);
                }

                if (digitalRead(CollisionRight) == 0 || digitalRead(CollisionLeft) == 0){
                    break;
                }
            }
            // Last check
            analogWrite(SpeedLeft, 0);
            analogWrite(SpeedRight, 0);
            delay(300);
            distance = int(measure());
            if (distance > mindistance + 50 || maxdistance > distance || digitalRead(CollisionRight) == 0 || digitalRead(CollisionLeft) == 0){
                break;
            }
        }

        break;

    default:
        break;
    }
}


void setup(){
    Serial.begin(9600);
    pinMode(Triger, OUTPUT);
    pinMode(Echo, INPUT);
    pinMode(reverseLeft, OUTPUT);
    pinMode(reverseRight, OUTPUT);
    pinMode(SpeedLeft, OUTPUT);
    pinMode(SpeedRight, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(Switch, INPUT);
    pinMode(CollisionLeft, INPUT_PULLUP);
    pinMode(CollisionRight, INPUT_PULLUP);

    digitalWrite(reverseLeft, 0);
    digitalWrite(reverseRight, 0);
    analogWrite(SpeedLeft, 0);
    analogWrite(SpeedRight, 0);
}

void loop(){
    distance = int(measure());

    if (fails > 10){
        turnOff(500);
    }
    if (millis()-Premillis >5000){
        Serial.print(distance);
        Serial.print(" , ");
        Serial.print(predistance);
        Serial.print(" , ");
        Serial.print(prepredistance);
        Serial.print(" , ");
        Serial.println(abs(prepredistance + predistance - distance*2));
        if (abs(prepredistance + predistance - distance*2) < 5){
            turnOff(100);
        }
        prepredistance = predistance;
        predistance = distance;
        Premillis = millis();
        direction = random(2);
    }

    if (digitalRead(CollisionRight) == 0){
        analogWrite(SpeedRight, defaultHighRight);
        analogWrite(SpeedLeft, defaultHighLeft);
        digitalWrite(reverseLeft, 1);
        digitalWrite(reverseRight, 1);
        digitalWrite(13, 0);
        delay(500);
        direction = 1;
        goAway(1, 1);
    }
    else if (digitalRead(CollisionLeft) == 0){
        analogWrite(SpeedRight, defaultHighRight);
        analogWrite(SpeedLeft, defaultHighLeft);
        digitalWrite(reverseLeft, 1);
        digitalWrite(reverseRight, 1);
        digitalWrite(13, 0);
        delay(500);
        direction = 0;
        goAway(1, 0);
    }
    else if (distance < mindistance || maxdistance < distance){
        // analogWrite(SpeedRight, 0);
        // analogWrite(SpeedLeft, 0);
        digitalWrite(13, 0);
        goAway(1, direction);
    }
    else if (distance < mindistance*2){
        switch (direction)
        {
        case 1:
            analogWrite(SpeedRight, defaultHighRight);
            analogWrite(SpeedLeft, int(defaultHighLeft * (float(distance)/50-0.5)));
            digitalWrite(reverseLeft, 0);
            digitalWrite(reverseRight, 0);
            digitalWrite(13, 1);
            fails = 0;
            break;

        case 0:
            analogWrite(SpeedRight, int(defaultHighRight * (float(distance)/50-0.5)));
            analogWrite(SpeedLeft, defaultHighLeft);
            digitalWrite(reverseLeft, 0);
            digitalWrite(reverseRight, 0);
            digitalWrite(13, 1);
            fails = 0;
            break;
        
        default:
            break;
        }
    }

    else{
        analogWrite(SpeedRight, defaultHighRight);
        analogWrite(SpeedLeft, defaultHighLeft);
        digitalWrite(reverseLeft, 0);
        digitalWrite(reverseRight, 0);
        digitalWrite(13, 1);
        fails = 0;
    }
}