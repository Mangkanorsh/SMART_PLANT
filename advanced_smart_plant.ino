#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <time.h>

// ==================== OLED Display ====================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ==================== Pin Definitions ====================
#define DHTPIN 4
#define DHTTYPE DHT22
#define SOIL_MOISTURE_PIN 32
#define LIGHT_SENSOR_PIN 34
#define RELAY_PIN 23

// ==================== Calibration ====================
const int SOIL_DRY = 4095;
const int SOIL_WET = 1500;
const int SOIL_THRESHOLD = 50;
const int TEMP_THRESHOLD_HIGH = 35;  // Alert if above 35°C
const int TEMP_THRESHOLD_LOW = 10;   // Alert if below 10°C

// ==================== Configuration ====================
// WiFi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Telegram Bot
#define BOT_TOKEN "YOUR_TELEGRAM_BOT_TOKEN"
#define CHAT_ID "YOUR_TELEGRAM_CHAT_ID"
const unsigned long BOT_MTBS = 1000; // Mean time between scan messages
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Google Sheets - Script Deployment
const char* GOOGLE_SCRIPT_ID = "YOUR_GOOGLE_SCRIPT_ID";
const char* GOOGLE_SCRIPT_URL = "https://script.google.com/macros/s/YOUR_SCRIPT_ID/exec";

// Web Server
AsyncWebServer server(80);
const char* PARAM_MESSAGE = "message";

// ==================== Global Variables ====================
DHT dht(DHTPIN, DHTTYPE);

// Sensor readings
int soilMoistureValue = 0;
int soilMoisturePercent = 0;
int lightValue = 0;
float temperature = 0;
float humidity = 0;
bool watering = false;
bool manualMode = false;
String lastWatered = "Never";
String systemStatus = "Normal";
unsigned long waterStartTime = 0; 
unsigned long WATER_DURATION = 5000;
unsigned long lastBotScan = 0;
unsigned long lastDataSend = 0;
const unsigned long DATA_SEND_INTERVAL = 300000; // 5 minutes
int lastSentSoilPercent = -1;
float lastSentTemp = -100;

