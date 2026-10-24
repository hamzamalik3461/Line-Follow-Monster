#include <QTRSensors.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to enable it
#endif

BluetoothSerial SerialBT;

// --- PIN DEFINITIONS ---
#define AIN1 26
#define AIN2 27
#define PWMA 14
#define BIN1 12
#define BIN2 13
#define PWMB 25
#define STBY 33

#define PWM_FREQ 5000
#define PWM_RES 8 // 0-255
#define CHAN_MOT_A 0
#define CHAN_MOT_B 1

const uint8_t SensorPins[] = {15, 2, 4, 16, 17, 5, 18, 19};
QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

// --- PID PARAMETERS ---
float Kp = 0.08;
float Ki = 0.0001;
float Kd = 1.2;

int baseSpeed = 200;
int maxSpeed = 255;

int lastError = 0;
float integral = 0;

bool robotRunning = false;
bool standaloneMode = true;
unsigned long startTime = 0;

void setup() {
  Serial.begin(115200);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  ledcSetup(CHAN_MOT_A, PWM_FREQ, PWM_RES);
  ledcAttachPin(PWMA, CHAN_MOT_A);
  
  ledcSetup(CHAN_MOT_B, PWM_FREQ, PWM_RES);
  ledcAttachPin(PWMB, CHAN_MOT_B);

  digitalWrite(STBY, LOW);

  qtr.setTypeRC();
  qtr.setSensorPins(SensorPins, SensorCount);

  SerialBT.begin("ESP32_LineFollower");
  Serial.println("Bluetooth started. Device: ESP32_LineFollower");

  startTime = millis();
  while (millis() - startTime < 3000) {
    if (SerialBT.hasClient()) {
      standaloneMode = false;
      Serial.println("Phone Connected via Bluetooth!");
      break;
    }
    delay(50);
  }

  calibrateSensors();

  if (standaloneMode) {
    Serial.println("Standalone Mode: Starting run in 1 second...");
    delay(1000);
    robotRunning = true;
    digitalWrite(STBY, HIGH);
  }
}

void loop() {
  handleBluetoothCommands();

  if (robotRunning) {
    calculatePIDAndDrive();
  } else {
    stopMotors();
  }
}

void calibrateSensors() {
  Serial.println("Calibrating sensors... Sweep across line.");
  digitalWrite(STBY, LOW);

  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
    delay(10);
  }
  Serial.println("Calibration Complete!");
}

void calculatePIDAndDrive() {
  uint16_t position = qtr.readLineBlack(sensorValues);
  int error = position - 3500;

  integral += error;
  integral = constrain(integral, -10000, 10000);
  
  int derivative = error - lastError;
  lastError = error;

  float motorSpeedDifference = (Kp * error) + (Ki * integral) + (Kd * derivative);

  int leftSpeed = baseSpeed + motorSpeedDifference;
  int rightSpeed = baseSpeed - motorSpeedDifference;

  leftSpeed = constrain(leftSpeed, 0, maxSpeed);
  rightSpeed = constrain(rightSpeed, 0, maxSpeed);

  setMotorSpeeds(leftSpeed, rightSpeed);
}

void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  digitalWrite(STBY, HIGH);

  if (leftSpeed >= 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    leftSpeed = -leftSpeed;
  }

  if (rightSpeed >= 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    rightSpeed = -rightSpeed;
  }

  ledcWrite(CHAN_MOT_A, leftSpeed);
  ledcWrite(CHAN_MOT_B, rightSpeed);
}

void stopMotors() {
  digitalWrite(STBY, LOW);
  ledcWrite(CHAN_MOT_A, 0);
  ledcWrite(CHAN_MOT_B, 0);
}

void handleBluetoothCommands() {
  if (SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n');
    command.trim();

    if (command == "START") {
      robotRunning = true;
      digitalWrite(STBY, HIGH);
      SerialBT.println("STATUS: RUNNING");
    } 
    else if (command == "STOP") {
      robotRunning = false;
      stopMotors();
      SerialBT.println("STATUS: STOPPED");
    } 
    else if (command == "CALIBRATE") {
      robotRunning = false;
      calibrateSensors();
      SerialBT.println("STATUS: CALIBRATED");
    }
    else if (command.startsWith("KP:")) {
      Kp = command.substring(3).toFloat();
      SerialBT.print("SET Kp: "); SerialBT.println(Kp);
    }
    else if (command.startsWith("KI:")) {
      Ki = command.substring(3).toFloat();
      SerialBT.print("SET Ki: "); SerialBT.println(Ki);
    }
    else if (command.startsWith("KD:")) {
      Kd = command.substring(3).toFloat();
      SerialBT.print("SET Kd: "); SerialBT.println(Kd);
    }
    else if (command.startsWith("SPEED:")) {
      baseSpeed = command.substring(6).toInt();
      baseSpeed = constrain(baseSpeed, 0, 255);
      SerialBT.print("SET SPEED: "); SerialBT.println(baseSpeed);
    }
  }
}
