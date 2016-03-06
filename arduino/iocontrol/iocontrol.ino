volatile int pwm_value_1 = 0;
volatile int prev_time_1 = 0;
volatile int pwm_value_2 = 0;
volatile int prev_time_2 = 0;

const byte PWM_PIN_3 = 4;
const byte YAW_PIN = 10;
const byte PITCH_PIN = 11;
 
volatile int pwm_value_3;

void rising_1();
void rising_2();
void falling_1();
void falling_2();

void piSerialWrite();

#include <Servo.h>

Servo Yaw;
Servo Pitch;
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change :
const long interval = 100;    

int YawOut, PitchOut;

void setup() {
    Serial.begin(57600);

    attachInterrupt(0, rising_1, RISING);
    attachInterrupt(1, rising_2, RISING);
    Serial.println("starting up");
    pinMode(PWM_PIN_3, INPUT);

    Yaw.attach(YAW_PIN);
    Pitch.attach(PITCH_PIN);
}
 
void loop() {
    pwm_value_3 = pulseIn(PWM_PIN_3, HIGH);


    piSerialRead();
    if(pwm_value_3 < 1500){
        YawOut = map(1500,1000,2000,0,180);
        PitchOut = map(1500,1000,2000,0,180);
    }

    piSerialRead();

    Yaw.write(YawOut);
    Pitch.write(PitchOut);
/*
    Serial.print("Pitch: ");
    Serial.print(pwm_value_1);
    Serial.print(" Yaw: ");
    Serial.print(pwm_value_2);
    Serial.print(" Enable: ");
    Serial.print(pwm_value_3);
    Serial.print(" PitchOut: ");
    Serial.print(PitchOut);
    Serial.print(" YawOut: ");
    Serial.print(YawOut);
    Serial.print("\n\r");
    */
    if (currentMillis - previousMillis >= interval) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;
        piSerialWrite();
    }

  }

void piSerialWrite()
{
    Serial.write("<");
    Serial.write("1:");
    Serial.print(pwm_value_1);
    Serial.println(">");
    Serial.write("<");
    Serial.write("2:");
    Serial.print(pwm_value_2);
    Serial.println(">");
    Serial.write("<");
    Serial.write("3:");
    Serial.print(pwm_value_3);
    Serial.println(">");
}

void piSerialRead()
{
    char buffer[10];
    char c;
    bool endfound = false;

    while(Serial.available() > 0 && !endfound) {
        c = Serial.read();
        if( c == '<'){
            for(int n = 0; n < 7; n++){
                c = Serial.read();
                buffer[n] = c;
            }
            endfound = true;
        }
    }

    if(buffer[6] == '>'){ //then we got a complete command
        if(buffer[0] == '1'){
            char numberext[5] = {buffer[2], buffer[3], buffer[4], buffer[5], '\0'};
            int i;
            PitchOut = map(atoi(numberext),1000,2000,0,180);
        }
        else if(buffer[0] == '2'){
            char numberext[5] = {buffer[2], buffer[3], buffer[4], buffer[5], '\0'};
            int i;
            YawOut = map(atoi(numberext),1000,2000,0,180);
        }
    }
}
        
 
void rising_1() {
    attachInterrupt(0, falling_1, FALLING);
    prev_time_1 = micros();
}

void rising_2() {
    attachInterrupt(1, falling_2, FALLING);
    prev_time_2 = micros();
}
 
void falling_1() {
    attachInterrupt(0, rising_1, RISING);
    pwm_value_1 = micros()-prev_time_1;
}

void falling_2() {
    attachInterrupt(1, rising_2, RISING);
    pwm_value_2 = micros()-prev_time_2;
}

