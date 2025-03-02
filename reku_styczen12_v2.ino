// Dla mikrokontrolera ESP32

#include <WiFi.h>
#include <ESPmDNS.h>
//#include <HTTPUpdateServer.h>
//#include <NetworkClient.h>

#include <vector>
#include "recuWebServer.h"
#include "globals.h"
#include "configureReadSensors.h"
#include "executiveDevices.h"


const char* ssid = "realme GT 2 Pro";
const char* password = "123456789";

const int nominalFansRpm=1500;	// Prędkość znamionowa(maksymalna) wentylatorów (stała)

// Zmienne monitorujące stan rekuperatora
float tempCzerpnia = 0; // z czujnika ds18b20
float tempCzerpniaSetpoint = 2; // zadawana z wizualizacji
float tempWyrzutnia = 0; // z czujnika SHT31
float tempWywiew = 0; // z czujnika ds18b20
float tempNawiew = 0; // z czujnika ds18b20
float humidityWywiew = 0; // z czujnika SHT31
int fanSpeedWywiew = 0;      // Prędkość wentylatora w % obliczana
int fanSpeedNawiew = 0;      // Prędkość wentylatora w % obliczana
int fanRpmWywiew = 0;     // Prędkość wentylatora wywiew rpm z czujnika
int fanRpmNawiew = 0;     // Prędkość wentylatora wywiew rpm z czujnika
int partyModeTime = 30;      // Czas trwania trybu manual w minutach zadawany z wizualizacji
int minSpeedFan = 20;         // Minimalna prędkość wentylatora w % zadawana z wizualizacji
int maxSpeedFanDay = 80;      // Maksymalna prędkość wentylatora w trybie dzień w % zadawana z wizualizacji
int maxSpeedFanNight = 50;    // Maksymalna prędkość wentylatora w trybie noc w % zadawana z wizualizacji
int flowRateWywiew = 50;	// Wartość odczytywana z czujnika przepływu
int flowRateNawiew = 50;	// Wartość odczytywana z czujnika przepływu
int servoBypass = 0;         // Pozycja w %
int servoRecyrkulacja = 0;  // Pozycja w %
int exhaustFilterFillLevel = 0; // Poziom zapełnienia filtra wywiew w %
int supplyFilterFillLevel = 0; // Poziom zapełnienia filtra nawiew w %
time_t czas = now();         // Aktualny czas w formacie Unix
int fanSpeedParty = 50; // prędkość wentylatora w % zadawana z wizualizacji
int fanSpeedManualDay = 50; // prędkość wentylatora w % zadawana z wizualizacji
int fanSpeedManualNight = 30; // prędkość wentylatora w % zadawana z wizualizacji
float humiditySetpoint = 45.0;  // Wilgotność zadana w %, , zadawana z wizualizacji DO USUNIĘCIA
float humiditySetpointWinter = 45.0;  // Wilgotność zadana w % zima, zadawana z wizualizacji
float humiditySetpointSummer = 50.0;  // Wilgotność zadana w % lato, , zadawana z wizualizacji
bool autoFlowControl = true;	// Automatyczne balansowanie przepływu strumieni
bool rekuModeAuto = true;	// Regulacja automatyczna/manual
int seasonSelect = 0;  //  0 = Auto, 1 = Lato, 2 = Zima , zadawana z wizualizacji
bool winterSummerMode = true; // true = zima, false = lato 
bool dayNightMode = true; // true = dzień, false = noc

// Zmienne do przechowywania godzin przejścia na tryb nocny i dzienny
int nightModeSwitchHour = 20;  // Domyślna godzina przejścia na tryb nocny (16-23), zadawana z wizualizacji
int dayModeSwitchHour = 7;     // Domyślna godzina przejścia na tryb dzienny (5-10), zadawana z wizualizacji

// Zmienne do obsługi błędów
std::vector<String> messagesList;
bool humidityTempSensorError = false;
bool flowSensorWywiewError = false;
bool flowSensorNawiewError = false;
bool fanWywiewError = false;
bool fanNawiewError = false;
bool tempSensorCzerpniaError = false;
bool tempSensorWyrzutniaError = false;
bool tempSensorWywiewError = false;
bool tempSensorNawiewError = false;





void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  /*while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  */
  Serial.println("WiFi connected");
  MDNS.begin("rekuperator");
  MDNS.addService("http", "tcp", 80);

  setupRecuWebserver();
  setupSensors();
  setupExecutiveDevices();
  
}

void loop() {
  czas = now();
  server.handleClient();
  readSensors();
  executiveDevices();
  
  
}