// HTML Web Page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>Smart Plant System</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { 
            font-family: Arial, sans-serif; 
            text-align: center; 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            margin: 0;
            padding: 20px;
            color: white;
        }
        .container { 
            max-width: 800px; 
            margin: 0 auto; 
            background: rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 30px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.1);
        }
        h1 { 
            color: white; 
            margin-bottom: 30px;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        .grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin: 30px 0;
        }
        .card {
            background: rgba(255, 255, 255, 0.15);
            border-radius: 15px;
            padding: 20px;
            transition: transform 0.3s, box-shadow 0.3s;
        }
        .card:hover {
            transform: translateY(-5px);
            box-shadow: 0 10px 20px rgba(0,0,0,0.2);
        }
        .sensor-value {
            font-size: 2.5em;
            font-weight: bold;
            margin: 10px 0;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        .unit {
            font-size: 0.8em;
            color: #d1d5db;
        }
        .status-indicator {
            display: inline-block;
            width: 12px;
            height: 12px;
            border-radius: 50%;
            margin-right: 8px;
        }
        .status-normal { background-color: #10B981; }
        .status-warning { background-color: #F59E0B; }
        .status-alert { background-color: #EF4444; }
        .btn {
            background: rgba(255, 255, 255, 0.2);
            border: 2px solid rgba(255, 255, 255, 0.3);
            color: white;
            padding: 12px 24px;
            border-radius: 25px;
            cursor: pointer;
            font-size: 1em;
            margin: 10px;
            transition: all 0.3s;
            width: 200px;
        }
        .btn:hover {
            background: rgba(255, 255, 255, 0.3);
            transform: scale(1.05);
        }
        .btn-water {
            background: linear-gradient(135deg, #3B82F6, #1D4ED8);
            border: none;
        }
        .btn-stop {
            background: linear-gradient(135deg, #EF4444, #DC2626);
            border: none;
        }
        .mode-btn {
            background: rgba(255, 255, 255, 0.1);
            border: 1px solid rgba(255, 255, 255, 0.3);
        }
        .mode-btn.active {
            background: rgba(59, 130, 246, 0.5);
            border-color: #3B82F6;
        }
        .history {
            margin-top: 30px;
            text-align: left;
        }
        .history-item {
            background: rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 10px;
            margin: 5px 0;
        }
    </style>
    <script>
        function updateSensorData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    // Update sensor values
                    document.getElementById('temp').innerHTML = data.temperature.toFixed(1);
                    document.getElementById('hum').innerHTML = data.humidity.toFixed(1);
                    document.getElementById('soil').innerHTML = data.soilMoisture;
                    document.getElementById('light').innerHTML = data.light;
                    document.getElementById('status').innerHTML = data.systemStatus;
                    document.getElementById('lastWatered').innerHTML = data.lastWatered;
                    
                    // Update watering status
                    document.getElementById('wateringStatus').innerHTML = 
                        data.watering ? 'ACTIVE' : 'INACTIVE';
                    document.getElementById('wateringStatus').style.color = 
                        data.watering ? '#10B981' : '#EF4444';
                    
                    // Update mode buttons
                    document.getElementById('autoBtn').className = 
                        data.manualMode ? 'btn mode-btn' : 'btn mode-btn active';
                    document.getElementById('manualBtn').className = 
                        data.manualMode ? 'btn mode-btn active' : 'btn mode-btn';
                    
                    // Update status indicator
                    let indicator = document.getElementById('statusIndicator');
                    indicator.className = 'status-indicator ';
                    if (data.systemStatus.includes('Alert')) {
                        indicator.className += 'status-alert';
                    } else if (data.systemStatus.includes('Warning')) {
                        indicator.className += 'status-warning';
                    } else {
                        indicator.className += 'status-normal';
                    }
                })
                .catch(error => console.error('Error:', error));
        }
        
        function controlPump(action) {
            fetch(`/control?action=${action}`)
                .then(response => response.text())
                .then(data => {
                    console.log('Control response:', data);
                    updateSensorData();
                });
        }
        
        function setMode(mode) {
            fetch(`/mode?mode=${mode}`)
                .then(response => response.text())
                .then(data => {
                    console.log('Mode response:', data);
                    updateSensorData();
                });
        }
        
        // Update data every 3 seconds
        setInterval(updateSensorData, 3000);
        
        // Initial load
        window.onload = updateSensorData;
    </script>
</head>
<body>
    <div class="container">
        <h1>🌱 Smart Plant System</h1>
        
        <div class="grid">
            <!-- Temperature Card -->
            <div class="card">
                <h3>🌡️ Temperature</h3>
                <div class="sensor-value">
                    <span id="temp">--</span><span class="unit">°C</span>
                </div>
            </div>
            
            <!-- Humidity Card -->
            <div class="card">
                <h3>💧 Humidity</h3>
                <div class="sensor-value">
                    <span id="hum">--</span><span class="unit">%</span>
                </div>
            </div>
            
            <!-- Soil Moisture Card -->
            <div class="card">
                <h3>🌱 Soil Moisture</h3>
                <div class="sensor-value">
                    <span id="soil">--</span><span class="unit">%</span>
                </div>
            </div>
            
            <!-- Light Card -->
            <div class="card">
                <h3>☀️ Light Level</h3>
                <div class="sensor-value">
                    <span id="light">--</span>
                </div>
            </div>
        </div>
        
        <!-- System Status -->
        <div class="card" style="grid-column: span 2;">
            <h3>System Status</h3>
            <p>
                <span id="statusIndicator" class="status-indicator status-normal"></span>
                Status: <span id="status" style="font-weight: bold;">Normal</span>
            </p>
            <p>Water Pump: <span id="wateringStatus" style="font-weight: bold;">INACTIVE</span></p>
            <p>Last Watered: <span id="lastWatered" style="font-weight: bold;">Never</span></p>
        </div>
        
        <!-- Control Panel -->
        <div class="card" style="grid-column: span 2;">
            <h3>Control Panel</h3>
            
            <div style="margin: 20px 0;">
                <button id="autoBtn" class="btn mode-btn active" onclick="setMode('auto')">Auto Mode</button>
                <button id="manualBtn" class="btn mode-btn" onclick="setMode('manual')">Manual Mode</button>
            </div>
            
            <div>
                <button class="btn btn-water" onclick="controlPump('start')" 
                        style="background: linear-gradient(135deg, #10B981, #047857);">
                    💧 Start Watering
                </button>
                <button class="btn btn-stop" onclick="controlPump('stop')">
                    ⏹️ Stop Watering
                </button>
            </div>
        </div>
        
        <!-- Quick Actions -->
        <div class="card">
            <h3>📱 Quick Actions</h3>
            <button class="btn" onclick="controlPump('water5')">Water for 5s</button>
            <button class="btn" onclick="controlPump('water10')">Water for 10s</button>
            <button class="btn" onclick="controlPump('update')">Force Update</button>
        </div>
        
        <!-- Telegram Commands -->
        <div class="card">
            <h3>🤖 Telegram Commands</h3>
            <p style="text-align: left; font-size: 0.9em;">
                • /status - Get current readings<br>
                • /water - Manual watering<br>
                • /mode auto|manual - Change mode<br>
                • /settings - View thresholds<br>
                • /help - Show all commands
            </p>
        </div>
    </div>
</body>
</html>
)rawliteral";

// ==================== Setup Function ====================
void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Relay OFF initially
  
  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  display.display();
  delay(1000);
  
  // Initialize DHT
  dht.begin();
  
  // Connect to WiFi
  connectToWiFi();
  
  // Initialize Telegram Bot
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  
  // Initialize Web Server
  setupWebServer();
  
  // Sync time for Telegram
  configTime(0, 0, "pool.ntp.org");
  
  // Send startup message
  sendTelegramMessage("🚀 Smart Plant System Started!\n" + getSystemInfo());
}

// ==================== Main Loop ====================
void loop() {
  // Read sensors
  readSensors();
  
  // Check for Telegram messages
  handleTelegramMessages();
  
  // Auto watering logic
  if (!manualMode && !watering) {
    if (soilMoisturePercent < SOIL_THRESHOLD) {
      startWatering("Auto");
    }
  }
  
  // Stop watering if duration exceeded
  if (watering && (millis() - waterStartTime > WATER_DURATION)) {
    stopWatering();
  }
  
  // Check temperature alerts
  checkTemperatureAlerts();
  
  // Update display
  updateDisplay();
  
  // Send data to Google Sheets periodically
  if (millis() - lastDataSend > DATA_SEND_INTERVAL) {
    if (WiFi.status() == WL_CONNECTED) {
      sendToGoogleSheets();
      lastDataSend = millis();
    }
  }
  
  delay(1000);
}

// ==================== Sensor Functions ====================
void readSensors() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
  soilMoisturePercent = map(soilMoistureValue, SOIL_DRY, SOIL_WET, 0, 100);
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
  
  lightValue = analogRead(LIGHT_SENSOR_PIN);
}

