#include "DataLogger.h"

DataLogger::DataLogger() : sdAtivo(false), rtcAtivo(false) {}

String DataLogger::formatDateTime(const DateTime& dt) {
    char buf[20];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
    return String(buf);
}

bool DataLogger::iniciar() {
    if (rtc.begin()) {
        rtcAtivo = true;
        Serial.println("[DataLogger] RTC iniciado.");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    } else {
        Serial.println("[DataLogger] ERRO: RTC ausente.");
    }

    if (!SD.begin(chipSelect)) {
        Serial.println("[DataLogger] ERRO: Cartao SD ausente.");
        return false;
    }
    sdAtivo = true;
    Serial.println("[DataLogger] Cartao SD OK.");
    return true;
}

void DataLogger::registrar(const String& dados) {
    if (!sdAtivo) return;
    File arq = SD.open("/grow_log.csv", FILE_APPEND);
    if (arq) {
        if (rtcAtivo) arq.print(formatDateTime(rtc.now()) + ",");
        else arq.print("SEM_RTC,");
        arq.println(dados);
        arq.close();
    }
}