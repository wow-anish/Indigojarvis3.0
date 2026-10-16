# Fall Detection & Emergency Alert System

An IoT-based healthcare monitoring project designed to detect potential falls using motion sensors and provide an emergency alert mechanism. The system uses an ESP32-C3, an MPU6050 accelerometer and gyroscope, and a MAX30102 heart-rate sensor to monitor motion and basic health parameters.

## Overview

Falls can be dangerous, particularly for elderly individuals and people who may need assistance after an accident. This project aims to provide a wearable pendant-style prototype that detects potential falls, allows the user to cancel false alarms within a predefined time window, and sends an SOS status through a Blynk IoT dashboard.

## Features

- **Fall Detection:** Uses acceleration and angular rotation data from the MPU6050 to identify potential falls.
- **Heart Rate Monitoring:** Uses the MAX30102 sensor to measure heart rate.
- **Emergency SOS Alert:** Initiates an SOS alert when a detected fall is not cancelled.
- **15-Second Cancellation Window:** Gives the user time to cancel a false alarm using a push button.
- **IoT Monitoring:** Sends fall status, heart-rate readings, and SOS status to the Blynk dashboard over Wi-Fi.
- **Wearable Pendant Design:** Aims to provide a compact, portable alternative to a conventional wrist-worn device.

## Hardware Components

| Component | Purpose |
|---|---|
| ESP32-C3 Super Mini | Main controller and Wi-Fi connectivity |
| MPU6050 | Acceleration and angular rotation sensing |
| MAX30102 | Heart-rate and SpO₂ sensor module |
| Push Button | Cancels an SOS alert during the countdown |
| Jumper Wires | Circuit connections |
| USB Power Supply / Battery | Powers the prototype |
| Pendant Enclosure | Wearable housing |

*Note: SpO₂ sensor integration depends on the firmware implementation. The primary documented monitoring feature is heart rate.*

## Software and Technologies

- Arduino IDE
- Embedded C/C++
- ESP32-C3
- I²C communication
- Blynk IoT Platform
- SparkFun MAX3010x Sensor Library
- heartRate library
- MPU6050 sensor library

## Circuit Connections

### MPU6050 and MAX30102

Both sensors communicate with the ESP32-C3 over the I²C interface.

| ESP32-C3 | Sensor Connection |
|---|---|
| GPIO 8 | SDA |
| GPIO 9 | SCL |
| 3.3V | Sensor VCC, where supported |
| GND | Sensor GND |

Both sensors share the same I²C bus.

### Push Button

The cancellation button connects to the configured GPIO input pins. In the prototype firmware, GPIO 4 and GPIO 5 were used for the button-related connections.

**Important:** Verify the exact button wiring against your final firmware before assembling the circuit.

## System Workflow

1. The MPU6050 continuously measures acceleration and angular rotation.
2. The controller evaluates the readings against configured fall-detection thresholds.
3. When a potential fall is detected, the system initiates a 15-second cancellation window.
4. If the user presses the button within the allowed window, the SOS alert is cancelled.
5. If the countdown expires without cancellation, the system updates the SOS status through Blynk.
6. Heart-rate measurements from the MAX30102 are also made available for monitoring.

## Fall Detection Parameters

The prototype uses configurable thresholds to identify potential falls.

| Parameter | Prototype Value |
|---|---:|
| Free-fall threshold | 0.55 g |
| Impact threshold | 2.0 g |
| Rotation threshold | 180° |
| SOS cancellation window | 15 seconds |

These values are initial experimental thresholds, not clinically validated limits. Actual performance depends on sensor placement, calibration, movement patterns, and environmental conditions.

## Blynk IoT Configuration

Create a Blynk template and configure the required device credentials.

The prototype uses the following virtual datastream mapping:

| Virtual Pin | Data |
|---|---|
| V0 | Fall status |
| V1 | Heart-rate data |
| V2 | SOS status |

Ensure the datastream types match the values sent by your firmware. Configure your Wi-Fi credentials and Blynk authentication details in the Arduino sketch.

**Security:** Never commit real Wi-Fi passwords, Blynk authentication tokens, or other private credentials to a public repository.

## Installation and Setup

### 1. Install Arduino IDE

Download it from the official website:

https://www.arduino.cc/en/software

### 2. Configure ESP32 Support

Install the Espressif ESP32 board package using the Arduino IDE Boards Manager.

Select the appropriate board configuration for your hardware, such as **ESP32C3 Dev Module**.

### 3. Install Required Libraries

Install the sensor libraries used by your firmware, including:

- SparkFun MAX3010x Sensor Library
- heartRate library
- The MPU6050 library used in your sketch

Library names and dependencies may vary depending on the exact code version.

### 4. Configure Credentials

Add your Wi-Fi SSID, Wi-Fi password, Blynk template information, and device authentication token to the appropriate configuration section of your sketch.

### 5. Upload the Firmware

Connect the ESP32-C3 to your computer, select the correct COM port, compile the sketch, and upload it.

### 6. Test the System

- Confirm that both sensors are detected over I²C.
- Check sensor readings in the Serial Monitor.
- Test fall-detection behavior using controlled movements.
- Verify the 15-second cancellation mechanism.
- Confirm that the Blynk dashboard receives the expected data.

Avoid deliberately performing dangerous falls during testing.

## Project Applications

- Assistive wearable prototypes
- Elderly-care monitoring concepts
- Personal safety systems
- IoT-based healthcare research
- Embedded systems and sensor-fusion learning

## Future Improvements

- Improve fall classification using sensor fusion and activity recognition.
- Add caregiver notifications through a reliable alert-delivery service.
- Introduce GPS-based location sharing using suitable hardware.
- Optimize battery life and develop a compact PCB.
- Improve pendant enclosure design, comfort, and sensor placement.
- Conduct structured testing to evaluate sensitivity, specificity, and false-alarm rates.



This project is an educational prototype intended to explore fall detection and emergency-alert concepts. It is not a substitute for professional medical equipment, supervised care, or a validated emergency-response system.
