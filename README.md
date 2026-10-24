# High-Speed ESP32 PID Line Follower Robot

A competition-grade, ultra-responsive line-following robot powered by an ESP32 microcontroller, Pololu QTR-8RC sensor array, and TB6612FNG motor driver. Features dual-mode operation: Bluetooth phone control and standalone automatic tracking.

---

## 🛠️ Complete Parts List

| Category | Component Description | Quantity | Notes / Specs |
| :--- | :--- | :--- | :--- |
| **Microcontroller** | ESP32 Development Board (30-pin DevKit V1) | 1 | Wi-Fi / Bluetooth Dual-Core |
| **Sensor Array** | Pololu QTR-8RC Reflectance Sensor Array | 1 | 8-Channel IR Reflectance Sensors |
| **Motor Driver** | TB6612FNG Dual H-Bridge Motor Driver | 1 | MOSFET-based, low drop voltage |
| **Motors** | N20 Micro Metal Gearmotors | 2 | 600 - 1000 RPM, 6V/12V rated |
| **Wheels** | High-Grip Silicone Wheels | 2 | Matched 3mm D-shaft for N20 |
| **Caster** | Mini Metal / Nylon Ball Caster | 1 | Front/Rear 3rd point of contact |
| **Power Supply** | 2S LiPo Battery (7.4V, 450-850mAh) | 1 | High-discharge rate (25C+) |
| **Voltage Regulator**| LM2596 or MP1584 DC-DC Buck Converter | 1 | Adjusted step-down output to 5.0V |
| **Switch** | SPST Mini Rocker Switch | 1 | Master Power ON/OFF |
| **Passives** | 100µF Electrolytic Capacitor | 1 | Power rail decoupling |
| | 0.1µF Ceramic Capacitor | 2 | Motor noise suppression |
| **Fasteners** | M3 Screws & Nuts (6mm, 10mm) | 16 | Chassis & Board mounting |
| | M2 Screws & Nuts (8mm) | 8 | N20 Motor Brackets |
| **Chassis** | 3D Printed or Carbon Fiber Chassis Frame | 1 | Lightweight & high-rigidity |

---

## 🔌 Pin-to-Pin Wiring Guide

### 1. Power Distribution Network
* **LiPo (+7.4V)** → Power Switch → **LM2596 IN+** & **TB6612FNG VM**
* **LiPo (GND)** → **Common GND Rail** (LM2596 IN-, ESP32 GND, Driver GND, Sensor GND)
* **LM2596 OUT+ (Calibrated to 5.0V)** → **ESP32 5V Pin**, **TB6612FNG VCC**, **QTR-8RC VCC**

### 2. Motor Driver (TB6612FNG) Connections
| TB6612FNG Pin | ESP32 Pin / Destination | Function |
| :--- | :--- | :--- |
| **VM** | LiPo Switch (+7.4V) | Motor Power |
| **VCC** | LM2596 Output (+5.0V) | Logic Power |
| **GND** | Common GND Rail | Ground |
| **STBY** | GPIO 33 | Driver Enable (Active HIGH) |
| **PWMA** | GPIO 14 | Left Motor PWM Channel 0 |
| **AIN1** | GPIO 26 | Left Motor Direction 1 |
| **AIN2** | GPIO 27 | Left Motor Direction 2 |
| **PWMB** | GPIO 25 | Right Motor PWM Channel 1 |
| **BIN1** | GPIO 12 | Right Motor Direction 1 |
| **BIN2** | GPIO 13 | Right Motor Direction 2 |
| **AO1 / AO2** | Left N20 Motor | Motor Output A |
| **BO1 / BO2** | Right N20 Motor | Motor Output B |

### 3. Sensor Array (Pololu QTR-8RC) Connections
| QTR-8RC Pin | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **VCC** | +5.0V Rail | Sensor Power |
| **GND** | Common GND Rail | Ground |
| **Sensor 1 (Far Left)** | GPIO 15 | Digital RC Timing Read |
| **Sensor 2** | GPIO 2 | Digital RC Timing Read |
| **Sensor 3** | GPIO 4 | Digital RC Timing Read |
| **Sensor 4** | GPIO 16 | Digital RC Timing Read |
| **Sensor 5** | GPIO 17 | Digital RC Timing Read |
| **Sensor 6** | GPIO 5 | Digital RC Timing Read |
| **Sensor 7** | GPIO 18 | Digital RC Timing Read |
| **Sensor 8 (Far Right)**| GPIO 19 | Digital RC Timing Read |

---

## 🚀 Step-by-Step Build & Setup Guide

### Step 1: Buck Converter Voltage Calibration (CRITICAL)
1. Before connecting the ESP32, wire the 7.4V battery to the **LM2596 IN+ / IN-**.
2. Turn on the switch and measure the output with a multimeter across **OUT+** and **OUT-**.
3. Rotate the brass potentiometer screw counter-clockwise until the multimeter reads **exactly 5.0V DC**.
4. Disconnect battery power before proceeding.

### Step 2: Mechanical Assembly
1. Mount the two N20 motors into the rear motor brackets using M2 screws.
2. Press fit the high-grip silicone wheels onto the N20 D-shafts.
3. Secure the ball caster at the front or rear to establish 3-point stability.
4. Mount the QTR-8RC sensor array at the front chassis lip, ensuring a clearance of **3mm - 5mm** above the track surface.

### Step 3: Wiring & Soldering
1. Solder motor leads directly to the N20 terminals and solder a 0.1µF capacitor across each motor terminal to suppress EMI noise.
2. Wire all ground leads to a single star-ground distribution rail.
3. Complete the connections between the ESP32, TB6612FNG, and QTR-8RC array according to the wiring guide above.

### Step 4: Software Upload & Calibration
1. Open `ESP32_PID_LineFollower.ino` in Arduino IDE.
2. Install the **QTRSensors** library via *Sketch -> Include Library -> Manage Libraries*.
3. Select board **ESP32 Dev Module** and upload the code.
4. Open `ControlPanel.html` in a web browser or use a Bluetooth Terminal App on Android to connect to `ESP32_LineFollower`.
5. Run the **CALIBRATE** command while sweeping the sensor array back and forth across a black track line on a white background.

---

## 📱 Live PID Tuning Guide
* **`Kp` (Proportional):** Controls steering force proportional to distance from center. Increase until robot tracks corners tightly without oscillating.
* **`Kd` (Derivative):** Damps oscillation and smooths turns. Increase if robot wobbles heavily on straightaways.
* **`Ki` (Integral):** Accumulates long-term positional error. Keep near 0 (0.0001) for fast line followers.
