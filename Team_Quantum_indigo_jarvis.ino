#define BLYNK_TEMPLATE_ID "TMPL310GamBp7"
#define BLYNK_TEMPLATE_NAME "Fall detection"
#define BLYNK_AUTH_TOKEN "nuOybxcKVY3hf3ldigbCGeeP7Q3dch0c"


#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <MPU6050.h>
#include <MAX30105.h>
#include <heartRate.h>
#include <math.h>

char ssid[] = "wow_anish";
char pass[] = "AN!$h2006";

// ---------- PINS ----------
#define SDA_PIN 8
#define SCL_PIN 9
#define BUTTON_PIN 5

// ---------- SENSORS ----------
MPU6050 mpu;
MAX30105 max30102;

// ---------- FAST FALL SETTINGS ----------
#define FREE_FALL 0.70
#define IMPACT 1.70
#define ROTATION 150.0
#define CANCEL_TIME 15000

bool fallDetected = false;
unsigned long fallTime = 0;

// ---------- HEART RATE ----------
long lastBeat = 0;
int heartRate = 0;
unsigned long lastHRUpdate = 0;


// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);
  delay(1000);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Start I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(500);

  // ---------------- MPU6050 ----------------

  mpu.initialize();

  if (mpu.testConnection())
    Serial.println("MPU6050: OK");
  else
    Serial.println("MPU6050: ERROR");

  // ---------------- MAX30102 ----------------

  if (max30102.begin(Wire, I2C_SPEED_STANDARD))
  {
    Serial.println("MAX30102: OK");

    max30102.setup();

    max30102.setPulseAmplitudeRed(0x0A);
    max30102.setPulseAmplitudeGreen(0);
  }
  else
  {
    Serial.println("MAX30102: ERROR");
  }

  // ---------------- BLYNK ----------------

  Serial.println("Connecting to Blynk...");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Initial Blynk values
  Blynk.virtualWrite(V0, 0);
  Blynk.virtualWrite(V1, "NORMAL");
  Blynk.virtualWrite(V2, "NORMAL");

  Serial.println("SYSTEM READY");
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
  Blynk.run();

  readHeartRate();
  detectFall();

  if (fallDetected)
    handleFall();
}


// =====================================================
// HEART RATE
// =====================================================

void readHeartRate()
{
  long irValue = max30102.getIR();

  // No finger
  if (irValue < 50000)
  {
    if (millis() - lastHRUpdate >= 1000)
    {
      Blynk.virtualWrite(V0, 0);
      lastHRUpdate = millis();
    }

    return;
  }

  // Detect heartbeat
  if (checkForBeat(irValue))
  {
    long delta = millis() - lastBeat;

    lastBeat = millis();

    float bpm = 60.0 / (delta / 1000.0);

    if (bpm >= 40 && bpm <= 220)
    {
      heartRate = (int)bpm;

      Serial.print("Heart Rate: ");
      Serial.print(heartRate);
      Serial.println(" BPM");
    }
  }

  // Send HR to Blynk every second
  if (millis() - lastHRUpdate >= 1000)
  {
    Blynk.virtualWrite(V0, heartRate);

    lastHRUpdate = millis();
  }
}


// =====================================================
// FAST MULTI-PARAMETER FALL DETECTION
// =====================================================

void detectFall()
{
  if (fallDetected)
    return;

  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(
    &ax, &ay, &az,
    &gx, &gy, &gz
  );

  // ---------- ACCELERATION ----------

  float Ax = ax / 16384.0;
  float Ay = ay / 16384.0;
  float Az = az / 16384.0;

  float acceleration = sqrt(
    Ax * Ax +
    Ay * Ay +
    Az * Az
  );

  // ---------- GYROSCOPE ----------

  float Gx = gx / 131.0;
  float Gy = gy / 131.0;
  float Gz = gz / 131.0;

  float rotation = sqrt(
    Gx * Gx +
    Gy * Gy +
    Gz * Gz
  );

  // ---------- FALL CONDITIONS ----------

  bool freeFall = acceleration < FREE_FALL;
  bool strongImpact = acceleration > IMPACT;
  bool strongRotation = rotation > ROTATION;

  // 1. Strong impact
  // Immediate detection
  if (strongImpact)
  {
    fallDetected = true;

    Serial.println("FAST IMPACT DETECTED");
  }

  // 2. Free fall + rotation
  else if (freeFall && strongRotation)
  {
    fallDetected = true;

    Serial.println("FREE FALL + ROTATION DETECTED");
  }

  // 3. Rotation + significant acceleration
  else if (strongRotation && acceleration > 1.5)
  {
    fallDetected = true;

    Serial.println("ROTATION + ACCELERATION DETECTED");
  }

  // ---------- FALL CONFIRMED ----------

  if (fallDetected)
  {
    fallTime = millis();

    Blynk.virtualWrite(
      V1,
      "FALL DETECTED"
    );

    Serial.println();
    Serial.println("******** FALL DETECTED ********");
    Serial.println("Press button within 15 seconds to cancel SOS");
  }
}


// =====================================================
// 15 SECOND SOS HANDLING
// =====================================================

void handleFall()
{
  // ---------- BUTTON PRESSED ----------

  if (digitalRead(BUTTON_PIN) == LOW)
  {
    delay(50);

    if (digitalRead(BUTTON_PIN) == LOW)
    {
      Serial.println("SOS CANCELLED");

      Blynk.virtualWrite(
        V1,
        "CANCELLED"
      );

      Blynk.virtualWrite(
        V2,
        "NORMAL"
      );

      fallDetected = false;

      delay(1000);

      Blynk.virtualWrite(
        V1,
        "NORMAL"
      );

      return;
    }
  }

  // ---------- 15 SECONDS COMPLETED ----------

  if (millis() - fallTime >= CANCEL_TIME)
  {
    Serial.println("SOS SENT");

    Blynk.virtualWrite(
      V2,
      "SOS SENT"
    );

    Blynk.logEvent(
      "sos_alert",
      "Possible fall detected. User did not cancel the SOS within 15 seconds."
    );

    fallDetected = false;
  }
}
