#include "recuWebServer.h"
#include "globals.h"

// Implementacja serwera
WebServer server(80);

// Funkcje obsługi żądań
void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Rekuperator</title>
            <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #cfcfd4; color: #333; }
        .container { padding: 5px; max-width: 600px; margin: auto; }
        h2 { text-align: center; color: #0056b3; }
        h3 { margin: 5px 0; color: #0056b3; }
        .sensor, .control { margin: 5px; padding: 3px; background: #fff; border: 1px solid #ddd; border-radius: 8px; }
        .focus  { margin: 5px; padding: 3px; background:#edeef5; border: 2px solid #ddd; border-radius: 8px; }
        .sensor span { font-weight: bold; }
        .field {
            display: inline-block; /* Elementy w jednej linii, jeśli jest miejsce */
            margin-right: 20px; /* Odstęp między polami */
          }
        button, input[type="range"], select {
          width: auto;
          min-width: 25%;
          padding: 3px;
          margin-top: 5px;
          border: 1px solid #0056b3;
          border-radius: 5px;
          background-color: #007bff;
          color: white;
          font-size: 15px;
        }
        button:hover, input[type="range"]:hover {
          background-color: #0056b3;
        }
        input[type="range"] { width: 80%; background: #f4f4f9; }
      </style>
        </head>
        <body>
            <div class="container">
                <h2>Rekuperator Monitor</h2>
                <div class="sensor">Temperatura czerpnia: <span id="tempCzerpnia">--</span> &deg;C</div>
                <div class="sensor">Temperatura wyrzutnia: <span id="tempWyrzutnia">--</span> &deg;C</div>
                <div class="sensor">Temperatura wywiew: <span id="tempWywiew">--</span> &deg;C</div>
                <div class="sensor">Temperatura nawiew: <span id="tempNawiew">--</span> &deg;C</div>
                <div class="sensor">Wilgotność wywiew: <span id="humidityWywiew">--</span> %</div>
                <div class="sensor">Predkość wentylatora wywiew: <span id="fanSpeedWywiew">--</span> %</div>
                <div class="sensor">Predkość wentylatora nawiew: <span id="fanSpeedNawiew">--</span> %</div>
                <div class="sensor">Przepływ powietrza wywiew: <span id="flowRateWywiew">--</span> m&sup3;/h</div>
                <div class="sensor">Przepływ powietrza nawiew: <span id="flowRateNawiew">--</span> m&sup3;/h</div>
                <div class="sensor">Poziom zapełnienia filtra wywiew: <span id="exhaustFilterFillLevel">--</span> %</div>
                <div class="sensor">Poziom zapełnienia filtra nawiew: <span id="supplyFilterFillLevel">--</span> %</div>
                <div class="sensor">Pozycja serwa bypass: <span id="servoBypass">--</span> %</div>
                <div class="sensor">Pozycja serwa recyrkulacja: <span id="servoRecyrkulacja">--</span> %</div>
                
                
                <div class="control">
                  <h3>Ustawienia trybu automatycznego</h3>
                  <button id="rekuModeAutoButton" onclick="toggleRekuModeAuto()">Tryb automatyczny</button>
                  <button id="autoFlowControlButton" onclick="toggleAutoFlowControl()">Kontrola przepływu on</button>
                  <div class="focus">
                  <h3>Godzina przejścia na tryb dzienny</h3>
                  <div class="field">Aktualna ustawiona: <span id="dayModeSwitchHourCurrent">--</span></div>
                  <div class="field">Pozycja suwaka: <span id="dayModeSwitchHourDisplay">7</span></div>
                  <input id="dayModeSwitchHourControl" type="range" min="5" max="10" step="1" value="7" oninput="updateDayModeSwitchHourDisplay()">
                  <button onclick="setDayModeSwitchHour()">Ustaw godzinę dzień</button>
                  <h3>Maksymalna pędkość wentylatora dzień</h3>
                  <div class="field">Aktualna wartość: <span id="maxSpeedFanDayCurrent">--</span> %</div>
                  <div class="field">Pozycja suwaka: <span id="maxSpeedFanDayDisplay">80</span> %</div>
                  <input id="maxSpeedFanDayControl" type="range" min="10" max="100" step="1" value="80" oninput="updateMaxSpeedFanDayDisplay()">
                  <button onclick="setMaxSpeedFanDay()">Ustaw prędkość</button>
                  </div>

                  <div class="focus">
                  <h3>Godzina przejścia na tryb nocny</h3>
                  <div class="field">Aktualna ustawiona: <span id="nightModeSwitchHourCurrent">--</span></div>
                  <div class="field">Pozycja suwaka: <span id="nightModeSwitchHourDisplay">20</span></div>
                  <input id="nightModeSwitchHourControl" type="range" min="16" max="23" step="1" value="20" oninput="updateNightModeSwitchHourDisplay()">
                  <button onclick="setNightModeSwitchHour()">Ustaw godzinę noc</button>
                  <h3>Maksymalna prędkość wentylatora noc</h3>
                  <div class="field">Aktualna wartość: <span id="maxSpeedFanNightCurrent">--</span> %</div>
                  <div class="field">Pozycja suwaka: <span id="maxSpeedFanNightDisplay">20</span> %</div>
                  <input id="maxSpeedFanNightControl" type="range" min="10" max="100" step="1" value="20" oninput="updateMaxSpeedFanNightDisplay()">
                  <button onclick="setMaxSpeedFanNight()">Ustaw prędkość</button>
                  </div>

                  <div class="focus">
                  <h3>Minimalna prędkość wentylatora</h3>
                  <div class="field">Aktualna wartość: <span id="minSpeedFanCurrent">--</span> %</div>
                  <div class="field">Pozycja suwaka: <span id="minSpeedFanDisplay">10</span> %</div>
                  <input id="minSpeedFanControl" type="range" min="5" max="50" step="1" value="10" oninput="updateMinSpeedFanDisplay()">
                  <button onclick="setMinSpeedFan()">Ustaw prędkość</button>
                  </div>

                  <div class="focus">
                    <h3>Mimalna wilgotność zadana zima</h3>
                    <div class="field">Aktualna wartość: <span id="humiditySetpointWinterCurrent">--</span>%</div>
                    <div class="field">Pozycja suwaka: <span id="humiditySetpointWinterDisplay">45</span>%</div>
                    <input id="humiditySetpointWinterControl" type="range" min="30" max="65" step="1" value="45" oninput="updateHumiditySetpointWinterDisplay()">
                    <button onclick="setHumiditySetpointWinter()">Ustaw wilgotność</button>
                  </div>

                  <div class="focus">
                    <h3>Mimalna wilgotność zadana lato</h3>
                    <div class="field">Aktualna wartość: <span id="humiditySetpointSummerCurrent">--</span>%</div>
                    <div class="field">Pozycja suwaka: <span id="humiditySetpointSummerDisplay">45</span>%</div>
                    <input id="humiditySetpointSummerControl" type="range" min="30" max="65" step="1" value="45" oninput="updateHumiditySetpointSummerDisplay()">
                    <button onclick="setHumiditySetpointSummer()">Ustaw wilgotność</button>
                  </div>

                  <div class="focus">
                    <h3>Minimalna temperatura czerpnia</h3>
                    <div class="field">Aktualna wartość: <span id="tempCzerpniaSetpointCurrent">--</span>&deg;C</div>
                    <div class="field">Pozycja suwaka: <span id="tempCzerpniaSetpointDisplay">1.0</span>&deg;C</div>
                    <input id="tempCzerpniaSetpointControl" type="range" min="0" max="10" step="0.1" value="1.0" oninput="updateTempCzerpniaSetpointDisplay()">
                    <button onclick="setTempCzerpniaSetpoint()">Ustaw temperaturę</button>
                  </div>

                  <div class="focus">
                    <h3>Pora roku</h3>
                    <div>Pora roku: <span id="seasonSelectDisplay">Auto</span></div>
                    <button onclick="setSeasonSelect(0)">Auto</button>
                    <button onclick="setSeasonSelect(1)">Lato</button>
                    <button onclick="setSeasonSelect(2)">Zima</button>
                  </div>

                  <div class="focus">
                    <h3>Zapisz ustawienia</h3>
                    <button onclick="saveToEEPROM()">Zapisz do EEPROM</button>
                  </div>
                </div>

                <div class="control">
                <h3>Ustawienia trybu przewietrzanie</h3>
                  <div class="focus">
                    <h3>Prędkość wentylatora</h3>
                    <div class="field">Aktualna wartość: <span id="fanSpeedPartyCurrent">--</span>%</div>
                    <div class="field">Pozycja suwaka: <span id="fanSpeedPartyDisplay">50</span>%</div>
                    <input id="fanSpeedPartyControl" type="range" min="0" max="100" step="1" value="50" oninput="updatefanSpeedPartyDisplay()">
                    <button onclick="setFanSpeedParty()">Ustaw prędkość</button>
                    <h3>Czas twania trybu przewietrzanie</h3>
                  <div class="field">Aktualna wartość: <span id="partyModeTimeCurrent">--</span> min</div>
                    <div class="field">Pozycja suwaka: <span id="partyModeTimeDisplay">30</span> min</div>
                    <input id="partyModeTimeControl" type="range" min="5" max="180" step="1" value="30" oninput="updatePartyModeTimeDisplay()">
                    <button onclick="setPartyModeTime()">Ustaw czas</button>
                 </div>
              </div>
                <div class="focus">
                    <h3>Ustawienia zegara</h3>
                    <div>Czas: <span id="czas">--</span></div>
                    <input id="timeInput" type="datetime-local">
                    <button onclick="setTime()">Ustaw czas</button>
                </div>

                <div class="focus">
                  <h3>Komunikaty</h3>
                  <ul id="messagesList"></ul>
                  <button onclick="resetMessages()">Zresetuj</button>
                </div>
                
                
                
           </div>
            <script>
                async function fetchData() {
                  const res = await fetch('/data');
                  const data = await res.json();
                              document.getElementById('tempCzerpnia').textContent = data.tempCzerpnia;
                              document.getElementById('tempWyrzutnia').textContent = data.tempWyrzutnia;
                              document.getElementById('tempWywiew').textContent = data.tempWywiew;
                              document.getElementById('tempNawiew').textContent = data.tempNawiew;
                              document.getElementById('humidityWywiew').textContent = data.humidityWywiew;
                              document.getElementById('fanSpeedWywiew').textContent = data.fanSpeedWywiew;
                              document.getElementById('fanSpeedNawiew').textContent = data.fanSpeedNawiew;
                              document.getElementById('flowRateWywiew').textContent = data.flowRateWywiew;
                              document.getElementById('flowRateNawiew').textContent = data.flowRateNawiew;
                              document.getElementById('servoBypass').textContent = data.servoBypass;
                              document.getElementById('servoRecyrkulacja').textContent = data.servoRecyrkulacja;
                              document.getElementById('czas').textContent = data.czas;
                              document.getElementById('rekuModeAutoButton').textContent = data.rekuModeAuto ? 'Tryb manualny' : 'Tryb automatyczny';
                              document.getElementById('autoFlowControlButton').textContent = data.autoFlowControl ? 'Kontrola przepływu on' : 'Kontrola przepływu off';
                              document.getElementById('fanSpeedPartyCurrent').textContent = data.fanSpeedParty;
                              document.getElementById('tempCzerpniaSetpointCurrent').textContent = data.tempCzerpniaSetpoint; 
                              document.getElementById('seasonSelectDisplay').textContent = ['Auto', 'Lato', 'Zima'][data.seasonSelect];
                              document.getElementById('humiditySetpointWinterCurrent').textContent = data.humiditySetpointWinter;
                              document.getElementById('humiditySetpointSummerCurrent').textContent = data.humiditySetpointSummer;
                              document.getElementById('dayModeSwitchHourCurrent').textContent = data.dayModeSwitchHour;
                              document.getElementById('nightModeSwitchHourCurrent').textContent = data.nightModeSwitchHour;
                              document.getElementById('exhaustFilterFillLevel').textContent = data.exhaustFilterFillLevel;
                              document.getElementById('supplyFilterFillLevel').textContent = data.supplyFilterFillLevel;
                              document.getElementById('partyModeTimeCurrent').textContent = data.partyModeTime;
                              document.getElementById('minSpeedFanCurrent').textContent = data.minSpeedFan;
                              document.getElementById('maxSpeedFanDayCurrent').textContent = data.maxSpeedFanDay;
                              document.getElementById('maxSpeedFanNightCurrent').textContent = data.maxSpeedFanNight;
                            



                }

                function updatefanSpeedPartyDisplay() {
                    const value = document.getElementById('fanSpeedPartyControl').value;
                    document.getElementById('fanSpeedPartyDisplay').textContent = value;
                }

                function updateTempCzerpniaSetpointDisplay() {
                    document.getElementById('tempCzerpniaSetpointDisplay').textContent = document.getElementById('tempCzerpniaSetpointControl').value;
                }

                function updateHumiditySetpointWinterDisplay() {
                    const value = document.getElementById('humiditySetpointWinterControl').value;
                    document.getElementById('humiditySetpointWinterDisplay').textContent = value;
                }

                function updateHumiditySetpointSummerDisplay() {
                    const value = document.getElementById('humiditySetpointSummerControl').value;
                    document.getElementById('humiditySetpointSummerDisplay').textContent = value;
                }

                async function setHumiditySetpointWinter() {
                    const setpoint = document.getElementById('humiditySetpointWinterControl').value;
                    await fetch('/setHumiditySetpointWinter', {
                          method: 'POST',
                          headers: { 'Content-Type': 'application/json' },
                          body: JSON.stringify({ humiditySetpointWinter: setpoint })
                    });
                    alert('Wilgotność zmieniona na ' + setpoint + '%');
                }

                async function setHumiditySetpointSummer() {
                    const setpoint = document.getElementById('humiditySetpointSummerControl').value;
                    await fetch('/setHumiditySetpointSummer', {
                          method: 'POST',
                          headers: { 'Content-Type': 'application/json' },
                          body: JSON.stringify({ humiditySetpointSummer: setpoint })
                    });
                    alert('Wilgotność zmieniona na ' + setpoint + '%');
                }


                async function setFanSpeedParty() {
                    const setpoint = document.getElementById('fanSpeedPartyControl').value;
                    await fetch('/setFanSpeedParty', {
                          method: 'POST',
                          headers: { 'Content-Type': 'application/json' },
                          body: JSON.stringify({ fanSpeedParty: setpoint })
                    });
                    alert('Prędkość zmieniona na ' + setpoint + '%')
                }

                
                function updatePartyModeTimeDisplay() {
                    const value = document.getElementById('partyModeTimeControl').value;
                    document.getElementById('partyModeTimeDisplay').textContent = value;
                }

                function updateMinSpeedFanDisplay() {
                    const value = document.getElementById('minSpeedFanControl').value;
                    document.getElementById('minSpeedFanDisplay').textContent = value;
                }

                function updateMaxSpeedFanDayDisplay() {
                    const value = document.getElementById('maxSpeedFanDayControl').value;
                    document.getElementById('maxSpeedFanDayDisplay').textContent = value;
                }

                function updateMaxSpeedFanNightDisplay() {
                    const value = document.getElementById('maxSpeedFanNightControl').value;
                    document.getElementById('maxSpeedFanNightDisplay').textContent = value;
                }


                async function setPartyModeTime() {
                    const time = document.getElementById('partyModeTimeControl').value;
                    await fetch('/setPartyModeTime', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({ time }),
                    });
                    alert('Manual Mode Time zmieniony na ' + time + ' minutes');
                }

                async function setMinSpeedFan() {
                    const speed = document.getElementById('minSpeedFanControl').value;
                    await fetch('/setMinSpeedFan', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({ speed }),
                    });
                    alert('Min Fan Speed zmieniona na ' + speed + '%');
                }

                async function setMaxSpeedFanDay() {
                    const speed = document.getElementById('maxSpeedFanDayControl').value;
                    await fetch('/setMaxSpeedFanDay', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({ speed }),
                    });
                    alert('Max Fan Speed Day zmieniona na ' + speed + '%');
                }

                async function setMaxSpeedFanNight() {
                    const speed = document.getElementById('maxSpeedFanNightControl').value;
                    await fetch('/setMaxSpeedFanNight', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({ speed }),
                    });
                    alert('Max Fan Speed Night zmieniona na ' + speed + '%');
                }


                async function setTempCzerpniaSetpoint() {
                    const setpoint = document.getElementById('tempCzerpniaSetpointControl').value;
                    await fetch('/setTempCzerpniaSetpoint', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({ tempCzerpniaSetpoint: setpoint }),
                    });
                    alert('Temperatura zmieniona na ' + setpoint + '°C');
                }

                async function setSeasonSelect(seasonSelect) {
                   await fetch('/setSeasonSelect', {
                      method: 'POST',
                      headers: { 'Content-Type': 'application/json' },
                      body: JSON.stringify({ seasonSelect }),
                    });
                    document.getElementById('seasonSelectDisplay').textContent = ['Auto', 'Lato', 'Zima'][seasonSelect];
                    alert('Pora roku ustawiona na ' + ['Auto', 'Lato', 'Zima'][seasonSelect]);
                }

                async function saveToEEPROM() {
                  try {
                      const response = await fetch('/saveToEEPROM', { method: 'POST' });

                      if (response.ok) {
                      alert('Dane zapisane do EEPROM.');
                      } else {
                        const error = await response.text();
                        alert('Błąd podczas zapisu: ' + error);
                       }
                      } catch (error) {
                      alert('Wystąpił błąd: ' + error.message);
                             }
                          }

                async function toggleRekuModeAuto() {
                    await fetch('/toggleRekuModeAuto', {
                        method: 'POST'
                    });
                    alert('Tryb wentylacji zmieniony');
                }

                async function toggleAutoFlowControl() {
                    await fetch('/toggleAutoFlowControl', {
                        method: 'POST'
                    });
                    alert('Tryb kontroli przepływu zmieniony');
                }



                async function setTime() {
                    const timeInput = document.getElementById('timeInput').value;
                    const timestamp = (new Date(timeInput).getTime() / 1000) + 3600;
                    await fetch('/setTime', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({ czas: timestamp }),
                    });
                    alert('Czas zmieniony');
                }

                function updateDayModeSwitchHourDisplay() {
                    const value = document.getElementById('dayModeSwitchHourControl').value;
                    document.getElementById('dayModeSwitchHourDisplay').textContent = value;
                }

                function updateNightModeSwitchHourDisplay() {
                    const value = document.getElementById('nightModeSwitchHourControl').value;
                    document.getElementById('nightModeSwitchHourDisplay').textContent = value;
                }

                async function setDayModeSwitchHour() {
                    const hour = document.getElementById('dayModeSwitchHourControl').value;
                    await fetch('/setDayModeSwitchHour', {
                    method: 'POST',
                      headers: { 'Content-Type': 'application/json' },
                      body: JSON.stringify({ dayModeSwitchHour: hour })
                    });
                    alert('Godzina przejścia na tryb dzienny ustawiona na: ' + hour + ' godzinę');
                }

                async function setNightModeSwitchHour() {
                  const hour = document.getElementById('nightModeSwitchHourControl').value;
                  await fetch('/setNightModeSwitchHour', {
                  method: 'POST',
                  headers: { 'Content-Type': 'application/json' },
                  body: JSON.stringify({ nightModeSwitchHour: hour })
                  });
                    alert('Godzina przejścia na tryb nocny ustawiona na: ' + hour + ' godzinę');
                }

                async function fetchMessages() {
                    const response = await fetch('/getMessages');
                    const data = await response.json();
                    const messagesListElement = document.getElementById('messagesList');
      
                    // Wyczyść aktualną zawartość listy
                    messagesListElement.innerHTML = '';
      
                    if (data.messages && data.messages.length > 0) {
                      // Jeśli są komunikaty, dodaj je do listy
                    data.messages.forEach(message => {
                    const li = document.createElement('li');
                    li.textContent = message;
                      messagesListElement.appendChild(li);
                  });
                    } else {
                    // Jeśli nic nie ma, dodaj komunikat "Brak komunikatów"
                      const li = document.createElement('li');
                        li.textContent = 'Brak komunikatów';
                        messagesListElement.appendChild(li);
                      }
                    }

                async function resetMessages() {
                  await fetch('/resetMessages', { method: 'POST' });
                  alert('Komunikaty zresetowane!');
                  fetchMessages();
                  }

                setInterval(fetchMessages, 5000);
                setInterval(fetchData, 3000);
            </script>
        </body>
        </html>
    )rawliteral");
}

