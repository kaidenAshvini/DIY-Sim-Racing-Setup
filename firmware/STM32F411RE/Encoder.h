#pragma once
#include <Arduino.h>

class Encoder {
  private:
  int32_t convertTicks(int32_t ticks);
  int32_t lastPosition = 0;
  int32_t lastVelocity = 0;
  uint32_t lastSampleUs = 0;

  public:
  int32_t count;
  void setup();
  void loop();
  
  int32_t currentPosition = 0;
  int32_t maxValue = 32767;

  int32_t currentVelocity = 0;
  int32_t maxVelocity = 1;

  int32_t currentAcceleration = 0;
  int32_t maxAcceleration = 1;

  int32_t positionChange = 0;
  int32_t maxPositionChange = 1;
};