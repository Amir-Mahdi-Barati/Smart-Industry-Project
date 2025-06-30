// maintained by AmirMahdi Barati
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22
#define MQ135PIN 34

#define RELAY_COUNT 8
const int relayPins[RELAY_COUNT] = {12, 13, 14, 15, 16, 17, 18, 19};
#define WATER_RELAY_PIN 21

AsyncWebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

bool relayState[RELAY_COUNT] = {false};
unsigned long relayOnDelay[RELAY_COUNT] = {0};
unsigned long relayOffDelay[RELAY_COUNT] = {0};
unsigned long relayOnStart[RELAY_COUNT] = {0};
unsigned long relayOffStart[RELAY_COUNT] = {0};
bool relayOnTimerActive[RELAY_COUNT] = {false};
bool relayOffTimerActive[RELAY_COUNT] = {false};

bool waterRelayState = false;

const char* ssid = "Control-Panel";
const char* password = "";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fa" dir="rtl">
<head>
<meta charset="UTF-8" />
<meta name="viewport" content="width=device-width, initial-scale=1" />
<title><پنل کنترل صنعت هوشمند /Intelligent Industrial Control Panel</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Vazirmatn&display=swap');
  body {
    font-family: 'Vazirmatn', sans-serif;
    margin: 0; padding: 20px;
    background: linear-gradient(135deg, #ffffffcc, #f0f0f0cc);
    color: #111;
    min-height: 100vh;
    display: flex;
    flex-direction: column;
    align-items: center;
  }
  .glass-container {
    background: rgba(255, 255, 255, 0.55);
    border-radius: 16px;
    padding: 30px 25px;
    max-width: 1100px;
    width: 100%;
    box-shadow: 0 8px 32px 0 rgba(31, 38, 135, 0.15);
    backdrop-filter: blur(8px);
    -webkit-backdrop-filter: blur(8px);
    border: 1px solid rgba(255, 255, 255, 0.25);
    color: #111;
  }
  h1 {
    text-align: center;
    margin-bottom: 30px;
    font-weight: 900;
    font-size: 30px;
    user-select: none;
  }
  .lang-switcher {
    position: fixed;
    top: 15px;
    right: 15px;
    background: #fff9;
    border: 1.5px solid #333;
    border-radius: 30px;
    cursor: pointer;
    padding: 8px 15px;
    font-weight: 700;
    color: #111;
    user-select: none;
    transition: background-color 0.3s ease;
    z-index: 999;
  }
  .lang-switcher:hover {
    background: #eee;
  }

  .relay-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit,minmax(280px,1fr));
    gap: 20px;
    margin-bottom: 50px;
  }
  .relay-card {
    background: rgba(255, 255, 255, 0.7);
    border-radius: 14px;
    padding: 20px;
    box-shadow: 0 6px 15px rgba(0,0,0,0.07);
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    transition: transform 0.2s ease;
  }
  .relay-card:hover {
    transform: translateY(-4px);
    box-shadow: 0 10px 20px rgba(0,0,0,0.1);
  }
  .relay-title {
    font-weight: 700;
    font-size: 22px;
    margin-bottom: 18px;
    color: #222;
    text-align: center;
  }
  .btn {
    padding: 14px;
    font-size: 20px;
    border: none;
    border-radius: 10px;
    cursor: pointer;
    font-weight: 700;
    margin-bottom: 15px;
    transition: all 0.3s ease;
    user-select: none;
    width: 48%;
    display: inline-block;
    text-align: center;
  }
  .btn-on {
    background-color: #4caf50;
    color: #fff;
    box-shadow: 0 5px 12px #4caf5077;
  }
  .btn-on:hover {
    background-color: #388e3c;
    box-shadow: 0 7px 15px #388e3caa;
    transform: scale(1.05);
  }
  .btn-on:active {
    transform: scale(0.95);
    box-shadow: 0 4px 8px #2e7d3211;
  }
  .btn-off {
    background-color: #ddd;
    color: #111;
    box-shadow: 0 4px 8px #bbb;
  }
  .btn-off:hover {
    background-color: #bbb;
    box-shadow: 0 6px 12px #999;
    transform: scale(1.05);
  }
  .btn-off:active {
    transform: scale(0.95);
    box-shadow: 0 3px 6px #777;
  }
  label {
    font-size: 15px;
    font-weight: 700;
    margin-bottom: 8px;
    display: block;
    color: #222;
  }
  input[type=number] {
    width: 100%;
    padding: 9px 14px;
    border: 2px solid #4caf50;
    border-radius: 8px;
    font-size: 18px;
    color: #222;
    margin-bottom: 18px;
    box-sizing: border-box;
    background: #f8fff8cc;
    transition: border-color 0.3s ease;
  }
  input[type=number]:focus {
    border-color: #388e3c;
    outline: none;
    background: #e6f4e6cc;
  }
  .timer-group {
    margin-top: auto;
  }
  /* سنسورها */
  .sensor-container {
    background: rgba(255,255,255,0.7);
    border-radius: 14px;
    box-shadow: 0 6px 20px rgba(0,0,0,0.07);
    padding: 25px 20px;
    margin-bottom: 40px;
    user-select: none;
  }
  .sensor-title {
    text-align: center;
    font-weight: 900;
    font-size: 26px;
    margin-bottom: 20px;
    color: #111;
  }
  .sensor-item {
    display: flex;
    justify-content: space-around;
    flex-wrap: wrap;
    gap: 30px;
  }
  .sensor-box {
    background: #f5f5f5cc;
    border-radius: 12px;
    box-shadow: inset 0 0 15px #4caf5033;
    width: 250px;
    padding: 20px;
    text-align: center;
    color: #111;
  }
  .sensor-label {
    font-weight: 700;
    font-size: 20px;
    color: #222;
    margin-bottom: 10px;
  }
  .sensor-value {
    font-weight: 900;
    font-size: 36px;
    color: #4caf50;
    margin-bottom: 8px;
    font-family: monospace;
  }
  svg {
    width: 120px;
    height: 120px;
    margin: auto;
    display: block;
  }
  .circle-bg {
    fill: none;
    stroke: #99999955;
    stroke-width: 12;
  }
  .circle-progress {
    fill: none;
    stroke: #4caf50;
    stroke-width: 12;
    stroke-linecap: round;
    transition: stroke-dashoffset 0.5s ease;
  }

  /* ریسپانسیو */
  @media (max-width: 650px) {
    .relay-grid {
      grid-template-columns: 1fr !important;
    }
    .sensor-item {
      justify-content: center;
    }
    .btn {
      width: 100% !important;
      margin-bottom: 10px;
    }
  }

  /* زبان */
  .lang-en {
    display: none;
    font-family: Arial, sans-serif;
  }
  .lang-fa {
    font-family: 'Vazirmatn', sans-serif;
  }
  .lang-active {
    display: block !important;
  }
