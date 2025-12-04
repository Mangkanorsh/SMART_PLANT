# **Smart Plant Monitoring & Watering System - Complete Documentation**

## **📖 Table of Contents**
1. [Overview](#-overview)
2. [Features](#-features)
3. [Hardware Requirements](#-hardware-requirements)
4. [Wiring Diagram](#-wiring-diagram)
5. [Installation Guide](#-installation-guide)
6. [Configuration](#-configuration)
7. [Web Interface](#-web-interface)
8. [Telegram Bot Commands](#-telegram-bot-commands)
9. [Google Sheets Integration](#-google-sheets-integration)
10. [Code Structure](#-code-structure)
11. [Calibration Guide](#-calibration-guide)
12. [Safety Features](#-safety-features)
13. [Troubleshooting](#-troubleshooting)
14. [API Reference](#-api-reference)


---

## **🌱 Overview**

The **Smart Plant Monitoring & Watering System** is an IoT-based solution that automates plant care using an ESP32 microcontroller. It monitors environmental conditions (temperature, humidity, soil moisture, light) and automatically waters plants when needed. The system provides remote monitoring and control through a web interface, Telegram bot, and logs data to Google Sheets for analysis.

**Key Improvements in this Version:**
- Enhanced safety with immediate relay control on startup
- Active HIGH relay logic (common for 5V relay modules)
- Manual mode enabled by default for safety
- Display shows IP address for easy web access
- Added Telegram commands to adjust system parameters
- Fixed web server compatibility issues

---

## **✨ Features**

### **Core Monitoring**
- 🌡️ **Temperature & Humidity**: DHT22 sensor provides accurate environmental readings
- 🌱 **Soil Moisture**: HW-080 sensor measures soil moisture percentage
- ☀️ **Ambient Light**: TEMT6000 sensor detects light levels
- 📟 **Local Display**: 0.96" OLED shows real-time data and system status

### **Smart Watering**
- 🤖 **Automatic Mode**: Waters plants when soil moisture falls below threshold
- 🎮 **Manual Mode**: Full control via web or Telegram
- ⏱️ **Configurable Duration**: Adjust watering time from 1-60 seconds
- 📊 **Watering History**: Tracks last watering time and duration

### **Remote Access**
- 🌐 **Web Dashboard**: Responsive HTML5 interface accessible from any device
- 🤖 **Telegram Bot**: Control and monitor via Telegram messages
- 📱 **Mobile Friendly**: Works perfectly on smartphones and tablets
- 📡 **WiFi Connectivity**: Connects to local WiFi network

### **Data & Notifications**
- 📊 **Google Sheets Logging**: Automatic data logging every 5 minutes
- 🔔 **Smart Alerts**: Telegram notifications for high temperatures
- ⚠️ **System Status**: Real-time status monitoring
- 📈 **Historical Data**: Track plant health over time

### **Safety & Reliability**
- 🔒 **Manual Mode Default**: Prevents unintended watering on startup
- ⚡ **Safe Relay Control**: Active HIGH logic with forced state enforcement
- 🔄 **Error Recovery**: Automatic reconnection attempts
- 💾 **Parameter Persistence**: Settings maintained during operation

---

## **🛠 Hardware Requirements**

### **Component List**
| Component | Quantity | Purpose | Notes |
|-----------|----------|---------|-------|
| ESP32 Development Board | 1 | Main controller | Any ESP32 variant works |
| DHT22 Sensor | 1 | Temperature & Humidity | Digital output, 3.3V compatible |
| HW-080 Soil Moisture Sensor | 1 | Soil moisture measurement | Analog output, 5V powered |
| TEMT6000 Light Sensor | 1 | Ambient light detection | Analog output, 3.3V compatible |
| 5V Relay Module | 1 | Water pump control | Active HIGH logic |
| 12V DC Water Pump | 1 | Plant watering | Submersible recommended |
| 12V Power Adapter | 1 | Pump power supply | 1-2A current rating |
| 0.96" I2C OLED Display | 1 | Local display | SSD1306, 128x64 pixels |
| Female DC Power Plug | 1 | Power connection | Matches your adapter |
| Jumper Wires | 20+ | Connections | Male-to-female recommended |
| Breadboard | 1 | Prototyping | Optional but helpful |

### **Tools Required**
- USB cable for ESP32 programming
- Computer with Arduino IDE
- Screwdriver for terminal connections
- Multimeter (for troubleshooting)
- Waterproof enclosure (recommended)

---

## **🔌 Wiring Diagram**

### **Pin Connections Table**
| ESP32 Pin | Component → Pin | Wire Color | Notes |
|-----------|----------------|------------|-------|
| **3.3V** | DHT22 VCC | Red | 3.3V power |
| **3.3V** | TEMT6000 VCC | Red | 3.3V power |
| **3.3V** | OLED Display VCC | Red | 3.3V power |
| **GND** | Relay Module GND | Black | Shared ground |
| **GND** | DHT22 GND | Black | |
| **GND** | TEMT6000 GND | Black | |
| **GND** | Soil Sensor GND | Black | |
| **GND** | OLED Display GND | Black | |
| **5V** | Relay Module VCC | Red | Relay power (5V) |
| **5V** | Soil Sensor VCC | Red | HW-080 requires 5V |
| **GPIO 23** | Relay Module IN | Yellow | Pump control signal |
| **GPIO 32** | Soil Sensor AOUT | Green | Analog soil moisture |
| **GPIO 4** | DHT22 Data/OUT | Blue | Digital temperature/humidity |
| **GPIO 34** | TEMT6000 AOUT | White | Analog light level |
| **GPIO 21** | OLED Display SDA | Purple | I2C data line |
| **GPIO 22** | OLED Display SCL | Orange | I2C clock line |

### **Power Wiring**
```
12V Power Adapter → Female DC Plug
    ↓
Relay Module COM Terminal → 12V Water Pump (+)
    ↓
Relay Module NO Terminal → 12V Water Pump (-)
    ↓
ESP32 Vin (optional) → For ESP32 power if not using USB
```

### **⚠️ Critical Wiring Notes**
1. **Ground Connection**: All GND pins MUST be connected together
2. **Analog Pins**: Use ADC1 pins (32, 33, 34, 35, 36, 39) when using WiFi
3. **Relay Logic**: This code uses **Active HIGH** logic (HIGH = ON, LOW = OFF)
4. **Power Separation**: Keep 12V pump wiring separate from 3.3V/5V sensor wiring
5. **Waterproofing**: Use waterproof enclosures for electronics near water

---

## **📥 Installation Guide**

### **Step 1: Software Setup**

#### **Install Arduino IDE**
1. Download Arduino IDE 2.0+ from [arduino.cc](https://www.arduino.cc/en/software)
2. Install with default settings

#### **Add ESP32 Board Support**
1. Open Arduino IDE → File → Preferences
2. Add to "Additional Board Manager URLs":
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. Tools → Board → Board Manager → Search "ESP32" → Install "ESP32 by Espressif Systems"

#### **Install Required Libraries**
Open Arduino IDE and install these libraries via Library Manager (Sketch → Include Library → Manage Libraries):

| Library Name | Author | Version | Purpose |
|--------------|--------|---------|---------|
| **Adafruit SSD1306** | Adafruit | 2.5.7+ | OLED display control |
| **Adafruit GFX Library** | Adafruit | 1.11.9+ | Graphics for display |
| **DHT sensor library** | Adafruit | 1.4.6+ | DHT22 temperature/humidity |
| **ArduinoJson** | Benoit Blanchon | 6.21.3+ | JSON parsing |
| **UniversalTelegramBot** | Brian Lough | 1.3.0+ | Telegram bot interface |
| **ESPAsyncWebServer** | dvarrel | 1.2.7+ | Async web server |
| **AsyncTCP** | dvarrel | 1.1.1+ | Async TCP for ESP32 |

### **Step 2: Hardware Assembly**

#### **Assembly Steps:**
1. **Power Connections First:**
   - Connect 12V adapter to female DC plug
   - Connect plug wires to relay COM and NO terminals
   - Connect water pump to relay NO and COM terminals

2. **ESP32 Connections:**
   - Follow the wiring table above systematically
   - Connect power pins first (3.3V, 5V, GND)
   - Then connect data/signal pins
   - Double-check all connections

3. **Safety Checks:**
   - Verify no loose wires
   - Check for potential short circuits
   - Ensure relay module is properly oriented
   - Keep electronics away from water sources

### **Step 3: Upload Code**

1. **Copy the Code:**
   - Create new sketch in Arduino IDE
   - Copy the complete code from the previous message
   - Paste into the sketch

2. **Configuration (MUST DO):**
   Edit these lines in the code:

   ```cpp
   // WiFi Configuration
   const char* ssid = "YOUR_WIFI_SSID";      // Your WiFi name
   const char* password = "YOUR_WIFI_PASSWORD"; // Your WiFi password

   // Telegram Bot Configuration
   #define BOT_TOKEN "YOUR_BOT_TOKEN_HERE"  // From BotFather
   #define CHAT_ID "YOUR_CHAT_ID_HERE"      // Your Telegram Chat ID
   ```

3. **Board Settings:**
   - Tools → Board → ESP32 Dev Module
   - Tools → Port → Select your COM port
   - Tools → Flash Size → 4MB (Default)
   - Tools → Partition Scheme → Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)

4. **Upload:**
   - Click Upload button (→)
   - Wait for "Done uploading" message
   - Open Serial Monitor (Tools → Serial Monitor, 115200 baud)

---

## **⚙️ Configuration**

### **System Parameters**

| Parameter | Default | Range | Description |
|-----------|---------|-------|-------------|
| `SOIL_DRY` | 4095 | 0-4095 | Analog reading for completely dry soil |
| `SOIL_WET` | 1500 | 0-4095 | Analog reading for saturated soil |
| `SOIL_THRESHOLD` | 50% | 0-100% | Water when soil moisture below this |
| `TEMP_THRESHOLD_HIGH` | 35°C | 0-50°C | High temperature alert threshold |
| `TEMP_THRESHOLD_LOW` | 10°C | -10-30°C | Low temperature alert threshold |
| `WATER_DURATION` | 5000ms | 1000-60000ms | Default watering time |
| `DATA_SEND_INTERVAL` | 300000ms | 60000-86400000ms | Google Sheets logging interval |

### **WiFi Configuration**
```cpp
// Enter your WiFi credentials
const char* ssid = "YourWiFiName";
const char* password = "YourWiFiPassword";
```

### **Telegram Bot Setup**
1. **Create Bot:**
   - Open Telegram, search for `@BotFather`
   - Send `/newbot` and follow instructions
   - Save the Bot Token provided

2. **Get Chat ID:**
   - Start your bot in Telegram
   - Visit: `https://api.telegram.org/bot<YOUR_TOKEN>/getUpdates`
   - Send a message to your bot
   - Refresh the page, find `"chat":{"id":` in the JSON
   - Copy the ID number

3. **Configure in Code:**
   ```cpp
   #define BOT_TOKEN "123456789:ABCdefGHIjklMNOpqrsTUVwxyz"
   #define CHAT_ID "987654321"
   ```

### **Google Sheets Setup**
1. **Create Google Sheet:**
   - Create new Google Sheet at [sheets.google.com](https://sheets.google.com)
   - Name columns: `Timestamp, Temperature, Humidity, Soil, Light, Watering, Mode`

2. **Create Google Apps Script:**
   - Extensions → Apps Script
   - Delete default code, paste:
   ```javascript
   function doGet(e) {
     var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
     var timestamp = new Date();
     
     var temperature = e.parameter.temperature;
     var humidity = e.parameter.humidity;
     var soil = e.parameter.soil;
     var light = e.parameter.light;
     var watering = e.parameter.watering;
     var mode = e.parameter.mode;
     
     var rowData = [
       timestamp,
       parseFloat(temperature),
       parseFloat(humidity),
       parseInt(soil),
       parseInt(light),
       watering,
       mode
     ];
     
     sheet.appendRow(rowData);
     
     return ContentService
       .createTextOutput(JSON.stringify({result: "success"}))
       .setMimeType(ContentService.MimeType.JSON);
   }
   ```

3. **Deploy as Web App:**
   - Click Deploy → New deployment
   - Type: Web app
   - Execute as: Me
   - Who has access: Anyone
   - Click Deploy
   - Copy the Web App URL

4. **Configure in Code:**
   ```cpp
   const char* GOOGLE_SCRIPT_URL = "https://script.google.com/macros/s/.../exec";
   ```

---

## **🌐 Web Interface**

### **Accessing the Dashboard**
1. Power on the system
2. Check Serial Monitor for IP address
3. Open browser to: `http://[ESP32_IP_ADDRESS]`
4. Example: `http://192.168.1.100`

### **Dashboard Features**

#### **Real-time Monitoring**
- **Temperature**: Current temperature in Celsius
- **Humidity**: Current humidity percentage
- **Soil Moisture**: Soil moisture percentage (0-100%)
- **Light Level**: Ambient light intensity (0-4095)

#### **Control Panel**
| Button | Function | Description |
|--------|----------|-------------|
| **Auto Mode** | Enable automatic watering | System waters when soil is dry |
| **Manual Mode** | Enable manual control | User controls all watering |
| **Start Watering** | Begin watering | Activates pump immediately |
| **Stop Watering** | Stop watering | Deactivates pump |
| **Water for 5s** | Timed watering | Waters for exactly 5 seconds |
| **Water for 10s** | Timed watering | Waters for exactly 10 seconds |
| **Force Update** | Refresh sensors | Immediately reads all sensors |

#### **System Status Panel**
- **Status Indicator**: Color-coded status (Green=Normal, Yellow=Warning, Red=Alert)
- **Water Pump Status**: Shows ACTIVE/INACTIVE
- **Last Watered**: Timestamp of last watering
- **Current Mode**: Auto or Manual mode

### **Mobile Compatibility**
- Fully responsive design
- Works on smartphones, tablets, and desktops
- Touch-friendly buttons
- Optimized for mobile data usage

---

## **🤖 Telegram Bot Commands**

### **Complete Command List**

| Command | Usage | Description | Example |
|---------|-------|-------------|---------|
| `/start` | `/start` | Welcome message and help | `/start` |
| `/help` | `/help` | Show all commands | `/help` |
| `/status` | `/status` | Get current sensor readings | `/status` |
| `/water` | `/water` | Start manual watering | `/water` |
| `/stop` | `/stop` | Stop ongoing watering | `/stop` |
| `/mode auto` | `/mode auto` | Switch to automatic mode | `/mode auto` |
| `/mode manual` | `/mode manual` | Switch to manual mode | `/mode manual` |
| `/settings` | `/settings` | View current settings | `/settings` |
| `/duration` | `/duration <seconds>` | Set watering duration | `/duration 10` |
| `/set_soil` | `/set_soil <%>` | Set soil moisture threshold | `/set_soil 40` |
| `/set_temp_high` | `/set_temp_high <°C>` | Set high temp alert | `/set_temp_high 30` |
| `/set_temp_low` | `/set_temp_low <°C>` | Set low temp alert | `/set_temp_low 15` |

### **Command Details**

#### **Basic Commands**
```bash
/start
# Returns: Welcome message with command list

/status
# Returns: All sensor readings + system info + IP address

/water
# Starts watering for default duration (5 seconds)
# Confirmation: "Watering started!"

/stop
# Stops any ongoing watering
# Confirmation: "Watering stopped!"
```

#### **Mode Control**
```bash
/mode auto
# Enables automatic watering
# System will water when soil < threshold

/mode manual
# Enables manual control only
# User must start/stop watering
```

#### **Parameter Adjustment**
```bash
/duration 10
# Sets watering time to 10 seconds
# Range: 1-60 seconds

/set_soil 40
# Sets soil moisture threshold to 40%
# Range: 0-100%

/set_temp_high 30
# Sets high temperature alert to 30°C
# Alerts when temperature exceeds this

/set_temp_low 15
# Sets low temperature alert to 15°C
# Alerts when temperature drops below this
```

#### **System Information**
```bash
/settings
# Returns: Current system settings:
# - Soil threshold
# - Temperature thresholds
# - Water duration
# - Current mode
```

### **Automated Notifications**
The system sends automatic Telegram notifications for:

1. **System Startup**: When ESP32 boots up
2. **Watering Started**: When watering begins (with source)
3. **Watering Completed**: When watering finishes (with duration)
4. **High Temperature Alert**: When temperature exceeds threshold
5. **Mode Changes**: When switching between auto/manual

---

## **📊 Google Sheets Integration**

### **Data Logging Features**

#### **What Gets Logged**
| Column | Data Type | Description |
|--------|-----------|-------------|
| **Timestamp** | DateTime | When data was recorded |
| **Temperature** | Float | Temperature in Celsius |
| **Humidity** | Float | Humidity percentage |
| **Soil** | Integer | Soil moisture percentage |
| **Light** | Integer | Light level (0-4095) |
| **Watering** | String | "true" or "false" |
| **Mode** | String | "Auto" or "Manual" |

#### **Logging Behavior**
- **Interval**: Logs every 5 minutes (configurable)
- **Smart Logging**: Only logs when values change significantly
- **Change Thresholds**: 
  - Soil moisture: ±2%
  - Temperature: ±0.5°C
- **No Duplicates**: Avoids logging identical data

### **Setting Up Data Analysis**

#### **Create Charts in Google Sheets**
1. **Select your data range**
2. **Insert → Chart**
3. **Recommended charts:**
   - **Line Chart**: Temperature/Humidity over time
   - **Bar Chart**: Watering frequency
   - **Gauge Chart**: Current soil moisture

#### **Example Formulas**
```excel
=AVERAGE(B2:B100)  // Average temperature
=COUNTIF(G2:G100, "true")  // Count watering events
=MAX(C2:C100)  // Maximum humidity
=MIN(D2:D100)  // Minimum soil moisture
```

#### **Automated Reports**
Create scheduled reports with Google Apps Script:
```javascript
function createDailyReport() {
  var sheet = SpreadsheetApp.getActiveSpreadsheet();
  var data = sheet.getDataRange().getValues();
  // Analyze data and send email report
}
```

---

## **📝 Code Structure**

### **Main File Organization**
```cpp
// ==================== OLED Display ====================
// Display configuration and setup

// ==================== Pin Definitions ====================
// GPIO pin assignments

// ==================== Calibration ====================
// Sensor calibration constants

// ==================== Configuration ====================
// WiFi, Telegram, Google Sheets config

// ==================== Global Variables ====================
// System state variables

// ==================== HTML Web Page ====================
// Web interface HTML/CSS/JS

// ==================== Setup Function ====================
// Initialization code

// ==================== Main Loop ====================
// Main program loop

// ==================== Sensor Functions ====================
// readSensors(), checkTemperatureAlerts()

// ==================== Watering Control ====================
// startWatering(), stopWatering()

// ==================== Display Functions ====================
// updateDisplay()

// ==================== WiFi Functions ====================
// connectToWiFi()

// ==================== Web Server Functions ====================
// setupWebServer(), API endpoints

// ==================== Telegram Functions ====================
// handleTelegramMessages(), sendTelegramMessage()

// ==================== Google Sheets Functions ====================
// sendToGoogleSheets()
```

### **Key Functions Explained**

#### **`setup()` - Initialization**
```cpp
void setup() {
  // 1. Safety: Force relay OFF before enabling output
  digitalWrite(RELAY_PIN, LOW);
  pinMode(RELAY_PIN, OUTPUT);
  
  // 2. Initialize components
  Serial.begin(115200);
  display.begin(...);
  dht.begin();
  
  // 3. Connect to WiFi
  connectToWiFi();
  
  // 4. Start web server
  setupWebServer();
  
  // 5. Send startup notification
  sendTelegramMessage("System Started");
}
```

#### **`loop()` - Main Program**
```cpp
void loop() {
  // 1. Read all sensors
  readSensors();
  
  // 2. Force relay state (safety)
  if (!watering) digitalWrite(RELAY_PIN, LOW);
  else digitalWrite(RELAY_PIN, HIGH);
  
  // 3. Check Telegram messages
  handleTelegramMessages();
  
  // 4. Auto watering logic
  if (!manualMode && !watering && soilMoisturePercent < SOIL_THRESHOLD) {
    startWatering("Auto");
  }
  
  // 5. Stop watering if duration exceeded
  if (watering && (millis() - waterStartTime > WATER_DURATION)) {
    stopWatering();
  }
  
  // 6. Update display
  updateDisplay();
  
  // 7. Log data periodically
  if (millis() - lastDataSend > DATA_SEND_INTERVAL) {
    sendToGoogleSheets();
  }
  
  delay(1000);
}
```

#### **`readSensors()` - Data Acquisition**
```cpp
void readSensors() {
  // DHT22: Temperature & Humidity
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  // HW-080: Soil Moisture (analog)
  soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
  soilMoisturePercent = map(soilMoistureValue, SOIL_DRY, SOIL_WET, 0, 100);
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
  
  // TEMT6000: Light Level (analog)
  lightValue = analogRead(LIGHT_SENSOR_PIN);
}
```

#### **`startWatering()` - Water Pump Control**
```cpp
void startWatering(String source) {
  watering = true;
  waterStartTime = millis();
  digitalWrite(RELAY_PIN, HIGH); // Turn relay ON
  
  // Record time
  struct tm timeinfo;
  if(getLocalTime(&timeinfo)){
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    lastWatered = String(timeStr);
  }
  
  // Send notification
  String message = "Watering Started!\nSource: " + source;
  sendTelegramMessage(message);
}
```

---

## **🎯 Calibration Guide**

### **Soil Moisture Sensor Calibration**

#### **Step-by-Step Calibration:**
1. **Dry Calibration:**
   ```cpp
   // 1. Take sensor out of soil, wipe dry
   // 2. Note analog reading in Serial Monitor
   // 3. This is your SOIL_DRY value (typically 4095)
   const int SOIL_DRY = 4095; // Update this value
   ```

2. **Wet Calibration:**
   ```cpp
   // 1. Submerge sensor probes in water (electronics dry!)
   // 2. Note analog reading in Serial Monitor
   // 3. This is your SOIL_WET value (typically 1500-2000)
   const int SOIL_WET = 1500; // Update this value
   ```

3. **Test Calibration:**
   ```cpp
   // Upload this test code
   void testSoilSensor() {
     int raw = analogRead(SOIL_MOISTURE_PIN);
     int percent = map(raw, SOIL_DRY, SOIL_WET, 0, 100);
     percent = constrain(percent, 0, 100);
     Serial.print("Raw: "); Serial.print(raw);
     Serial.print(" -> Percent: "); Serial.print(percent);
     Serial.println("%");
   }
   ```

#### **Plant-Specific Thresholds**
| Plant Type | Recommended Threshold | Notes |
|------------|----------------------|-------|
| Succulents/Cacti | 20-30% | Drought-tolerant |
| Most Houseplants | 40-50% | General purpose |
| Tropical Plants | 60-70% | Moisture-loving |
| Vegetables | 50-60% | Regular watering |

### **Temperature Sensor Verification**
```cpp
void verifyDHT22() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  
  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT22 Error!");
  } else {
    Serial.print("Temp: "); Serial.print(temp);
    Serial.print("°C, Hum: "); Serial.print(hum);
    Serial.println("%");
  }
}
```

### **Light Sensor Calibration**
```cpp
void calibrateLightSensor() {
  // Test in different conditions
  Serial.println("Cover sensor - Dark:");
  Serial.println(analogRead(LIGHT_SENSOR_PIN));
  delay(3000);
  
  Serial.println("Room light:");
  Serial.println(analogRead(LIGHT_SENSOR_PIN));
  delay(3000);
  
  Serial.println("Direct sunlight (if possible):");
  Serial.println(analogRead(LIGHT_SENSOR_PIN));
}
```

---

## **🔒 Safety Features**

### **Electrical Safety**
1. **Relay Safety:**
   - Active HIGH logic prevents accidental activation
   - Default OFF state on startup
   - Manual mode by default
   - Forced state checking in main loop

2. **Power Isolation:**
   - 12V pump circuit separate from 3.3V logic
   - Proper grounding for all components
   - Fuse protection recommended for pump

3. **Waterproofing:**
   - Enclose ESP32 and relay in waterproof box
   - Use waterproof connectors for pump
   - Elevate electronics above water level

### **Software Safety**
1. **State Verification:**
   ```cpp
   // Force correct relay state every loop
   if (!watering) {
     digitalWrite(RELAY_PIN, LOW); // Double-check OFF
   }
   ```

2. **Duration Limits:**
   ```cpp
   // Maximum watering time enforcement
   if (watering && (millis() - waterStartTime > MAX_WATER_TIME)) {
     stopWatering();
     sendTelegramMessage("⚠️ Emergency stop: Watering too long!");
   }
   ```

3. **Error Recovery:**
   ```cpp
   // Auto-reconnect WiFi
   if (WiFi.status() != WL_CONNECTED) {
     connectToWiFi();
   }
   ```

### **Physical Safety**
- **Mount securely**: Prevent tipping or falling
- **Check regularly**: Inspect for wear or damage
- **Keep dry**: Protect from rain and splashes
- **Child/pet safety**: Keep wires and pump inaccessible

---

## **🔧 Troubleshooting**

### **Common Issues & Solutions**

| Issue | Symptoms | Solution |
|-------|----------|----------|
| **No WiFi Connection** | "WiFi Failed!" on display | 1. Check SSID/password<br>2. Move closer to router<br>3. Restart ESP32 |
| **Telegram Not Working** | No response to commands | 1. Verify Bot Token<br>2. Check Chat ID<br>3. Use `client.setInsecure()` |
| **Pump Won't Turn On** | No relay click, no water | 1. Check 12V power supply<br>2. Verify relay wiring<br>3. Test relay with `digitalWrite(RELAY_PIN, HIGH)` |
| **Pump Won't Turn Off** | Relay stuck ON | 1. Check relay logic (Active HIGH/LOW)<br>2. Add force-off in setup<br>3. Replace relay module |
| **Incorrect Sensor Readings** | Wrong values on display | 1. Recalibrate sensors<br>2. Check wiring<br>3. Replace faulty sensor |
| **OLED Not Displaying** | Blank screen | 1. Check I2C address (0x3C)<br>2. Verify SDA/SCL pins<br>3. Check power to OLED |
| **Web Interface Not Loading** | Can't access dashboard | 1. Check IP address in Serial Monitor<br>2. Try different browser<br>3. Clear browser cache |
| **Google Sheets Not Logging** | No data in spreadsheet | 1. Check Google Script URL<br>2. Verify internet connection<br>3. Check script permissions |

### **Diagnostic Commands**
Add to `loop()` for debugging:
```cpp
void diagnosticMode() {
  Serial.println("=== DIAGNOSTIC MODE ===");
  Serial.print("WiFi: "); Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
  Serial.print("Relay Pin: "); Serial.println(digitalRead(RELAY_PIN));
  Serial.print("Soil Raw: "); Serial.println(analogRead(SOIL_MOISTURE_PIN));
  Serial.print("Light Raw: "); Serial.println(analogRead(LIGHT_SENSOR_PIN));
  Serial.print("Watering: "); Serial.println(watering ? "YES" : "NO");
  Serial.print("Manual Mode: "); Serial.println(manualMode ? "YES" : "NO");
  Serial.println("========================");
}
```

### **Serial Monitor Output**
Normal startup sequence:
```
Connecting to WiFi
.......
WiFi connected!
IP address: 192.168.1.100
HTTP server started
Telegram message sent
Temperature: 22.5 °C
Humidity: 45.0 %
Soil Moisture: 65 %
Light Level: 1024
```

---

## **🔌 API Reference**

### **Web Server Endpoints**

| Endpoint | Method | Parameters | Response | Description |
|----------|--------|------------|----------|-------------|
| `/` | GET | None | HTML | Web dashboard |
| `/data` | GET | None | JSON | Sensor data |
| `/control` | GET | `action` | Text | Control pump |
| `/mode` | GET | `mode` | Text | Change mode |

### **JSON Response Format**
```json
{
  "temperature": 22.5,
  "humidity": 45.0,
  "soilMoisture": 65,
  "light": 1024,
  "watering": false,
  "manualMode": true,
  "systemStatus": "Normal",
  "lastWatered": "14:30:22"
}
```

### **Control Actions**
| Action | Description | URL Example |
|--------|-------------|-------------|
| `start` | Start watering | `/control?action=start` |
| `stop` | Stop watering | `/control?action=stop` |
| `water5` | Water for 5s | `/control?action=water5` |
| `water10` | Water for 10s | `/control?action=water10` |
| `update` | Refresh sensors | `/control?action=update` |

### **Mode Selection**
| Mode | Description | URL Example |
|------|-------------|-------------|
| `auto` | Enable auto mode | `/mode?mode=auto` |
| `manual` | Enable manual mode | `/mode?mode=manual` |

### **Example API Usage**
```bash
# Using curl
curl http://192.168.1.100/data
curl http://192.168.1.100/control?action=start
curl http://192.168.1.100/mode?mode=auto

# Using Python
import requests
response = requests.get("http://192.168.1.100/data")
data = response.json()
print(f"Temperature: {data['temperature']}°C")
```

---

## **🚀 Advanced Customization**

### **Adding More Sensors**
```cpp
// Example: Add a water level sensor
#define WATER_LEVEL_PIN 35

void setup() {
  pinMode(WATER_LEVEL_PIN, INPUT);
}

void loop() {
  int waterLevel = analogRead(WATER_LEVEL_PIN);
  // Add to display and web interface
}
```

### **Power Saving Mode**
```cpp
// Deep sleep between readings
void enterDeepSleep() {
  Serial.println("Entering deep sleep for 5 minutes");
  esp_sleep_enable_timer_wakeup(5 * 60 * 1000000);
  esp_deep_sleep_start();
}
```

### **Multiple Plant Support**
```cpp
// Array of soil sensors
const int soilPins[] = {32, 33, 34};
const int relayPins[] = {23, 25, 26};
const int numPlants = 3;

void waterPlant(int plantIndex) {
  digitalWrite(relayPins[plantIndex], HIGH);
  delay(WATER_DURATION);
  digitalWrite(relayPins[plantIndex], LOW);
}
```

### **Weather Integration**
```cpp
// Get weather forecast
void getWeatherForecast() {
  HTTPClient http;
  http.begin("http://api.weatherapi.com/v1/forecast.json?key=YOUR_KEY&q=auto:ip");
  int httpCode = http.GET();
  if (httpCode == 200) {
    String payload = http.getString();
    // Parse JSON for rain forecast
  }
  http.end();
}
```

---

## **📈 Performance Optimization**

### **Memory Optimization**
```cpp
// Enable PSRAM if available
#if CONFIG_SPIRAM_SUPPORT
  heap_caps_malloc_extmem_enable(512);
#endif

// Use PROGMEM for large strings
const char large_html[] PROGMEM = R"rawliteral(...)rawliteral";
```

### **WiFi Optimization**
```cpp
// Set WiFi power save mode
WiFi.setSleep(false); // Maximum performance

// Static IP configuration (optional)
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WiFi.config(local_IP, gateway, subnet);
```

### **Display Optimization**
```cpp
// Reduce display updates when unchanged
static float lastDisplayTemp = -100;
static int lastDisplaySoil = -1;

void updateDisplayConditional() {
  if (abs(temperature - lastDisplayTemp) > 0.5 || 
      abs(soilMoisturePercent - lastDisplaySoil) > 2) {
    updateDisplay();
    lastDisplayTemp = temperature;
    lastDisplaySoil = soilMoisturePercent;
  }
}
```

---