void handleData() {


  StaticJsonDocument<512> json;
  json["tempCzerpnia"] = tempCzerpnia;
  json["tempCzerpniaSetpoint"] = tempCzerpniaSetpoint;
  json["fanSpeedParty"] = fanSpeedParty;
  json["humiditySetpointWinter"] = humiditySetpointWinter;
  json["humiditySetpointSummer"] = humiditySetpointSummer;
  json["tempWyrzutnia"] = tempWyrzutnia;
  json["tempWywiew"] = tempWywiew;
  json["tempNawiew"] = tempNawiew;
  json["humidityWywiew"] = humidityWywiew;
  json["fanSpeedWywiew"] = fanSpeedWywiew;
  json["fanSpeedNawiew"] = fanSpeedNawiew;
  json["flowRateWywiew"] = flowRateWywiew;
  json["flowRateNawiew"] = flowRateNawiew;
  json["servoBypass"] = servoBypass;
  json["servoRecyrkulacja"] = servoRecyrkulacja;
  char timeBuffer[20];
  snprintf(timeBuffer, sizeof(timeBuffer), "%02d-%02d-%04d %02d:%02d:%02d", day(czas), month(czas), year(czas), hour(czas), minute(czas), second(czas));
  json["czas"] = timeBuffer;
  json["rekuModeAuto"] = rekuModeAuto;
  json["autoFlowControl"] = autoFlowControl;
  json["seasonSelect"] = seasonSelect;
  json["dayModeSwitchHour"] = dayModeSwitchHour;
  json["nightModeSwitchHour"] = nightModeSwitchHour;
  json["exhaustFilterFillLevel"] = exhaustFilterFillLevel;
  json["supplyFilterFillLevel"] = supplyFilterFillLevel;
  json["partyModeTime"] = partyModeTime;
  json["minSpeedFan"] = minSpeedFan;
  json["maxSpeedFanDay"] = maxSpeedFanDay;
  json["maxSpeedFanNight"] = maxSpeedFanNight;




  String response;
  serializeJson(json, response);
  server.send(200, "application/json", response);
}


