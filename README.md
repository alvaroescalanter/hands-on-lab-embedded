
# Embedded Platforms and Communications for IoT
## Hands-on Exercise

### 1. Project Overview

This project implements an embedded application using **Zephyr RTOS** on the **STM32 NUCLEO-WL55JC** development board.

The system measures ambient light using an LDR connected to the ADC and controls an external RGB LED according to the measured brightness. Button B2 allows switching between operating modes through short and long presses.

The application uses Zephyr threads, message queues, semaphores, timers and GPIO interrupts.

### 2. System Operation

The application implements a Finite State Machine (FSM) with three modes:

- **NORMAL:** Measures brightness every 2 seconds and controls the RGB LED:
  - Below 33%: Red.
  - 33–66%: Yellow.
  - Above 66%: Green.
- **BLUE:** RGB LED remains blue and ADC sampling is paused.
- **OFF:** RGB LED is turned off and ADC sampling is paused.

**Mode transitions:**
- Short press B2: NORMAL ↔ BLUE.
- Long press B2 (≥1 second): NORMAL / BLUE → OFF.
- Short press B2: OFF → NORMAL.

The serial terminal displays the current mode, brightness measurements and RGB LED color.

### 3. Software Architecture

The application is divided into four main modules:

- **main.c:** Implements the FSM, processes ADC measurements, selects RGB colors and handles mode transitions.

- **adc_thread.c / adc_thread.h:** Thread that waits for a semaphore triggered every 2 seconds, reads the LDR through ADC1 and sends measurements to the main application using a message queue.

- **buttonB2_thread.c / buttonB2_thread.h:** Handles button interrupts, debounce and short/long press detection using timers. Notifies the main application through semaphores.

- **rgb_thread.c / rgb_thread.h:** Thread that receives color commands through a message queue and controls the RGB LED GPIO outputs.

### 4. Hardware Configuration

**Board:** STM32 NUCLEO-WL55JC

**Pin connections:**
- LDR: PB1 (A0) — ADC1 Channel 5.
- RGB Red: PA5 — GPIO output.
- RGB Green: PA6 — GPIO output.
- RGB Blue: PA7 — GPIO output.
- Button B2: PA1 — GPIO input (board configuration).

The RGB LED uses a **common-anode configuration** with active-low GPIO outputs and current-limiting resistors.

The custom Device Tree overlay (`boards/nucleo_wl55jc.overlay`) configures ADC1 and the RGB LED pins. SPI1 is disabled to allow PA5, PA6 and PA7 to operate as GPIO outputs.

### 5. Build Instructions

The project requires a configured Zephyr RTOS development environment.

To build the application for the STM32 NUCLEO-WL55JC, run the following command from the project root directory:

```bash
west build -p auto -b nucleo_wl55jc . -d build -- -DDTC_OVERLAY_FILE=boards/nucleo_wl55jc.overlay -DCONF_FILE=confs/prj_nucleo_wl55jc.conf
```

The generated firmware is located in `build/zephyr/zephyr.hex` and can be flashed using STM32CubeProgrammer.
