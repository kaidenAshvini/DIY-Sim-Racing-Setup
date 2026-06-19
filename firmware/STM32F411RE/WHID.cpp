#include "WHID.h"

HID_& HID()
{
  static HID_ obj;
  return obj;
}

Adafruit_USBD_HID usb_hid;

volatile uint32_t ffbOutputCount = 0;
volatile uint8_t lastFfbReportId = 0;
volatile uint16_t lastFfbSize = 0;
volatile uint8_t lastFfbByte0 = 0;
volatile uint8_t lastFfbByte1 = 0;
volatile uint8_t lastFfbType = 0;

// heres where it gets fucking tricky
// #define HID_REPORT_TYPE_FEATURE 3
FfbReportHandler ffbReportHandler;

uint16_t get_report_callback(
  uint8_t report_id,
  hid_report_type_t report_type,
  uint8_t* buffer,
  uint16_t reqlen
) {
  memset(buffer, 0, reqlen);

  // ------------------------------------------------------------
  // Input report ID 2: PID State
  // Buffer excludes report ID.
  // ------------------------------------------------------------
  if (report_type == HID_REPORT_TYPE_INPUT && report_id == 2) {
    if (reqlen < 2) return 0;

    uint8_t status = 0;

    // Mirror your old working implementation:
    // bit 1 = actuators enabled
    // bit 4 = actuator power
    if (ffbReportHandler.pidState.status & 0x02) {
      status |= 0x02;
      status |= 0x10;
    }

    if (ffbReportHandler.devicePaused) {
      status |= 0x01;
    }

    buffer[0] = status;

    // bit 0 = an effect is playing
    // bits 1..7 = effect block index
    uint8_t playingId = 0;

    for (uint8_t i = 1; i <= MAX_EFFECTS; i++) {
      if (ffbReportHandler.gEffectStates[i].state & MEFFECTSTATE_PLAYING) {
        playingId = i;
        break;
      }
    }

    if (playingId != 0) {
      buffer[1] = 0x01 | ((playingId & 0x7F) << 1);
    } else {
      buffer[1] = 0;
    }

    return 2;
  }

  // ------------------------------------------------------------
  // Feature report ID 6: PID Block Load
  // Buffer excludes report ID.
  // ------------------------------------------------------------
  if (report_type == HID_REPORT_TYPE_FEATURE && report_id == 6) {
    if (reqlen < 4) return 0;

    const auto& load = ffbReportHandler.pidBlockLoad;

    buffer[0] = load.effectBlockIndex;
    buffer[1] = load.loadStatus;
    buffer[2] = (uint8_t)(load.ramPoolAvailable & 0xFF);
    buffer[3] = (uint8_t)(load.ramPoolAvailable >> 8);

    return 4;
  }

  // ------------------------------------------------------------
  // Feature report ID 7: PID Pool
  // Buffer excludes report ID.
  // ------------------------------------------------------------
  if (report_type == HID_REPORT_TYPE_FEATURE && report_id == 7) {
    if (reqlen < 4) return 0;

    uint16_t poolSize = MEMORY_SIZE;

    buffer[0] = (uint8_t)(poolSize & 0xFF);
    buffer[1] = (uint8_t)(poolSize >> 8);
    buffer[2] = MAX_EFFECTS;
    buffer[3] = 0x03;

    return 4;
  }

  return 0;
}

volatile uint32_t createEffectCount = 0;
volatile uint8_t createReportId = 0;
volatile uint16_t createSize = 0;
volatile uint8_t lastFfbBytes[8] = {};
volatile uint8_t lastFfbCapturedLen = 0;
volatile uint32_t pidStatusGetCount = 0;

static uint8_t previousId = 0xFF;

void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  if (report_type == HID_REPORT_TYPE_FEATURE && report_id == 5) 
  {
    if (bufsize < 3) return;

    USB_FFBReport_CreateNewEffect_Feature_Data_t request = {};

    request.reportId = report_id;
    request.effectType = buffer[0];
    request.byteCount =
      (uint16_t)buffer[1] |
      ((uint16_t)buffer[2] << 8);

    ffbReportHandler.FfbOnCreateNewEffect(&request);
    return;
  }
  if (report_type == HID_REPORT_TYPE_OUTPUT && bufsize > 0)
  {
    lastFfbCapturedLen = min<uint16_t>(bufsize, 8);

    for (uint8_t i = 0; i < lastFfbCapturedLen; i++) {
      lastFfbBytes[i] = buffer[i];
    }

    ffbReportHandler.FfbOnUsbData((uint8_t*)buffer, bufsize);
    uint8_t actualReportId = buffer[0];



  }

// if (bufsize > 0) {
//     ffbOutputCount++;
//     lastFfbReportId = report_id;
//     lastFfbSize = bufsize;

//     lastFfbType = (uint8_t)report_type;
//     lastFfbByte0 = buffer[0];
//     lastFfbByte1 = (bufsize > 1) ? buffer[1] : 0;

//     ffbReportHandler.FfbOnUsbData((uint8_t*)buffer, bufsize);
//   }
}

void HID_::setup() {
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }
  usb_hid.enableOutEndpoint(true);
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(_hidReportDescriptor, sizeof(_hidReportDescriptor));
  usb_hid.setStringDescriptor("TinyUSB HID Generic");
  usb_hid.setReportCallback(get_report_callback, set_report_callback);
  usb_hid.begin();

  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
}

int HID_::SendReport(uint8_t id, const void* data, int len) {
  if (!TinyUSBDevice.mounted()) {
    return -1;
  }

  if (!usb_hid.ready()) {
    return -2;
  }
    return usb_hid.sendReport(id, data, len);
}