# Arduino & IoT Projects Repository

Welcome to this repository, showcasing a collection of embedded systems projects developed using Arduino and ESP32 platforms. These projects demonstrate fundamental concepts of hardware interaction, data processing, and simple user interfaces, laying the groundwork for more complex Internet of Things (IoT) applications.

---

## What is the Internet of Things (IoT)?

The Internet of Things (IoT) refers to the vast network of physical objects — "things" — that are embedded with sensors, software, and other technologies for the purpose of connecting and exchanging data with other devices and systems over the internet.

**Key Fundamentals of IoT:**

1.  **Sensors/Actuators:** These are the "eyes and ears" (sensors) and "hands" (actuators) of an IoT device. Sensors collect data from the physical world (temperature, light, pressure, motion, button presses), while actuators interact with the physical world (turning lights on/off, opening/closing doors, moving motors).
2.  **Connectivity:** This is how IoT devices communicate. It can be wired (Ethernet) or wireless (Wi-Fi, Bluetooth, LoRa, Cellular, Zigbee, Z-Wave). The choice depends on range, power consumption, and data rate requirements.
3.  **Data Processing:** Raw data collected by sensors needs to be processed. This can happen locally on the device (edge computing) or be sent to a cloud server for analysis, storage, and decision-making.
4.  **User Interface/Applications:** This is how users interact with the IoT system, typically through mobile apps, web dashboards, or local displays (like an LCD). It allows users to monitor data, control devices, and configure settings.
5.  **Security:** A critical aspect of IoT, ensuring data privacy, device integrity, and protection against unauthorized access.

Our projects in this repository serve as practical examples of building foundational elements that contribute to an IoT ecosystem.

---

## Projects Overview

### 1. Arduino Calculator with Base Conversion

This project implements a functional calculator on an Arduino Uno, capable of performing basic arithmetic operations and converting results between different number bases (decimal, binary, hexadecimal, octal).

* **Hardware Used:**
    * Arduino Uno (or compatible board)
    * 4x4 Keypad Matrix
    * 16x2 I2C Liquid Crystal Display (LCD)
    * Breadboard and jumper wires
* **Key Features:**
    * **Basic Arithmetic:** Addition, subtraction, multiplication, and division.
    * **Number Input:** Digits 0-9 for inputting numbers.
    * **Base Conversion:** After a calculation, pressing the `=` button repeatedly cycles the result display through decimal, binary, hexadecimal, and octal formats.
    * **Clear Functionality:** A 'C' key to reset the calculator.
    * **LCD Interface:** Displays input numbers, operators, and results on a 16x2 LCD.
* **Code Highlights:**
    * Utilizes the `Keypad.h` and `LiquidCrystal.h` libraries for hardware interaction.
    * Custom functions for converting decimal numbers to binary, hexadecimal, and octal strings, and vice-versa.
    * **State Management** (e.g., `enteringFirstNumber`, `currentOperator`) to handle calculation flow.
    * Robust handling of the `=` key for both calculation and conversion cycling.
      
