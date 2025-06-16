# 🤖 Smart Industry Project  
### 👨‍💻Amir Mahdi Barati (Programmer)

---

## 🌡️ DHT22 Temperature and Humidity Sensor

### 🔍 Overview and Efficiency  
The **DHT22 sensor** is a widely used digital sensor for measuring temperature and relative humidity with **high accuracy and stability**. It’s perfect for **smart industry** because it delivers reliable environmental data for:

- Process control ⚙️  
- Safety monitoring 🛡️  
- Automation triggers 🤖  

### ⚡ Key Advantages  
- Measures **temperature & humidity** with a single sensor  
- High accuracy: **±0.5°C** for temperature & **±2-5%** for humidity  
- Stable long-term performance & low power consumption 🔋  
- Easy digital interface with microcontrollers like **ESP32**

### ⚙️ Functional Operation  
DHT22 uses a **capacitive humidity sensor** + **thermistor** to measure air conditions and outputs digital data via a proprietary **single-wire protocol**. The microcontroller sends a start signal → sensor replies with calibrated data bits.

### ⚠️ Important Notes and Settings  
- **Power Supply:** 3.3V to 5V (recommend 3.3V for ESP32)  
- **Data Pin:** Connect to a GPIO pin with a **4.7kΩ pull-up resistor**  
- **Sampling Rate:** Max one reading every **1-2 seconds** (faster may cause errors)  
- **Signal Timing:** Precise timing needed → use compatible DHT22 libraries  
- **Environment:** Avoid direct sunlight ☀️, condensation 💧, or dust 🏜️  
- **Calibration:** Factory calibrated, but periodic checks recommended in critical apps
- **ssid:** Control-Panel
---

## 🔌 Connection Table

| Component       | ESP32 Pin | Signal Type    | Connected To        | Description                                              |
|-----------------|-----------|----------------|---------------------|----------------------------------------------------------|
| 🌡️ DHT22 Sensor  | GPIO 4    | Digital Input  | DHT22 Data Pin      | Measures ambient temperature & humidity for monitoring.  |
| 🌫️ MQ135 Sensor  | GPIO 34   | Analog Input   | MQ135 Analog Output | Detects air quality by sensing gases & pollution levels. |
| 🔴 Relay 1       | GPIO 12   | Digital Output | Relay Coil #1       | Controls device #1 (fan, motor) via automation.           |
| 🔴 Relay 2       | GPIO 13   | Digital Output | Relay Coil #2       | Controls device #2 in industrial processes.                |
| 🔴 Relay 3       | GPIO 14   | Digital Output | Relay Coil #3       | Smart switching of device #3 in industry.                  |
| 🔴 Relay 4       | GPIO 15   | Digital Output | Relay Coil #4       | Part of integrated smart automation system.                |
| 🔴 Relay 5       | GPIO 16   | Digital Output | Relay Coil #5       | Automated machinery control device #5.                      |
| 🔴 Relay 6       | GPIO 17   | Digital Output | Relay Coil #6       | Expands automation with device #6.                          |
| 🔴 Relay 7       | GPIO 18   | Digital Output | Relay Coil #7       | Flexible smart system operations device #7.                 |
| 🔴 Relay 8       | GPIO 19   | Digital Output | Relay Coil #8       | Completes relay control for diverse equipment.              |
| ⚡ 3.3V Power    | 3.3V      | Power Supply   | Sensors & Relays    | Stable 3.3V power for reliable operation.                    |
| 🟤 GND          | GND       | Ground         | Common Ground       | Common reference ground for signal integrity.               |

---

### 📅 Date: 2025  

Feel free to open issues or submit PRs if you want to contribute! 🚀  
Thanks for checking out the **Smart Industry Project**! 🔧🤖