</style>
</head>
<body>
  <button class="lang-switcher" id="langSwitcher">English / فارسی</button>
  <div class="glass-container">
    <h1>
      <span class="lang-fa lang-active">پنل کنترل صنعت هوشمند </span>
      <span class="lang-en">Intelligent Industrial Control Panel</span>

    </h1>
    <div class="sensor-container">
      <div class="sensor-title">
        <span class="lang-fa lang-active">وضعیت سنسورها</span>
        <span class="lang-en">Sensor Status</span>
      </div>
      <div class="sensor-item">
        <div class="sensor-box">
          <div class="sensor-label">
            <span class="lang-fa lang-active">دما (°C)</span>
            <span class="lang-en">Temperature (°C)</span>
          </div>
          <div id="tempVal" class="sensor-value">-</div>
          <svg viewBox="0 0 120 120">
            <circle class="circle-bg" cx="60" cy="60" r="50"></circle>
            <circle id="tempCircle" class="circle-progress" cx="60" cy="60" r="50"
              stroke-dasharray="314" stroke-dashoffset="314"></circle>
          </svg>
        </div>
        <div class="sensor-box">
          <div class="sensor-label">
            <span class="lang-fa lang-active">رطوبت (%)</span>
            <span class="lang-en">Humidity (%)</span>
          </div>
          <div id="humVal" class="sensor-value">-</div>
          <svg viewBox="0 0 120 120">
            <circle class="circle-bg" cx="60" cy="60" r="50"></circle>
            <circle id="humCircle" class="circle-progress" cx="60" cy="60" r="50"
              stroke-dasharray="314" stroke-dashoffset="314"></circle>
          </svg>
        </div>
        <div class="sensor-box">
          <div class="sensor-label">
            <span class="lang-fa lang-active">کیفیت هوا (ppm)</span>
            <span class="lang-en">Air Quality (ppm)</span>
          </div>
          <div id="airVal" class="sensor-value">-</div>
        </div>
      </div>
    </div>
    <div class="relay-grid">
      %RELAY_CARDS%
    </div>
  </div>