void checkTemperatureAlerts() {
  static bool tempAlertSent = false;
  
  if (temperature > TEMP_THRESHOLD_HIGH && !tempAlertSent) {
    String message = "🔥 HIGH TEMPERATURE ALERT!\n";
    message += "Temperature: " + String(temperature, 1) + "°C\n";
    message += "Threshold: " + String(TEMP_THRESHOLD_HIGH) + "°C";
    sendTelegramMessage(message);
    systemStatus = "Alert: High Temp";
    tempAlertSent = true;
  } else if (temperature <= TEMP_THRESHOLD_HIGH - 2) {
    tempAlertSent = false;
    systemStatus = "Normal";
  }
}

// ==================== Watering Control ====================
void startWatering(String source) {
  watering = true;
  waterStartTime = millis();
  digitalWrite(RELAY_PIN, LOW); // Relay ON
  
  // Update last watered time
  struct tm timeinfo;
  if(getLocalTime(&timeinfo)){
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    lastWatered = String(timeStr);
  }
  
  // Send notification
  String message = "💧 Watering Started!\n";
  message += "Source: " + source + "\n";
  message += "Soil Moisture: " + String(soilMoisturePercent) + "%";
  sendTelegramMessage(message);
  
  Serial.println("Watering started from: " + source);
}

