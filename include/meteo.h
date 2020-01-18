#pragma once

class U8G2;

class Meteoradar {
private:
    bool _Initialized : 1;

    float _Temperature;
    float _Pressure;
    float _Altitude;
    float _SealevelPressure;
    float _Humidity;
public:
    Meteoradar();

    void Begin();

    void Update();

    float GetTemperature();
    float GetPressure();
    float GetAltitude();
    float GetSealevelPressure();
    float GetHumidity();

    void WriteToDisplay(U8G2& u8g2);
};