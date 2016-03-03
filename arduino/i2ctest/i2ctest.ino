/*
#include <Wire.h>


void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(115200);           // start serial for output
  Serial.println("serial is working");
}

void loop() {
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}
*/

#include <Wire.h>

#define SLAVE_ADDRESS 0x04
int number = 0;
int state = 0;

void setup(){
    
    Serial.begin(115200);
    Serial.println("Started Program");
    
    Wire.begin(SLAVE_ADDRESS);

    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);

    Serial.println("Callbacks Initiated");

}

void loop() {
    delay(100);
}

void receiveData(int byteCount)
{
    while(Wire.available()){
        number = Wire.read();

        Serial.println("Data Received");
        Serial.println(number);

    }
}

void sendData(){
    Wire.write(9);
}