void stopWatering() {
  if (watering) {
    watering = false;
    digitalWrite(RELAY_PIN, HIGH); // Relay OFF
    
    String message = "✅ Watering Completed!\n";
    message += "Duration: " + String((millis() - waterStartTime) / 1000) + "s\n";
    message += "Current Soil: " + String(soilMoisturePercent) + "%";
    sendTelegramMessage(message);
    
    Serial.println("Watering stopped");
  }
}

// ==================== Display Functions ====================
void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(temperature, 1);
  display.println(" C");
  
  display.print("Hum:  ");
  display.print(humidity, 1);
  display.println(" %");
  
  display.print("Soil: ");
  display.print(soilMoisturePercent);
  display.println(" %");
  
  display.print("Light:");
  display.println(lightValue);
  
  display.setCursor(0, 40);
  display.print("Mode: ");
  display.println(manualMode ? "Manual" : "Auto");
  
  display.print("Pump: ");
  display.println(watering ? "ON " : "OFF");
  
  if (watering) {
    int elapsed = (millis() - waterStartTime) / 1000;
    display.print("Time: ");
    display.print(elapsed);
    display.print("/");
    display.println(WATER_DURATION / 1000);
  }
  
  display.display();
}

// ==================== WiFi Functions ====================
void connectToWiFi() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connecting to WiFi");
  display.display();
  
  WiFi.begin(ssid, password);
  int attempts = 0;
  
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi Connected!");
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.display();
    delay(2000);
  } else {
    Serial.println("\nWiFi connection failed!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi Failed!");
    display.display();
  }
}

// ==================== Web Server Functions ====================
void setupWebServer() {
  // Serve HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  
  // API endpoint for sensor data
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"temperature\":" + String(temperature, 1) + ",";
    json += "\"humidity\":" + String(humidity, 1) + ",";
    json += "\"soilMoisture\":" + String(soilMoisturePercent) + ",";
    json += "\"light\":" + String(lightValue) + ",";
    json += "\"watering\":" + String(watering ? "true" : "false") + ",";
    json += "\"manualMode\":" + String(manualMode ? "true" : "false") + ",";
    json += "\"systemStatus\":\"" + systemStatus + "\",";
    json += "\"lastWatered\":\"" + lastWatered + "\"";
    json += "}";
    
    request->send(200, "application/json", json);
  });
  
  // Control endpoint
  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request){
    String action;
    if (request->hasParam("action")) {
      action = request->getParam("action")->value();
      
      if (action == "start") {
        startWatering("Web Manual");
        request->send(200, "text/plain", "Watering started");
      } else if (action == "stop") {
        stopWatering();
        request->send(200, "text/plain", "Watering stopped");
      } else if (action == "water5") {
        WATER_DURATION = 5000;
        startWatering("Web Timer");
        request->send(200, "text/plain", "Watering for 5 seconds");
      } else if (action == "water10") {
        WATER_DURATION = 10000;
        startWatering("Web Timer");
        request->send(200, "text/plain", "Watering for 10 seconds");
      } else if (action == "update") {
        readSensors();
        request->send(200, "text/plain", "Sensors updated");
      }
    }
  });
  
  // Mode endpoint
  server.on("/mode", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("mode")) {
      String mode = request->getParam("mode")->value();
      manualMode = (mode == "manual");
      String response = manualMode ? "Manual mode activated" : "Auto mode activated";
      request->send(200, "text/plain", response);
      
      sendTelegramMessage("🔧 Mode changed to: " + mode);
    }
  });
  
  server.begin();
  Serial.println("HTTP server started");
}

