#include <ESP32Encoder.h>
#define ENC_A 18
#define ENC_B 19
#define LED 2

ESP32Encoder encoder;

void setup() {
  Serial.begin(9600);
  pinMode(ENC_A,INPUT_PULLUP);
  pinMode(ENC_B,INPUT_PULLUP);

  encoder.attachHalfQuad(ENC_A,ENC_B);
  encoder.clearCount();

  pinMode(LED,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(50);
  digitalWrite(LED,HIGH);
  delay(50);
  digitalWrite(LED,LOW);

  Serial.println((int32_t)encoder.getCount());
  delay(10);

}
