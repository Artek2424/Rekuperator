#ifndef CONFIGUREREADSENSORS_H
#define CONFIGUREREADSENSORS_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <SensirionI2CSdp.h>
#include <SHT85.h>
#include <DS3232RTC.h>
#include <TimeLib.h>

void setupSensors();
void readSensors();

#endif
