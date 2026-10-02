<div align="center">

<img src="https://capsule-render.vercel.app/api?type=waving&color=0:03234B,100:EF4444&height=200&section=header&text=STM32%20Temperature%20Monitor&fontSize=42&fontColor=ffffff&animation=fadeIn&fontAlignY=36&desc=Internal%20sensor%20%C2%B7%20VREFINT%20compensation%20%C2%B7%20I2C%20LCD&descAlignY=58&descSize=18" width="100%" alt="STM32 Temperature Monitor banner"/>

<a href="#-how-it-works"><img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=600&size=20&pause=1200&color=EF4444&center=true&vCenter=true&width=720&lines=The+chip+measures+its+own+temperature;Two-channel+ADC+scan+with+DMA;Supply-independent+reading+via+VREFINT;Updated+on+a+16x2+LCD+every+second" alt="Typing summary"/></a>

<br/>

[![STM32](https://img.shields.io/badge/STM32F401CCU6-Black%20Pill-03234B?logo=stmicroelectronics&logoColor=white)](https://www.st.com/en/microcontrollers-microprocessors/stm32f401cc.html)
[![C](https://img.shields.io/badge/C-HAL-A8B9CC?logo=c&logoColor=black)](Core/Src/main.c)
[![ADC](https://img.shields.io/badge/ADC1-scan%20%2B%20DMA-EF4444)](#-how-it-works)
[![STM32CubeIDE](https://img.shields.io/badge/STM32CubeIDE-03234B?logo=stmicroelectronics&logoColor=white)](https://www.st.com/en/development-tools/stm32cubeide.html)
<br/>
[![Status](https://img.shields.io/badge/Status-Complete-2ea44f)](#-status-and-ideas)
[![License](https://img.shields.io/badge/License-MIT-blue)](LICENSE)

</div>

---

## <img src="https://api.iconify.design/lucide/target.svg?color=%23EF4444" width="26" align="top" alt=""/> Project Overview

No external sensor needed: the **STM32F401's built-in temperature sensor** is read together with the **internal voltage reference (VREFINT)**, so the result does not depend on the exact supply voltage. The temperature is shown on a **16x2 I2C LCD**.

<div align="center">
<img src="output.jpeg" alt="Temperature shown on the LCD" width="230"/>
</div>

### Key Features

- <img src="https://api.iconify.design/lucide/thermometer.svg?color=%23EF4444" width="18" align="top" alt=""/> **Internal sensor**: ADC1 channel TEMPSENSOR, 480-cycle sampling as the datasheet requires.
- <img src="https://api.iconify.design/lucide/scale.svg?color=%230EA5E9" width="18" align="top" alt=""/> **VREFINT compensation**: the reference channel is converted in the same scan to work out the real ADC supply voltage.
- <img src="https://api.iconify.design/lucide/repeat.svg?color=%237C3AED" width="18" align="top" alt=""/> **Scan + DMA**: both channels in one sequence, triggered by TIM3, written straight into `AdcRaw[2]`.
- <img src="https://api.iconify.design/lucide/monitor.svg?color=%2310B981" width="18" align="top" alt=""/> **LCD readout**: `Temperature:` / `xx.xx C`, refreshed every second.

## <img src="https://api.iconify.design/lucide/network.svg?color=%23EF4444" width="26" align="top" alt=""/> How It Works

```mermaid
flowchart LR
    T[TIM3 TRGO] --> A[ADC1 scan<br/>rank 1: VREFINT<br/>rank 2: TEMPSENSOR]
    A -->|DMA2 stream 0| R[AdcRaw 0..1]
    R -->|ConvCplt callback<br/>sets flag| C[main loop, every 1 s<br/>compensate + convert]
    C --> L[16x2 I2C LCD]

    classDef hw fill:#03234B,stroke:#EF4444,color:#fff
    classDef fw fill:#EEEDFE,stroke:#7F77DD,color:#222
    class L hw
    class T,A,R,C fw
```

```
VDDA       = 1.21 V × 4095 / AdcRaw[0]          (VREFINT, nominal 1.21 V)
V_sense    = VDDA × AdcRaw[1] / 4095
T (°C)     = (V_sense − 0.76 V) / 0.0025 V/°C + 25
```

`V25 = 0.76 V` and `Avg_Slope = 2.5 mV/°C` are the typical values from the STM32F401 datasheet.

## <img src="https://api.iconify.design/lucide/plug.svg?color=%23EF4444" width="26" align="top" alt=""/> Wiring

| From | To (STM32F401CCU6) | Notes |
|---|---|---|
| LCD backpack SCL | PB6 | I2C1, 100 kHz, open-drain with pull-ups |
| LCD backpack SDA | PB7 | |
| LCD backpack VCC / GND | 5V / GND | |
| ST-Link SWDIO / SWCLK | PA13 / PA14 | Programming |

Clock: 25 MHz HSE → PLL (M=25, N=168, P=2) → 84 MHz.

## <img src="https://api.iconify.design/lucide/zap.svg?color=%23EF4444" width="26" align="top" alt=""/> Build and Flash

1. Open the project in **STM32CubeIDE** (or open `Temperature-Sensor.ioc`).
2. Enable float formatting for `sprintf` (Project Properties → C/C++ Build → MCU Settings → *Use float with printf*), otherwise the LCD shows an empty number.
3. Build, connect the ST-Link, and Run.
4. The LCD shows `Temp Monitor` for two seconds, then the live temperature.

To change the refresh rate, edit `HAL_Delay(1000)` in the main loop. For Fahrenheit: `Temperature * 9.0 / 5.0 + 32.0`.

## <img src="https://api.iconify.design/lucide/wrench.svg?color=%23EF4444" width="26" align="top" alt=""/> Troubleshooting

| Symptom | Check |
|---|---|
| LCD blank | PB6/PB7 wiring, 5V supply, contrast pot, I2C address in `LCD.h` (0x27 or 0x3F) |
| Number missing after `C` | Float printf support not enabled |
| Reading a few degrees off | Expected with typical V25/slope values; see the calibration idea below |
| Reading never changes | DMA interrupt enabled; TIM3 started; ADC started with length 2 |

## <img src="https://api.iconify.design/lucide/folder-tree.svg?color=%23EF4444" width="26" align="top" alt=""/> Project Structure

```
IntTemp-Sensor-STM32F401/
├── Core/
│   ├── Inc/LCD.h              # LCD driver API
│   ├── Src/LCD.c              # HD44780 4-bit driver over PCF8574
│   └── Src/main.c             # ADC scan + DMA, TIM3, conversion, display
├── Drivers/                   # STM32 HAL and CMSIS (generated)
├── Temperature-Sensor.ioc     # CubeMX configuration
└── output.jpeg
```

## <img src="https://api.iconify.design/lucide/gauge.svg?color=%23EF4444" width="26" align="top" alt=""/> Status and Ideas

Complete and working (June 2025). The internal sensor measures the **die temperature**, which runs a little above room temperature while the chip is working, and typical accuracy with datasheet constants is about ±1.5 to 2 °C.

Possible improvements:
- Use the factory calibration values stored in the chip (`VREFINT_CAL`, and `TS_CAL1` / `TS_CAL2` at 30 °C and 110 °C) instead of the typical constants
- Average several samples per reading
- Drive the display from the DMA callback instead of a fixed delay

## <img src="https://api.iconify.design/lucide/scale.svg?color=%23EF4444" width="26" align="top" alt=""/> License

My code (`LCD.c`, `LCD.h` and the `USER CODE` sections of `main.c`) is MIT, see [LICENSE](LICENSE). Files generated by STM32CubeMX and everything under `Drivers/` remain under STMicroelectronics' and Arm's licenses stated in those files.

Questions: mukeshkumar.cse24@gmail.com

<div align="center">

<img src="https://capsule-render.vercel.app/api?type=waving&color=0:EF4444,100:03234B&height=110&section=footer&animation=fadeIn" width="100%" alt=""/>

</div>