void handleSetHumiditySetpointWinter() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> json;
    deserializeJson(json, server.arg("plain"));
    humiditySetpointWinter = json["humiditySetpointWinter"];
    server.send(200, "application/json", "{\"message\":\"Humidity setpoint updated\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid data\"}");
  }
}

void handleSetHumiditySetpointSummer() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> json;
    deserializeJson(json, server.arg("plain"));
    humiditySetpointSummer = json["humiditySetpointSummer"];
    server.send(200, "application/json", "{\"message\":\"Humidity setpoint updated\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid data\"}");
  }
}

void handleSetFanSpeedParty() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> json;
    deserializeJson(json, server.arg("plain"));
    fanSpeedParty = json["fanSpeedParty"];
    server.send(200, "application/json", "{\"message\":\"Fan speed setpoint updated\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid data\"}");
  }
}

void handleSetPartyModeTime() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> json;
    deserializeJson(json, server.arg("plain"));
    partyModeTime = json["time"];
    server.send(200, "application/json", "{\"message\":\"Manual Mode Time updated\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid data\"}");
  }
}

void handleSetMinSpeedFan() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> json;
    deserializeJson(json, server.arg("plain"));
    minSpeedFan = json["speed"];
    server.send(200, "application/json", "{\"message\":\"Min Fan Speed updated\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid data\"}");
  }
}