// ==================== Telegram Functions ====================
void handleTelegramMessages() {
  if (millis() - lastBotScan > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    
    while (numNewMessages) {
      Serial.println("Got Telegram message");
      
      for (int i = 0; i < numNewMessages; i++) {
        String chat_id = bot.messages[i].chat_id;
        String text = bot.messages[i].text;
        String from_name = bot.messages[i].from_name;
        
        if (chat_id != CHAT_ID) {
          bot.sendMessage(chat_id, "Unauthorized user", "");
          continue;
        }
        
        if (text == "/start" || text == "/help") {
          String welcome = "Welcome " + from_name + "!\n";
          welcome += "Available commands:\n";
          welcome += "/status - Get current readings\n";
          welcome += "/water - Manual watering\n";
          welcome += "/stop - Stop watering\n";
          welcome += "/mode auto - Switch to auto mode\n";
          welcome += "/mode manual - Switch to manual mode\n";
          welcome += "/settings - View system settings\n";
          welcome += "/help - Show this message";
          bot.sendMessage(chat_id, welcome, "");
        }
        
        else if (text == "/status") {
          bot.sendMessage(chat_id, getSystemInfo(), "");
        }
        
        else if (text == "/water") {
          if (!watering) {
            startWatering("Telegram");
            bot.sendMessage(chat_id, "💧 Watering started!", "");
          } else {
            bot.sendMessage(chat_id, "Watering already in progress", "");
          }
        }
        
        else if (text == "/stop") {
          if (watering) {
            stopWatering();
            bot.sendMessage(chat_id, "✅ Watering stopped", "");
          } else {
            bot.sendMessage(chat_id, "No watering in progress", "");
          }
        }
        
        else if (text.startsWith("/mode")) {
          if (text == "/mode auto") {
            manualMode = false;
            bot.sendMessage(chat_id, "🔧 Auto mode activated", "");
          } else if (text == "/mode manual") {
            manualMode = true;
            bot.sendMessage(chat_id, "🔧 Manual mode activated", "");
          }
        }
        
        else if (text == "/settings") {
          String settings = "⚙️ System Settings:\n";
          settings += "Soil Threshold: " + String(SOIL_THRESHOLD) + "%\n";
          settings += "Temp Alert High: " + String(TEMP_THRESHOLD_HIGH) + "°C\n";
          settings += "Temp Alert Low: " + String(TEMP_THRESHOLD_LOW) + "°C\n";
          settings += "Water Duration: " + String(WATER_DURATION/1000) + "s\n";
          settings += "Current Mode: " + String(manualMode ? "Manual" : "Auto");
          bot.sendMessage(chat_id, settings, "");
        }
        
        else {
          bot.sendMessage(chat_id, "Unknown command. Use /help for commands", "");
        }
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastBotScan = millis();
  }
}

void sendTelegramMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    bot.sendMessage(CHAT_ID, message, "");
    Serial.println("Telegram message sent");
  }
}

String getSystemInfo() {
  String info = "📊 System Status:\n";
  info += "Temperature: " + String(temperature, 1) + "°C\n";
  info += "Humidity: " + String(humidity, 1) + "%\n";
  info += "Soil Moisture: " + String(soilMoisturePercent) + "%\n";
  info += "Light Level: " + String(lightValue) + "\n";
  info += "Water Pump: " + String(watering ? "ACTIVE" : "INACTIVE") + "\n";
  info += "Mode: " + String(manualMode ? "Manual" : "Auto") + "\n";
  info += "Last Watered: " + lastWatered + "\n";
  info += "Status: " + systemStatus;
  return info;
}

// ==================== Google Sheets Functions ====================
void sendToGoogleSheets() {
  // Only send if data has changed significantly
  if (abs(soilMoisturePercent - lastSentSoilPercent) < 2 && 
      abs(temperature - lastSentTemp) < 0.5) {
    return;
  }
  
  HTTPClient http;
  String url = String(GOOGLE_SCRIPT_URL);
  url += "?temperature=" + String(temperature, 1);
  url += "&humidity=" + String(humidity, 1);
  url += "&soil=" + String(soilMoisturePercent);
  url += "&light=" + String(lightValue);
  url += "&watering=" + String(watering);
  url += "&mode=" + String(manualMode ? "Manual" : "Auto");
  
  http.begin(url);
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    Serial.printf("Data sent to Google Sheets. Response: %d\n", httpCode);
    lastSentSoilPercent = soilMoisturePercent;
    lastSentTemp = temperature;
  } else {
    Serial.printf("Failed to send data. Error: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();
}