<script>
  const relayCount = %RELAY_COUNT%;

  // تغییر زبان
  const langSwitcher = document.getElementById('langSwitcher');
  langSwitcher.onclick = () => {
    const faEls = document.querySelectorAll('.lang-fa');
    const enEls = document.querySelectorAll('.lang-en');
    if (faEls[0].classList.contains('lang-active')) {
      // نمایش انگلیسی
      faEls.forEach(e => e.classList.remove('lang-active'));
      enEls.forEach(e => e.classList.add('lang-active'));
      document.documentElement.dir = 'ltr';
      langSwitcher.innerText = "فارسی / English";
    } else {
      // نمایش فارسی
      enEls.forEach(e => e.classList.remove('lang-active'));
      faEls.forEach(e => e.classList.add('lang-active'));
      document.documentElement.dir = 'rtl';
      langSwitcher.innerText = "English / فارسی";
    }
  };

  function toggleRelay(num, state) {
    fetch(`/relay?num=${num}&state=${state}`).then(res => res.json()).then(data => {
      if(data.success){
        updateRelayUI(num, state);
      }
    });
  }

  function updateRelayUI(num, state){
    const onBtn = document.getElementById(`onBtn${num}`);
    const offBtn = document.getElementById(`offBtn${num}`);
    if(state == 1){
      onBtn.style.backgroundColor = '#4caf50';
      onBtn.style.color = '#fff';
      offBtn.style.backgroundColor = '#ddd';
      offBtn.style.color = '#111';
    } else {
      offBtn.style.backgroundColor = '#4caf50';
      offBtn.style.color = '#fff';
      onBtn.style.backgroundColor = '#ddd';
      onBtn.style.color = '#111';
    }
  }

  function setTimer(num){
    let onDelay = document.getElementById(`onDelay${num}`).value || 0;
    let offDelay = document.getElementById(`offDelay${num}`).value || 0;
    fetch(`/timer?num=${num}&onDelay=${onDelay}&offDelay=${offDelay}`).then(res => res.json()).then(data => {
      if(data.success){
        alert(
          (document.documentElement.dir === 'rtl' ? 
          `تایمر رله ${num+1} تنظیم شد: روشن شدن بعد ${onDelay} ثانیه، خاموش شدن بعد ${offDelay} ثانیه` : 
          `Relay ${num+1} timer set: ON delay ${onDelay} sec, OFF delay ${offDelay} sec`)
        );
      } else {
        alert(document.documentElement.dir === 'rtl' ? "خطا در تنظیم تایمر" : "Failed to set timer");
      }
    });
  }

  function fetchSensorData(){
    fetch('/sensor').then(res=>res.json()).then(data=>{
      document.getElementById('tempVal').innerText = data.temperature.toFixed(1);
      document.getElementById('humVal').innerText = data.humidity.toFixed(1);
      document.getElementById('airVal').innerText = data.airQuality.toFixed(0);

      // دایره‌های پیشرفت
      let tempPercent = Math.min(Math.max((data.temperature / 50) * 314, 0), 314);
      let humPercent = Math.min(Math.max((data.humidity / 100) * 314, 0), 314);
      document.getElementById('tempCircle').style.strokeDashoffset = 314 - tempPercent;
      document.getElementById('humCircle').style.strokeDashoffset = 314 - humPercent;
    });
  }

  setInterval(fetchSensorData, 2000);
  
</script>
</body>
</html>
)rawliteral";

String relayCardsHTML() {
  String html = "";
  for(int i = 0; i < RELAY_COUNT; i++){
    html += "<div class=\"relay-card\">";
    html += "<div class=\"relay-title\">";
    html += "<span class=\"lang-fa lang-active\">رله شماره " + String(i+1) + "</span>";
    html += "<span class=\"lang-en\">Relay #" + String(i+1) + "</span>";
    html += "</div>";
    html += "<button id=\"onBtn" + String(i) + "\" class=\"btn btn-on\" onclick=\"toggleRelay(" + String(i) + ",1)\">";
    html += "<span class=\"lang-fa lang-active\">روشن</span><span class=\"lang-en\">ON</span></button>";
    html += "<button id=\"offBtn" + String(i) + "\" class=\"btn btn-off\" onclick=\"toggleRelay(" + String(i) + ",0)\">";
    html += "<span class=\"lang-fa lang-active\">خاموش</span><span class=\"lang-en\">OFF</span></button>";
    html += "<div class=\"timer-group\">";
    html += "<label for=\"onDelay" + String(i) + "\"><span class=\"lang-fa lang-active\">تأخیر روشن شدن (ثانیه):</span><span class=\"lang-en\">ON Delay (sec):</span></label>";
    html += "<input type=\"number\" id=\"onDelay" + String(i) + "\" min=\"0\" step=\"1\" value=\"0\">";
    html += "<label for=\"offDelay" + String(i) + "\"><span class=\"lang-fa lang-active\">تأخیر خاموش شدن (ثانیه):</span><span class=\"lang-en\">OFF Delay (sec):</span></label>";
    html += "<input type=\"number\" id=\"offDelay" + String(i) + "\" min=\"0\" step=\"1\" value=\"0\">";
    html += "<button class=\"btn btn-on\" style=\"width:100%; margin-top:8px;\" onclick=\"setTimer(" + String(i) + ")\">";
    html += "<span class=\"lang-fa lang-active\">تنظیم تایمر</span><span class=\"lang-en\">Set Timer</span></button>";
    html += "</div>";
    html += "</div>";
  }
  return html;
}