void handleSetMaxSpeedFanDay() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> json;
    deserializeJson(json, server.arg("plain"));
    maxSpeedFanDay = json["speed"];
    server.send(200, "application/json", "{\"message\":\"Max Fan Speed Day updated\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid data\"}");
  }
}

void handleSetMaxSpeedFanNight() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> json;
    deserializeJson(json, server.arg("plain"));
    maxSpeedFanNight = json["speed"];
    server.send(200, "application/json", "{\"message\":\"Max Fan Speed Night updated\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid data\"}");
  }
}


void handleSetTempCzerpniaSetpoint() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> json;
    deserializeJson(json, server.arg("plain"));
    tempCzerpniaSetpoint = json["tempCzerpniaSetpoint"];
    server.send(200, "application/json", "{\"message\":\"Temperature setpoint updated\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid data\"}");
  }
}

void handleSetSeasonSelect() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> json;
    deserializeJson(json, server.arg("plain"));
    seasonSelect = json["seasonSelect"];
    server.send(200, "application/json", "{\"message\":\"SeasonSelect updated\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid data\"}");
  }
}

void handleSaveToEEPROM() {
  saveToEEPROM();  // Wywołaj funkcję zapisującą dane
  server.send(200, "text/plain", "Dane zapisane do EEPROM.");
}

