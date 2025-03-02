#include "recuWebServer.h"
#include "globals.h"
#include "executiveDevices.h"

// Piny PWM dla wentylatorów
#define FAN_WYWIEW_PIN 16
#define FAN_NAWIEW_PIN 12


// PID dla wilgotności
float pidHumidityInput = 0, pidHumidityOutput = 0, pidHumiditySetpoint = 0;
QuickPID pidHumidity(&pidHumidityInput, &pidHumidityOutput, &pidHumiditySetpoint,
                     2.0, 0.2, 0.01, QuickPID::pMode::pOnError, QuickPID::dMode::dOnMeas, QuickPID::iAwMode::iAwCondition, QuickPID::Action::reverse);

// PID dla bilansowania przepływu
float pidFlowInput = 0, pidFlowOutput = 0, pidFlowSetpoint = 0;
QuickPID pidFlow(&pidFlowInput, &pidFlowOutput, &pidFlowSetpoint,
                 3.0, 0.2, 0.01, QuickPID::pMode::pOnError, QuickPID::dMode::dOnMeas, QuickPID::iAwMode::iAwCondition, QuickPID::Action::direct);

void setupExecutiveDevices() {
    pinMode(FAN_WYWIEW_PIN, OUTPUT);
    pinMode(FAN_NAWIEW_PIN, OUTPUT);

    pidHumidity.SetMode(QuickPID::Control::automatic);
    pidHumidity.SetOutputLimits(0, 255);

    pidFlow.SetMode(QuickPID::Control::automatic);
    pidFlow.SetOutputLimits(-50, 50);

    analogWrite(FAN_WYWIEW_PIN, 0);
    analogWrite(FAN_NAWIEW_PIN, 0);
}

void executiveDevices() {
    // Ustalanie trybu dzienny/nocny
    int currentHour = hour(czas);
    dayNightMode = (currentHour >= dayModeSwitchHour && currentHour < nightModeSwitchHour);
    flowRateNawiew = fanSpeedParty;

    // Obliczanie min i max PWM tylko raz na cykl
    int minPwm = map(minSpeedFan, 0, 100, 0, 255);
    int maxPwm = map(dayNightMode ? maxSpeedFanDay : maxSpeedFanNight, 0, 100, 0, 255);

    int avgFanSpeed = 0; // Wyliczona średnia prędkość wentylatorów

    // Obliczanie średniej prędkości na podstawie wilgotności
    if (rekuModeAuto) {
        pidHumiditySetpoint = winterSummerMode ? humiditySetpointWinter : humiditySetpointSummer;
        pidHumidityInput = humidityWywiew;
        pidHumidity.Compute();
        avgFanSpeed = (int)pidHumidityOutput;
    } else {
        // Tryb ręczny: średnia prędkość ustawiana na podstawie ustawień manualnych, mapowanie z 0-100 % na PWM
        avgFanSpeed = map(dayNightMode ? fanSpeedManualDay : fanSpeedManualNight, 0, 100, minPwm, maxPwm);
    }

    // Obliczanie przepływów
    if (autoFlowControl) {
        pidFlowSetpoint = flowRateWywiew; // Bilansowanie względem wywiewu
        pidFlowInput = flowRateNawiew;
        pidFlow.Compute();
    } else {
        pidFlowOutput = 0; // Bez bilansowania
    }

    // Dostosowanie prędkości wentylatorów
    // Zmienne PWM
    int fanWywiewPwm = avgFanSpeed - (int)round(pidFlowOutput); // Kompensacja bilansu, zaokrąglenie w dół lub w górę dla zmiany PWM o 1 i rzutowanie na typ integer
    int fanNawiewPwm = avgFanSpeed + (int)pidFlowOutput; 

    // Ulepszone przenoszenie korekcji na drugi wentylator po osiągnięciu limitu prędkości, dla zachowania rónowagi przepływów.  TO JUŻ DZIAŁA
    // Niezależnie który wentylator ma większe opory, drugi koryguje prędkość by utrzymać bilans przepływu na 0.
    if (fanWywiewPwm < minPwm) {
        int correction = minPwm - fanWywiewPwm;
        Serial.print("correction: "); // Debug
        Serial.println(correction);
        fanWywiewPwm = minPwm;
        fanNawiewPwm += correction;
        if (fanNawiewPwm > maxPwm) {
            fanNawiewPwm = maxPwm;
        }
    } else if (fanWywiewPwm > maxPwm) {
        int correction = fanWywiewPwm - maxPwm;
        fanWywiewPwm = maxPwm;
        fanNawiewPwm -= correction;
        if (fanNawiewPwm < minPwm) {
            fanNawiewPwm = minPwm;
        }
    }

    if (fanNawiewPwm < minPwm) {
        int correction = minPwm - fanNawiewPwm;
        fanNawiewPwm = minPwm;
        fanWywiewPwm += correction;
        if (fanWywiewPwm > maxPwm) {
            fanWywiewPwm = maxPwm;
        }
    } else if (fanNawiewPwm > maxPwm) {
        int correction = fanNawiewPwm - maxPwm;
        fanNawiewPwm = maxPwm;
        fanWywiewPwm -= correction;
        if (fanWywiewPwm < minPwm) {
            fanWywiewPwm = minPwm;
        }
    }

    // Aktualizacja PWM
    analogWrite(FAN_WYWIEW_PIN, fanWywiewPwm); // Przepisz na wyjścia
    analogWrite(FAN_NAWIEW_PIN, fanNawiewPwm);

      // Debug
    Serial.print("PWM Wywiew: ");
    Serial.print(fanWywiewPwm);
    Serial.print("\tPWM Nawiew: ");
    Serial.print(fanNawiewPwm);
    Serial.print("\tpidHumidityOutput: ");
    Serial.print(pidHumidityOutput);
    Serial.print("\tpidFlowOutput: ");
    Serial.println(pidFlowOutput);

    delay(2000); // Odczyt co 2 sekundy, test PID, blokuje procesor 
}
