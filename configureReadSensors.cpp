#include "configureReadSensors.h"
#include "globals.h"
#include "recuWebServer.h"


OneWire oneWire(32);
DallasTemperature sensors(&oneWire);
DeviceAddress addrCzerpnia = { /* Insert address */ };  //ODCZYTAĆ I WPISAĆ ADRESY CZUJNIKÓW
DeviceAddress addrWyrzutnia = { /* Insert address */ };
DeviceAddress addrNawiew = { /* Insert address */ };

// Inicjalizacja czujników
SensirionI2CSdp sdpWywiew;
SensirionI2CSdp sdpNawiew;
SHT31 sht31(0x44);

// Deklaracja czasów dla każdej funkcji
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long previousMillis4 = 0;

// Interwał czasowy między wywołaniami funkcji
const unsigned long interval = 2000;
const unsigned long offset = 500; // Przesunięcie czasowe między funkcjami


volatile unsigned long lastPulseTimeWywiew = 0;
volatile unsigned long lastPulseTimeNawiew = 0;
volatile int pulseCountWywiew = 0;
volatile int pulseCountNawiew = 0;

void IRAM_ATTR fanWywiewInterrupt() {
  lastPulseTimeWywiew = millis();
  pulseCountWywiew++;
}

void IRAM_ATTR fanNawiewInterrupt() {
  lastPulseTimeNawiew = millis();
  pulseCountNawiew++;
}


void readTemperatureSensors() {
  // Wysłanie polecenia odczytu temperatur
  sensors.requestTemperatures();

  // Odczyt temperatury z czujnika "Czerpnia"
  tempCzerpnia = sensors.getTempC(addrCzerpnia);
  if (tempCzerpnia == DEVICE_DISCONNECTED_C || tempCzerpnia < -55.0 || tempCzerpnia > 125.0) {
    tempSensorCzerpniaError = true;  // Wykryto błąd
    addMessage("Błąd odczytu temperatury czerpnia");
  } else {
    tempSensorCzerpniaError = false; // Odczyt poprawny
  }

  // Odczyt temperatury z czujnika "Wyrzutnia"
  tempWyrzutnia = sensors.getTempC(addrWyrzutnia);
  if (tempWyrzutnia == DEVICE_DISCONNECTED_C || tempWyrzutnia < -55.0 || tempWyrzutnia > 125.0) {
    tempSensorWyrzutniaError = true;  // Wykryto błąd
    addMessage("Błąd odczytu temperatury wyrzutnia");
  } else {
    tempSensorWyrzutniaError = false; // Odczyt poprawny
  }

  // Odczyt temperatury z czujnika "Nawiew"
  tempNawiew = sensors.getTempC(addrNawiew);
  if (tempNawiew == DEVICE_DISCONNECTED_C || tempNawiew < -55.0 || tempNawiew > 125.0) {
    tempSensorNawiewError = true;  // Wykryto błąd
    addMessage("Błąd odczytu temperatury nawiew");
  } else {
    tempSensorNawiewError = false; // Odczyt poprawny
  }
}


void readFlowSensors() {
  float differentialPressure, temperature;
  int ret = sdpWywiew.readMeasurement(differentialPressure, temperature);
  if (ret != 0) {
    addMessage("Błąd odczytu przepływu wywiew");
    flowSensorWywiewError = true;
  } else {
    flowRateWywiew = differentialPressure;
    flowSensorWywiewError = false;
  }

  ret = sdpNawiew.readMeasurement(differentialPressure, temperature);
  if (ret != 0) {
    addMessage("Błąd odczytu przepływu nawiew");
    flowSensorNawiewError = true;
  } else {
    flowRateNawiew = differentialPressure;
    flowSensorNawiewError = false;
  }
}



void readHumiditySensor() {
  if (!sht31.read()) {
    addMessage("Błąd odczytu wilgotności i temperatury wywiew");
    humidityTempSensorError = true;
    tempSensorWywiewError = true;
  } else {
    humidityWywiew = sht31.getHumidity();
    tempWywiew = sht31.getTemperature();
    humidityTempSensorError = false;
    tempSensorWywiewError = false;
  }
}


