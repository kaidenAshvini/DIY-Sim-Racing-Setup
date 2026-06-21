#include <VescUart.h>
#include <HardwareTimer.h>
#include <Arduino.h>
#include "Encoder.h"
#include "FfbEngine.h"
#include "FfbReportHandler.h"
#include "WheelConfig.h"
#include "WHID.h"


VescUart vUart;
HardwareSerial vSerial(PA10,PA9);
HardwareSerial DebugSerial(PA3, PA2); 

int32_t totalForce;

Encoder enc;
FfbEngine ffbEngine;
WheelConfig wheelConfig;

WheelReport wr;

#define MAXAMPS 6.0


// void FfbEngine::SetFfb(FfbReportHandler* reporthandler) {
//   ffbReportHandler = reporthandler;
// }

void setup() {
  DebugSerial.begin(9600);
  delay(1000);

  vSerial.begin(115200);
  vUart.setSerialPort(&vSerial);

  HID().setup();

  ffbEngine.SetFfb(&ffbReportHandler);
  ffbEngine.SetGain(wheelConfig);

  memset(&wr, 0, sizeof(wr));
  HID().SendReport(HID_REPORTID_WHEEL, &wr, sizeof(wr));

  // // TEMP TEST: force one constant FFB effect on.
  // volatile TEffectState& test = ffbReportHandler.gEffectStates[1];

  // test.state = MEFFECTSTATE_PLAYING;
  // test.effectType = USB_EFFECT_CONSTANT;
  // test.magnitude = 10000;                 // full positive force
  // test.gain = 255;                        // full effect gain
  // test.duration = USB_DURATION_INFINITE;  // never expires
  // test.elapsedTime = 0;
  // test.startTime = millis();

  ffbReportHandler.devicePaused = false;

  enc.setup();
}
static uint32_t lastDebugMs = 0;
static uint32_t lastHidUs = 0;

static uint32_t lastFfbUs = 0;

#define USEDEBUG false

void loop() {
  TinyUSB_Device_Task();
  enc.loop();



  wr.x = enc.currentPosition;
  wr.y = 0;
  wr.rx = 0;
  wr.ry = 0;
  wr.z =0;
  wr.rz =0;
  wr.buttons = 0;

  totalForce = ffbEngine.ForceCalculator(enc);    
  totalForce = constrain(totalForce, -255, 255);

  if (USEDEBUG && millis() - lastDebugMs >= 1000) {
    DebugSerial.print("Pos=");
    DebugSerial.print(enc.currentPosition);
    DebugSerial.print(" Force=");
    DebugSerial.println(totalForce);

  DebugSerial.println();

for (uint8_t i = 0; i <= MAX_EFFECTS; i++) {
  volatile TEffectState& e = ffbReportHandler.gEffectStates[i];

  if (e.state != 0 || e.effectType != 0) {
    DebugSerial.print(" S");
    DebugSerial.print(i);

    DebugSerial.print(" state=");
    DebugSerial.print(e.state);

    DebugSerial.print(" play=");
    DebugSerial.print((e.state & MEFFECTSTATE_PLAYING) ? 1 : 0);

    DebugSerial.print(" type=");
    DebugSerial.print(e.effectType);

    DebugSerial.print(" mag=");
    DebugSerial.print(e.magnitude);

    DebugSerial.print(" gain=");
    DebugSerial.print(e.gain);

    DebugSerial.print(" cp=");
    DebugSerial.print(e.cpOffset);

    DebugSerial.print(" posK=");
    DebugSerial.print(e.positiveCoefficient);

    DebugSerial.print(" negK=");
    DebugSerial.print(e.negativeCoefficient);

    DebugSerial.print(" dead=");
    DebugSerial.println(e.deadBand);

    DebugSerial.print(" dur=");
    DebugSerial.print(e.duration);

    DebugSerial.print(" elapsed=");
    DebugSerial.print(e.elapsedTime);

    DebugSerial.print(" posSat=");
DebugSerial.print(e.positiveSaturation);

DebugSerial.print(" negSat=");
DebugSerial.print(e.negativeSaturation);


    bool active =
      (e.state & MEFFECTSTATE_PLAYING) &&
      ((e.elapsedTime <= e.duration) ||
      (e.duration == USB_DURATION_INFINITE)) &&
      !ffbReportHandler.devicePaused;

    DebugSerial.print(" active=");
    DebugSerial.println(active ? 1 : 0);

    DebugSerial.println();
  }

  // DebugSerial.println();
  // DebugSerial.print("curvel");
  // DebugSerial.println(enc.currentVelocity);
  // DebugSerial.print("maxvel");
  // DebugSerial.println(enc.maxVelocity);
  //   DebugSerial.print("curAcc");
  // DebugSerial.println(enc.currentAcceleration);
  //   DebugSerial.print("maxAcc");
  // DebugSerial.println(enc.maxAcceleration);

  // DebugSerial.print("Constant Hits");
  // DebugSerial.println(dbgConstantCaseHits);


  }

    lastDebugMs = millis();
  }
  

  if (micros() - lastHidUs >= 2000) {
    HID().SendReport(HID_REPORTID_WHEEL, &wr, sizeof(wr));
    lastHidUs = micros();
  }
  uint32_t nowUs = micros();

  if ((uint32_t)(nowUs - lastFfbUs) >= 1000) {
    lastFfbUs = nowUs;

    totalForce = ffbEngine.ForceCalculator(enc);
    totalForce = constrain(totalForce, -255, 255);
  }

  vUart.setCurrent(constrain(((float) -totalForce/255) * MAXAMPS,-MAXAMPS,MAXAMPS));

}