void handleToggleRekuModeAuto() {
  rekuModeAuto = !rekuModeAuto;
  server.send(200, "application/json", "{\"message\":\"Tryb wentylacji zmieniony\"}");
}

void handleToggleAutoFlowControl() {
  autoFlowControl = !autoFlowControl;
  server.send(200, "application/json", "{\"message\":\"Tryb kontroli zmieniony\"}");
}

void handleSetDayModeSwitchHour() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> json;
    deserializeJson(json, server.arg("plain"));
    dayModeSwitchHour = json["dayModeSwitchHour"];
    server.send(200, "application/json", "{\"message\":\"Day mode switch hour updated\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid data\"}");
  }
}

void handleSetNightModeSwitchHour() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> json;
    deserializeJson(json, server.arg("plain"));
    nightModeSwitchHour = json["nightModeSwitchHour"];
    server.send(200, "application/json", "{\"message\":\"Night mode switch hour updated\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Invalid data\"}");
  }
}


void handleSetTime() {
  if (server.hasArg("plain")) {
    StaticJsonDocument<200> json;
    DeserializationError error = deserializeJson(json, server.arg("plain"));
    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON format\"}");
      return;
    }

    if (json.containsKey("czas")) {

      time_t newTime = json["czas"];
      if (newTime > 0) {
        setTime(newTime);
        czas = now();
        server.send(200, "application/json", "{\"message\":\"Time updated successfully\"}");
      } else {
        server.send(400, "application/json", "{\"error\":\"Invalid timestamp\"}");
      }
    } else {
      server.send(400, "application/json", "{\"error\":\"Missing 'czas' field\"}");
    }
  } else {
    server.send(400, "application/json", "{\"error\":\"No data provided\"}");
  }
}

