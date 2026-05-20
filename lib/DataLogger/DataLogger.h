#pragma once
#include <Arduino.h>
#include <SD.h>
#include <RTClib.h>

class DataLogger {
private:
    RTC_DS3231 rtc;
    const int chipSelect = 5;
    bool sdAtivo;
    bool rtcAtivo;
    String formatDateTime(const DateTime& dt);

public:
    DataLogger();
    bool iniciar();
    void registrar(const String& dados);
    bool estaAtivo() const { return sdAtivo && rtcAtivo; }
    RTC_DS3231& getRTC() { return rtc; }
};