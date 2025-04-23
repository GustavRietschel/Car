# ESP32 Remote Controlled F1 Car

This project implements a remote-controlled car using ESP32 microcontrollers. The F1 car is controlled wirelessly using ESP-NOW protocol and includes features like temperature monitoring and speed control.

## Features

- Wireless control using ESP-NOW protocol
- Motor speed control
- Servo control for steering
- Temperature monitoring using DS18B20 sensor
- Real-time data transmission between controller and car

## Hardware Requirements

- ESP32 Development Board (x2 - one for car, one for controller)
- DC Motor with driver and encoder 
- Servo Motor
- DS18B20 Temperature Sensor
- Power Supply
- Jumper Wires
- Breadboard (optional)

## Pin Configuration

- Motor Control: 18
- Motor Direction: GPIO 19
- Motor Encoder: 21
- Servo Control: GPIO 25
- DS18B20: GPIO 4

## Software Requirements

- PlatformIO
- ESP-IDF Framework


## Building and Uploading

1. Clone this repository
2. Open the project in PlatformIO
3. Build the project
4. Upload to the ESP32 board

## Usage

1. Power on both ESP32 boards (car and controller)
2. The car will automatically connect to the controller
3. Use the controller to send commands to the car
4. The car will respond with temperature and speed data


## Author

Gustav Rietschel

