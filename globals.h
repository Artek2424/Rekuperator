#ifndef GLOBALS_H
#define GLOBALS_H


// Deklaracje zmiennych globalnych
    extern float tempCzerpnia, tempWyrzutnia, tempWywiew, humidityWywiew, tempNawiew, tempCzerpniaSetpoint, humiditySetpoint, humiditySetpointWinter, humiditySetpointSummer;
    extern int fanSpeedWywiew, fanSpeedNawiew, flowRateWywiew, flowRateNawiew, servoBypass, servoRecyrkulacja, fanSpeedParty, fanSpeedManualDay, fanSpeedManualNight, seasonSelect,
     nightModeSwitchHour, dayModeSwitchHour, exhaustFilterFillLevel, supplyFilterFillLevel, partyModeTime, minSpeedFan, maxSpeedFanDay, maxSpeedFanNight, fanRpmWywiew, fanRpmNawiew;
    extern time_t czas;
    extern bool rekuModeAuto, autoFlowControl, humidityTempSensorError, flowSensorWywiewError, flowSensorNawiewError, fanWywiewError, fanNawiewError, tempSensorCzerpniaError,
     tempSensorWyrzutniaError, tempSensorWywiewError, tempSensorNawiewError, dayNightMode, winterSummerMode;
    extern std::vector<String> messagesList;


#endif