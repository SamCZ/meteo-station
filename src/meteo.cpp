#include "meteo.h"

#include <U8g2lib.h>

#include "Wire.h"

#include <Adafruit_BMP085.h>
#include "Adafruit_HTU21DF.h"

Adafruit_BMP085 bmp;
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

Meteoradar::Meteoradar() : _Initialized(false), _Temperature(0), _Pressure(0), _Altitude(0), _SealevelPressure(0), _Humidity(0)
{

}

void Meteoradar::Begin()
{
    if (!bmp.begin()) {
        Serial.println("Could not find a valid BMP085 sensor, check wiring!");
        return;
    }

    if (!htu.begin()) {
        Serial.println("Couldn't find sensor!");
        return;
    }

    _Initialized = true;
}

void Meteoradar::Update()
{
    if(!_Initialized) {
        return;
    }

    float temp1 = bmp.readTemperature();
    _Pressure = bmp.readPressure() / 1000.0f;
    _Altitude = bmp.readAltitude();
    _SealevelPressure = bmp.readSealevelPressure() / 1000.0f;

    float temp2 = htu.readTemperature();
    _Humidity = htu.readHumidity();

    _Temperature = (temp1 + temp2) / 2.0f;
}

float Meteoradar::GetTemperature()
{
    return _Temperature;
}

float Meteoradar::GetPressure()
{
    return _Pressure;
}

float Meteoradar::GetAltitude()
{
    return _Altitude;
}

float Meteoradar::GetSealevelPressure()
{
    return _SealevelPressure;
}

float Meteoradar::GetHumidity()
{
    return _Humidity;
}

void Meteoradar::WriteToDisplay(U8G2& u8g2)
{
    String line1 = "T: " + String(_Temperature) + "C | P: " + String(_Pressure) + "mPa";
    String line2 = "H: " + String(_Humidity) + "% | SP: " + String(_SealevelPressure) + "mPa";
    String line3 = "A: " + String(_Altitude) + "m";

    u8g2.setFont(u8g2_font_6x12_tf);

    u8g2.drawStr(0,8, line1.c_str());
    u8g2.drawStr(0,17, line2.c_str());
    u8g2.drawStr(0,26, line3.c_str());
}