volatile int pwm_value_1 = 0;
volatile int prev_time_1 = 0;
volatile int pwm_value_2 = 0;
volatile int prev_time_2 = 0;

const byte PWM_PIN_3 = 4;
const byte YAW_PIN = 11;
const byte PITCH_PIN = 10;
 
volatile int pwm_value_3;

void rising_1();
void rising_2();
void falling_1();
void falling_2();

#include <Servo.h>

Servo Yaw;
Servo Pitch;

void setup() {
    Serial.begin(115200);
    attachInterrupt(0, rising_1, RISING);
    attachInterrupt(1, rising_2, RISING);
    Serial.println("starting up");
    pinMode(PWM_PIN_3, INPUT);

    Yaw.attach(YAW_PIN);
    Pitch.attach(PITCH_PIN);
}
 
void loop() {
    pwm_value_3 = pulseIn(PWM_PIN_3, HIGH);

    int YawOut, PitchOut;

    if(pwm_value_3 > 1500){
        YawOut = map(pwm_value_2,1000,2000,0,180);
        PitchOut = map(pwm_value_1,1000,2000,0,180);
    } else {
        YawOut = map(1500,1000,2000,0,180);
        PitchOut = map(1500,1000,2000,0,180);
    }

    Yaw.write(YawOut);
    Pitch.write(PitchOut);

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