![**See it working on Thinkercad!**](https://www.tinkercad.com/things/f5nE4pSwjXz-arduino-calculator-with-base-conversion)

### 2. Arduino Elevator Simulation

This project simulates a multi-floor elevator system using an Arduino (or ESP32 for advanced features), NeoPixel LEDs to indicate the current floor, and a set of physical buttons for floor selection and control.

* **Hardware Used:**
    * Arduino Uno (or ESP32 DevKitC V4 for FreeRTOS version)
    * NeoPixel LED strip (10 pixels for 10 floors, 0-9)
    * LED for Door Status indication (with current-limiting resistor)
    * Analog Button Panel (resistor ladder circuit for 10+ buttons)
    * Breadboard and jumper wires
* **Key Features:**
    * **Floor Indication:** NeoPixels light up to show the elevator's current floor, with a random color for visual appeal.
    * **Button Input:** Simulates a real elevator panel with dedicated buttons for each floor, plus "Door Control" and "Emergency Stop."
    * **Elevator State Machine:** Manages elevator behavior through distinct states: `IDLE`, `MOVING_UP`, `MOVING_DOWN`, `ARRIVED`, `EMERGENCY`.
    * **Step-by-Step Movement:** The elevator "travels" one floor at a time with a configurable delay.
    * **Door Control:** A dedicated LED indicates when doors are opening/closing at a floor or are forced open during an emergency.
    * **Emergency Stop:** Halts elevator movement and opens doors until deactivated.
    * **Input Debouncing:** Prevents multiple registrations from a single physical button press.
* **Code Highlights:**
    * Uses the `Adafruit_NeoPixel.h` library for controlling the LEDs.
    * Employs `analogRead()` to differentiate between multiple buttons on a single analog pin using a resistor ladder.
    * Includes a robust state machine implementation for managing elevator logic.
    * For ESP32 versions, it can leverage **FreeRTOS tasks** (`xTaskCreatePinnedToCore`) for concurrent operations (e.g., handling CAN bus communication in a separate task, as seen in a related project).
    * Utilizes `millis()` for effective button debouncing.
      
![Arduino Elevator Simulation with NeoPixels](https://github.com/user-attachments/assets/cc73e167-ec26-4c47-b985-432a837c9cac)
![**See it working on Thinkercad!**](https://www.tinkercad.com/things/3BzBqNuqTWo-arduino-elevator-simulation-with-neopixels)

---

## Future Projects / Enhancements

These projects provide a solid foundation. Here are some ideas for future expansion and new IoT-related ventures:

### For the Calculator Project:

* **Floating-Point Precision:** Implement more robust handling for decimal numbers and potentially higher precision.
* **Memory Functions:** Add M+, M-, MR, MC buttons for storing and recalling values.
* **Scientific Functions:** Include trigonometric, logarithmic, and power functions.
* **Graphical LCD Support:** Upgrade to a graphical LCD (e.g., OLED or TFT) to display more complex output, graphs, or a more visually appealing interface.
* **Wireless Connectivity:** Make it an IoT calculator that logs calculations to a cloud database or can be controlled remotely.

### For the Elevator Simulation Project:

* **Multiple Elevators:** Simulate a building with multiple elevators, including call dispatch and scheduling algorithms.
* **Floor Display on LCD:** Integrate the 16x2 LCD to show current floor, destination, and status messages instead of just serial output.
* **Voice Announcements:** Add a small speaker module and integrate pre-recorded or synthesized voice announcements for floor arrivals and status updates.
* **Energy Consumption Simulation:** Add a simple model to estimate energy consumption based on elevator travel.
* **Cloud Integration (True IoT):**
    * Send elevator status (current floor, door status, emergency state) to a cloud platform (e.g., Firebase, AWS IoT, Google Cloud IoT Core).
    * Allow remote control of the elevator from a web dashboard or mobile app.
    * Implement predictive maintenance by analyzing travel patterns and sensor data.

### General IoT Project Ideas:

* **Smart Home Automation:**
    * **Environmental Monitoring:** Sensors for temperature, humidity, air quality (PM2.5, CO2) with data logging and alerts.
    * **Smart Lighting:** Control lights remotely, schedule lighting patterns, or adjust based on ambient light.
    * **Automated Plant Watering System:** Sensors for soil moisture, automatic pump control, and remote monitoring.
* **Asset Tracking:**
    * **GPS Tracker:** Track location of objects using GPS modules and send data via cellular (GSM) or LoRaWAN.
    * **Inventory Management:** Use RFID or NFC for tracking items in a warehouse.
* **Health and Wellness:**
    * **Wearable Fitness Tracker:** Simple heart rate monitoring, step counting, and data logging.
    * **Medication Dispenser:** Automated and scheduled medication dispensing with reminders.
* **Security:**
    * **Door/Window Sensors:** Detect open/closed states and trigger alerts.
    * **Motion Detectors:** PIR sensors for security alarms.

---

Feel free to explore, fork, and contribute to these projects! Your feedback and ideas are always welcome.