void calculateFanSpeeds() {
  unsigned long currentTimeCalc = millis();


  // Wywiew
  noInterrupts();
  int pulsesWywiew = pulseCountWywiew;
  unsigned long lastPulseWywiew = lastPulseTimeWywiew;
  pulseCountWywiew = 0;
  interrupts();

  if (currentTimeCalc > lastPulseWywiew && pulsesWywiew > 0) {
    unsigned long timeDeltaWywiew = currentTimeCalc - lastPulseWywiew;
    if (timeDeltaWywiew > 0) {
      fanRpmWywiew = (pulsesWywiew * 60000) / timeDeltaWywiew;
    } else {
      fanRpmWywiew = 0;
    }
  } else {
    fanRpmWywiew = 0;
  }

  // Nawiew
  noInterrupts();
  int pulsesNawiew = pulseCountNawiew;
  unsigned long lastPulseNawiew = lastPulseTimeNawiew;
  pulseCountNawiew = 0;
  interrupts();

  if (currentTimeCalc > lastPulseNawiew && pulsesNawiew > 0) {
    unsigned long timeDeltaNawiew = currentTimeCalc - lastPulseNawiew;
    if (timeDeltaNawiew > 0) {
      fanRpmNawiew = (pulsesNawiew * 60000) / timeDeltaNawiew;
    } else {
      fanRpmNawiew = 0;
    }
  } else {
    fanRpmNawiew = 0;
  }
}


void setupSensors() {
  sensors.begin();

  pinMode(4, INPUT);  // Przerwania z czujników halla wentylatorów
  attachInterrupt(digitalPinToInterrupt(4), fanWywiewInterrupt, FALLING);

  pinMode(23, INPUT);
  attachInterrupt(digitalPinToInterrupt(23), fanNawiewInterrupt, FALLING);

  Wire.begin(27, 26); // Inicjalizacja magistrali I2C 1 i 2
  Wire1.begin(25, 33);


  // Inicjalizacja czujników Sensirion SDP810
  sdpWywiew.begin(Wire, SDP8XX_I2C_ADDRESS_0);
  sdpNawiew.begin(Wire1, SDP8XX_I2C_ADDRESS_0);
  sdpWywiew.stopContinuousMeasurement();
  sdpNawiew.stopContinuousMeasurement();
  if (sdpWywiew.startContinuousMeasurementWithDiffPressureTCompAndAveraging()) {
    addMessage("Błąd inicjalizacji czujnika przepływu wywiew");
  }
  if (sdpNawiew.startContinuousMeasurementWithDiffPressureTCompAndAveraging()) {
    addMessage("Błąd inicjalizacji czujnika przepływu nawiew");
  }

  // Inicjalizacja SHT31
  if (!sht31.begin()) {
   addMessage("Błąd inicjalizacji czujnika wilgotności i temperatury wywiew");
  }
  

// Inicjalizacja czasów
  unsigned long startMillis = millis();
  previousMillis1 = startMillis;
  previousMillis2 = startMillis;
  previousMillis3 = startMillis;
  previousMillis4 = startMillis;

} 


void readSensors() {
  unsigned long currentMillis = millis();

  // Wywołanie funkcji 1 co 2000 ms
  if (currentMillis - previousMillis1 >= interval) {
    previousMillis1 += interval; // Utrzymanie synchronizacji
    readTemperatureSensors();
  }

  // Wywołanie funkcji 2 co 2000 ms, przesunięcie 500 ms
  if (currentMillis - previousMillis2 >= interval + offset) {
    previousMillis2 += interval; // Utrzymanie synchronizacji
    readFlowSensors();
  }

  // Wywołanie funkcji 3 co 2000 ms, przesunięcie 1000 ms
  if (currentMillis - previousMillis3 >= interval + 2 * offset) {
    previousMillis3 += interval; // Utrzymanie synchronizacji
    readHumiditySensor();
  }

  // Wywołanie funkcji 4 co 2000 ms, przesunięcie 1500 ms
  if (currentMillis - previousMillis4 >= interval + 3 * offset) {
    previousMillis4 += interval; // Utrzymanie synchronizacji
    calculateFanSpeeds();
  }
}
