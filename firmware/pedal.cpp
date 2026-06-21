    #include <iostream>
#include <cmath>
#include "serialib.h"
#include "public.h"
#include "vjoyinterface.h"

#pragma comment( lib, "VJOYINTERFACE" )

#define DEV_ID 1 //general device thing i frogot what it called

#if defined (_WIN32) || defined(_WIN64)
//for serial ports above "COM9", we must use this extended syntax of "\\.\COMx".
//also works for COM0 to COM9.
//https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea?redirectedfrom=MSDN#communications-resources
#define SERIAL_PORT_THROTTLE "\\\\.\\COM17"
#define SERIAL_PORT_BRAKE "\\\\.\\COM10"
#endif
//https://github.com/shauleiz/vJoy/blob/master/SDK/c%23/FeederDemoCS/Program.cs
#define useSerial false

#define MAXRANGE 32767



JOYSTICK_POSITION iReport = { 0 };

class serialDev {
public:
    int baud;
    char errorOpening;
    serialib serial;
    char buf[256*2];
    int n;

    double curPos = 0;

    serialDev(int _baud, bool isThrottle) {
        this->baud = _baud;
        if (isThrottle) {
            errorOpening = this->serial.openDevice(SERIAL_PORT_THROTTLE, this->baud);
        }
        else {
            errorOpening = this->serial.openDevice(SERIAL_PORT_BRAKE, this->baud);
        }
        if (errorOpening != 1) return;
    }
    double read() {
        n = serial.readBytes(this->buf, sizeof(this->buf) - 1, 100); // timeout = 100ms
        this->buf[n] = '\0';
        if (n > 3) {
            try {
                curPos = std::stod(this->buf);
            }
            catch(std::invalid_argument& e) {
                curPos = 0;
            }
//            std::cout << curPos;
        }
        return curPos;
    }
};

int main() {

//    serialib serial;
//    serialib serial2;
//
//    char errorOpening = serial.openDevice(SERIAL_PORT, 9600);
//    char errorOpening

//    if (errorOpening != 1) return errorOpening;
//    printf("Successful connection to %s\n", SERIAL_PORT);

//    char buffer[256];

    serialDev wheel = serialDev(9600,false);
    serialDev wheel2 = serialDev(9600,true);
//    serialDev throttle = serialDev(11250);

    UINT DevID = DEV_ID; //literally fucking useless
    USHORT X = 30000;
    USHORT Y = 10000;
    USHORT Z = 10000;
    BYTE id = 1;
    BOOL ContinuousPOV=FALSE;

    if (!vJoyEnabled()) {
        std::cout << "VJoy Not Enabled";
        return -2;
    }
    if (!AcquireVJD(id)) {
        std::cout << "Failed to acquire vJoy device\n";
        return -2;
    }
    double xPos = 0;
    double xAdd = 0;
    double prevX;
    int cnt =0;
    double scale = 1;
    while (true) {
//        int n = serial.readBytes(buffer, sizeof(buffer) - 1, 100); // timeout = 100ms

//        if (n > 0) {
////            buffer[n] = '\0';
//            if (n > 3) {
//                xPos = std::fmod((std::stod(buffer) / 1200), 1); // one rev = 0->1
        double newX = wheel.read()/-150.0;
        if (newX<0.0) { newX = 0.0; }
        if (newX>1.0) { newX = 1.0; }
        X = (int) (newX * MAXRANGE);

        double newY = wheel2.read()/-190.0;
        if (newY<0.0) { newX = 0.0; }
        if (newY>1.0) { newX = 1.0; }
        Y = (int) (newY * MAXRANGE);

        if (cnt > 5) {
            std::cout << newX << std::endl;
            std::cout << newY << std::endl;
            cnt =0;
        }
        cnt++;

        id = (BYTE)DevID;
        iReport.bDevice = id;
        iReport.wAxisX = X;
        iReport.wAxisY = Y;
        iReport.wAxisZ = Z;

        if (!UpdateVJD(id,(PVOID)&iReport)) {
            std::cout << id;
            std::cout << " fialed \n";
            AcquireVJD(id);
            ContinuousPOV = (BOOL)GetVJDContPovNumber(id);

        }
//        else {
//            std::cout << "sending a pint \n";
//        }
    }

    wheel.serial.closeDevice();

}


