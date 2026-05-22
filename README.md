# Proximity11 MQTT IoT Monitor

STM32-based IoT project that reads proximity and ambient light data from the **VCNL4200** sensor (via the Proximity11 Click board) and publishes alerts to an MQTT broker over Wi-Fi using an ESP-AT module.

---

## Features

- Detects proximity events and ambient light changes via hardware interrupts
- Publishes real-time alerts to an MQTT topic (`test_proximity11`) on **HiveMQ public broker**
- Bidirectional UART bridge between the debug console (USART2) and the ESP-AT Wi-Fi module (USART3)
- Visual feedback via GPIO-toggled LEDs on proximity/light detection

---

## Hardware

| Component | Details |
|---|---|
| MCU | STM32L4 series (HAL-based) |
| Sensor | VCNL4200 – Proximity11 Click (I2C) |
| Wi-Fi | ESP8266/ESP32 module with AT firmware (USART3) |
| Debug UART | USART2 (redirected `printf`) |
| Interrupt pin | EXTI2 (GPIO_PIN_2) |
| LED – Proximity | GPIOA, PIN 11 |
| LED – Light | GPIOB, PIN 15 |

---

## Software Stack

- **STM32 HAL** (CubeMX-generated base)
- **Proximity11 driver** (`proximity11.h / .c`)
- **ESP-AT command set** for Wi-Fi and MQTT
- MQTT broker: `broker.hivemq.com:1883`

---

## How It Works

1. On startup, `application_init()` configures the Proximity11 sensor over I2C.
2. `ConnectToWiFi()` sends a sequence of AT commands to:
   - Set Wi-Fi mode
   - Join the configured AP
   - Configure and connect MQTT client
   - Subscribe to `test_proximity11`
3. The sensor's interrupt line (EXTI2) triggers `HAL_GPIO_EXTI_Callback`:
   - **Bit 7 set** → proximity detected → sets `ps_flag`
   - **Bit 6 set** → ambient light detected → sets `als_flag`
4. In the main loop, active flags trigger MQTT publish commands over USART3.

---

## MQTT Topics

| Topic | Message |
|---|---|
| `test_proximity11` | `Proximity detected!` |
| `test_proximity11` | `Light in room detected!` |

---

## Configuration

Edit `ConnectToWiFi()` in `main.c` to change credentials:

```c
// Wi-Fi network
"AT+CWJAP=\"YourSSID\",\"YourPassword\"\r\n"

// MQTT client identity
"AT+MQTTUSERCFG=0,1,\"clientID\",\"user\",\"password\",0,0,\"\"\r\n"

// Broker address
"AT+MQTTCONN=0,\"broker.hivemq.com\",1883,1\r\n"
```

---

## Project Structure

```
├── Core/
│   ├── Src/
│   │   ├── main.c          # Application logic
│   │   ├── i2c.c           # I2C peripheral init
│   │   ├── usart.c         # UART peripheral init
│   │   └── gpio.c          # GPIO init
│   └── Inc/
│       └── main.h
├── Drivers/
│   └── proximity11.h/.c    # Sensor driver
└── README.md
```

---

## Getting Started

1. Clone the repo and open in **STM32CubeIDE**.
2. Update Wi-Fi SSID/password and MQTT credentials in `main.c`.
3. Flash to your STM32L4 board.
4. Monitor output on USART2 (115200 baud) or subscribe to the MQTT topic with any client (e.g. MQTT Explorer, mosquitto_sub).

---

## License

Based on STMicroelectronics HAL libraries. See `LICENSE` for details.