void handleGetMessages() {
  StaticJsonDocument<512> json;
  JsonArray messages = json.createNestedArray("messages");
  for (const auto& message : messagesList) {
    messages.add(message);
  }
  String response;
  serializeJson(json, response);
  server.send(200, "application/json", response);
}

void handleResetMessages() {
  resetMessages();
  server.send(200, "application/json", "{\"message\":\"Messages reset successfully\"}");
}

void addMessage(const String& message) {
  if (std::find(messagesList.begin(), messagesList.end(), message) == messagesList.end()) {
    messagesList.push_back(message);
  }
}

void resetMessages() {
  messagesList.clear();
}

void saveToEEPROM() {
  EEPROM.begin(512);
  EEPROM.put(0, tempCzerpniaSetpoint);
  EEPROM.put(4, seasonSelect);
  EEPROM.put(8, dayModeSwitchHour);
  EEPROM.put(12, nightModeSwitchHour);
  EEPROM.put(16, humiditySetpointWinter);
  EEPROM.commit();
  EEPROM.end();
}

void loadFromEEPROM() {
  EEPROM.begin(512);
  EEPROM.get(0, tempCzerpniaSetpoint);
  EEPROM.get(4, seasonSelect);
  EEPROM.get(8, dayModeSwitchHour);
  EEPROM.get(12, nightModeSwitchHour);
  EEPROM.get(16, humiditySetpointWinter);
  EEPROM.end();
}

