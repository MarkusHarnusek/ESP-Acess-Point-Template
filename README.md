# ESP32 Wi-Fi Access Point Template

This repository provides a ready-to-use template for building endpoint applications on your ESP32 board using the `espidf` framework.

## Features

- Preconfigured for [PlatformIO](https://platformio.org/).
- Simple and customizable Wi-Fi access point functionality.
- Default access point IP: `192.168.4.1`.

**Note:** After flashing the firmware onto the board, it is recommended to reset the device to avoid bootloader clutter in the serial output.

## Customization

You can customize the access point settings in the `src/main.c` file by modifying the following variables:

- **`AP_SSID`**: The SSID (name) of the Wi-Fi network.
- **`AP_PASSWORD`**: The password for the Wi-Fi network.
- **`AP_MAX_CLIENTS`**: The maximum number of clients allowed to connect to the network.
- **`html_content`**: The HTML content for the access point's main page.
- **`AP_AUTHMODE`**: The authentication protocol to be used for the network.

## Getting Started

1. Clone this repository and open it in PlatformIO.
2. Configure the access point settings in `src/main.c` as needed.
3. Build and flash the firmware onto your ESP32 board.
4. Reset the board after flashing to ensure proper operation.
5. Connect to the Wi-Fi network and navigate to `192.168.4.1` in your browser.

## Requirements

- ESP32 development board (e.g., AZ-Delivery DevKit V4).
- PlatformIO installed in your development environment.

For more information on PlatformIO, visit the [official documentation](https://docs.platformio.org/).