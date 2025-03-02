#ifndef RECUWEBSERVER_H
#define RECUWEBSERVER_H

#include <WebServer.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <EEPROM.h>

// Deklaracja serwera
extern WebServer server;



// Deklaracje funkcji
void setupRecuWebserver();
void handleRoot();
void handleData();
void handleSetHumiditySetpointWinter();
void handleSetHumiditySetpointSummer();
void handleSetFanSpeedParty();
void handleSetTempCzerpniaSetpoint();
void handleSetSeasonSelect();
void handleToggle();
void handleToggleAutoFlowControl();
void handleSetDayModeSwitchHour();
void handleSetNightModeSwitchHour();
void handleSetTime();
void handleGetMessages();
void handleResetMessage();
void addMessage(const String& message);
void resetMessages();
void saveToEEPROM();
void loadFromEEPROM();
void handleSetPartyModeTime();
void handleSetMinSpeedFan();
void handleSetMaxSpeedFanDay();
void handleSetMaxSpeedFanNight();

#endif
