#include <VescUart.h>
#include <HardwareTimer.h>
#include <Arduino.h>
#include "Encoder.h"

HardwareTimer *encoderTimer = new HardwareTimer(TIM2);

#define ENC_A PA0
#define ENC_B PA1

#define CPR 2400
#define MAXVAL 32767
#define MAXROT 1

int32_t Encoder::convertTicks(int32_t ticks) {
  float value = ((float)ticks / CPR / MAXROT) * MAXVAL;
  value = constrain(value, -32767.0f, 32767.0f);
  return (int32_t)value;
}

void Encoder::setup() {


  pinMode(ENC_A,INPUT);
  pinMode(ENC_B,INPUT);

  encoderTimer->setMode(1, TIMER_INPUT_CAPTURE_RISING, PA0);
  encoderTimer->setMode(2, TIMER_INPUT_CAPTURE_RISING, PA1);

  TIM2->SMCR &= ~TIM_SMCR_SMS;
  TIM2->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;

  TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
  TIM2->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;

  TIM2->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);

  TIM2->ARR = 0xFFFFFFFF; 
  TIM2->CNT = 0;

  lastSampleUs = micros();

  encoderTimer->resume();

}
void Encoder::loop() {
   uint32_t now = micros();
  uint32_t dtUs = now - lastSampleUs;

  if (dtUs < 1000) {
    return;
  }
  count = (int32_t)TIM2->CNT;
  currentPosition = convertTicks(count);


  positionChange = currentPosition - lastPosition;

  currentVelocity =
    (int32_t)(((int64_t)positionChange * 1000000LL) / dtUs);

  currentAcceleration =
    (int32_t)(((int64_t)(currentVelocity - lastVelocity) * 1000000LL) / dtUs);

  if (abs(currentVelocity) > maxVelocity) {
    maxVelocity = abs(currentVelocity);
  }
  if (maxVelocity < 1) {
    maxVelocity = 1;
  }
  if (abs(currentAcceleration) > currentAcceleration) {
    maxAcceleration = abs(currentAcceleration);
  }
  if (maxAcceleration < 1) {
    maxAcceleration = 1;
  }

  if (abs(positionChange) > maxPositionChange) {
    maxPositionChange = abs(positionChange);
  }
  if (maxPositionChange < 1) {
    maxPositionChange = 1;
  }



  lastPosition = currentPosition;
  lastVelocity = currentVelocity;
  lastSampleUs = now;
}
