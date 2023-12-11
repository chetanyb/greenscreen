# Green$creen: Vehicle Emission Monitoring and Red Carbon Rating System

## Overview

Green$creen aims to revolutionize carbon emission tracking with IoT and blockchain. We measure vehicle emissions in real-time, and balance emissions on-chain with RED tokens. It focuses on monitoring and analyzing vehicle emissions using sensors such as MQ135 and SHT10, combined with controllers like ESP32. The system provides real-time emission data, calculates a Red Carbon Rating, and securely stores the information on the Ethereum blockchain.

## Features

- **Sensor Integration**: Green$creen supports popular environmental sensors like MQ135 and SHT10, providing accurate measurements of air quality and temperature.
  
- **Controller Compatibility**: The project is designed to seamlessly work with ESP32 controllers, ensuring efficient data collection and processing.

- **Real-time Monitoring**: Green$creen offers real-time monitoring of vehicle emissions, enabling instant analysis and reporting.

- **Ethereum Blockchain Integration**: All emission data is securely stored on the Ethereum blockchain, ensuring transparency and immutability.

- **Red Carbon Rating Calculation**: The system calculates a Red Carbon Rating based on the emission data, offering a standardized measure of a vehicle's environmental impact.

## Getting Started

### Prerequisites

- Install [PlatformIO](https://platformio.org) for ESP32 development.
- Open features/iot/ in platformIO and upload the code to your device
-Run ngrok for the ESP host

### Installation

1. Clone the Green$creen repository:
   ```bash
   git clone https://github.com/chetanyb/greenscreen

2. Open the Arduino IDE and load the Green$creen project.

3. Configure the project with your Ethereum wallet address and other necessary parameters.

4. Upload the code to your ESP32 controller.

### Usage
1. Connect the MQ135 and SHT10 sensors to the ESP32 controller as per the provided schematics.

2. Power on the system and let it start monitoring vehicle emissions.

3. The data will be securely pushed to the Ethereum blockchain.

4. Access the Red Carbon Rating through the provided interface or query the blockchain.

### License
This project is licensed under the MIT License - see the LICENSE file for details.