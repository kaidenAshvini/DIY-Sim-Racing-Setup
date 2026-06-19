#ifndef WHID_h
#define WHID_h

#include <stdint.h>
#include <Arduino.h>
#include <FfbReportHandler.h>
#include <Adafruit_TinyUSB.h>
#include "FfbWheelDescriptor.h"

typedef struct __attribute__((packed)) {
  uint8_t buttons;

  int16_t x;
  int16_t y;
  int16_t rx;
  int16_t ry;
  int16_t z;
  int16_t rz;
} WheelReport;

class HID_ {
public:
  void setup();
  int SendReport(uint8_t id, const void* data, int len);
  FfbReportHandler ffbReportHandler;
};

HID_& HID();

uint16_t get_report_callback(
  uint8_t report_id,
  hid_report_type_t report_type,
  uint8_t* buffer,
  uint16_t reqlen
);

void set_report_callback(
  uint8_t report_id,
  hid_report_type_t report_type,
  uint8_t const* buffer,
  uint16_t bufsize
);

extern FfbReportHandler ffbReportHandler;

extern volatile uint32_t ffbOutputCount;
extern volatile uint8_t lastFfbReportId;
extern volatile uint16_t lastFfbSize;

extern volatile uint8_t lastFfbByte0;
extern volatile uint8_t lastFfbByte1;
extern volatile uint8_t lastFfbType;

extern volatile uint32_t createEffectCount;
extern volatile uint8_t createReportId;
extern volatile uint16_t createSize;

extern volatile uint32_t ffbOutputCount;
extern volatile uint8_t lastFfbBytes[8];
extern volatile uint8_t lastFfbCapturedLen;
extern volatile uint32_t pidStatusGetCount;

#endif