void setup(){
  Serial.begin(115200);
  dht.begin();

  for(int i=0; i < RELAY_COUNT; i++){
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);
  }
  pinMode(WATER_RELAY_PIN, OUTPUT);
  digitalWrite(WATER_RELAY_PIN, LOW);

  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String page = index_html;
    page.replace("%RELAY_CARDS%", relayCardsHTML());
    page.replace("%RELAY_COUNT%", String(RELAY_COUNT));
    request->send(200, "text/html", page);
  });

  server.on("/relay", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("num") && request->hasParam("state")){
      int num = request->getParam("num")->value().toInt();
      int state = request->getParam("state")->value().toInt();
      if(num >= 0 && num < RELAY_COUNT){
        if(state == 1){
          if(relayOnDelay[num] > 0){
            relayOnTimerActive[num] = true;
            relayOnStart[num] = millis();
            relayOffTimerActive[num] = false;
          } else {
            relayState[num] = true;
            digitalWrite(relayPins[num], HIGH);
            relayOnTimerActive[num] = false;
            relayOffTimerActive[num] = false;
          }
        } else {
          if(relayOffDelay[num] > 0){
            relayOffTimerActive[num] = true;
            relayOffStart[num] = millis();
            relayOnTimerActive[num] = false;
          } else {
            relayState[num] = false;
            digitalWrite(relayPins[num], LOW);
            relayOnTimerActive[num] = false;
            relayOffTimerActive[num] = false;
          }
        }
        request->send(200, "application/json", "{\"success\":true}");
        return;
      }
    }
    request->send(400, "application/json", "{\"success\":false}");
  });

  server.on("/timer", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("num") && request->hasParam("onDelay") && request->hasParam("offDelay")){
      int num = request->getParam("num")->value().toInt();
      int onDelaySec = request->getParam("onDelay")->value().toInt();
      int offDelaySec = request->getParam("offDelay")->value().toInt();

      if(num >= 0 && num < RELAY_COUNT){
        relayOnDelay[num] = onDelaySec * 1000UL;
        relayOffDelay[num] = offDelaySec * 1000UL;

        relayOnTimerActive[num] = false;
        relayOffTimerActive[num] = false;

        request->send(200, "application/json", "{\"success\":true}");
        return;
      }
    }
    request->send(400, "application/json", "{\"success\":false}");
  });

  server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request){
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int airQuality = analogRead(MQ135PIN);
    if(isnan(temperature)) temperature = 0;
    if(isnan(humidity)) humidity = 0;

    String json = "{";
    json += "\"temperature\":" + String(temperature, 1) + ",";
    json += "\"humidity\":" + String(humidity, 1) + ",";
    json += "\"airQuality\":" + String(airQuality);
    json += "}";

    request->send(200, "application/json", json);
  });

  WiFi.softAP(ssid, password);
  server.begin();
}

void loop(){
  unsigned long now = millis();

  for(int i=0; i < RELAY_COUNT; i++){
    // تایمر روشن شدن رله
    if(relayOnTimerActive[i]){
      if(now - relayOnStart[i] >= relayOnDelay[i]){
        relayState[i] = true;
        digitalWrite(relayPins[i], HIGH);
        relayOnTimerActive[i] = false;
      }
    }
    // تایمر خاموش شدن رله
    if(relayOffTimerActive[i]){
      if(now - relayOffStart[i] >= relayOffDelay[i]){
        relayState[i] = false;
        digitalWrite(relayPins[i], LOW);
        relayOffTimerActive[i] = false;
      }
    }
  }
}