void setupRecuWebserver() {
  
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/setHumiditySetpointWinter", HTTP_POST, handleSetHumiditySetpointWinter);
  server.on("/setHumiditySetpointSummer", HTTP_POST, handleSetHumiditySetpointSummer);
  server.on("/setFanSpeedParty", HTTP_POST, handleSetFanSpeedParty);
  server.on("/setTempCzerpniaSetpoint", HTTP_POST, handleSetTempCzerpniaSetpoint);
  server.on("/setSeasonSelect", HTTP_POST, handleSetSeasonSelect);
  server.on("/saveToEEPROM", HTTP_POST, handleSaveToEEPROM);
  server.on("/toggleRekuModeAuto", HTTP_POST, handleToggleRekuModeAuto);
  server.on("/toggleAutoFlowControl", HTTP_POST, handleToggleAutoFlowControl);
  server.on("/setTime", HTTP_POST, handleSetTime);
  server.on("/setDayModeSwitchHour", HTTP_POST, handleSetDayModeSwitchHour);
  server.on("/setNightModeSwitchHour", HTTP_POST, handleSetNightModeSwitchHour);
  server.on("/getMessages", HTTP_GET, handleGetMessages);
  server.on("/resetMessages", HTTP_POST, handleResetMessages);
  server.on("/setPartyModeTime", HTTP_POST, handleSetPartyModeTime);
  server.on("/setMinSpeedFan", HTTP_POST, handleSetMinSpeedFan);
  server.on("/setMaxSpeedFanDay", HTTP_POST, handleSetMaxSpeedFanDay);
  server.on("/setMaxSpeedFanNight", HTTP_POST, handleSetMaxSpeedFanNight);

  server.begin();
  Serial.println("HTTP server started");
  loadFromEEPROM(); //Załaduj setpointy z eeprom
}
