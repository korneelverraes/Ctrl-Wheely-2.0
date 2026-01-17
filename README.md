# Ctrl-Wheely
*Optimization of car dashboard interaction through a physical movable button in the center console and a head-up display.* 

*Projectteam: Korneel Verraes; Warre Robbe; Seppe Nijs; Bram Tops*

19/01/2026

## Introduction
Beetje uitleg over wat ctrl wheely is en waarom

## Embodiment
verwijzing + uitleg CAD files

uitleg over prototype

## Hardware

The Ctrl-Wheely hardware consists of a custom-built physical control dial connected to an Arduino-based control system. The dial provides rotational input, directional movement, button presses and haptic feedback through a motor with encoder. The small buttons are soldered on a circular prototyping board.

All components are wired according to the electrical scheme and physical setup shown below.

### Components

- 1 x Arduino Uno  
- 1 x iPower GM3506 Gimbal Motor with Encoder  
- 1 x SimpleFOC Mini motor driver  
- 5 x DRV5053 Analog-Bipolar Hall Effect Sensors  
- 2 x Small push button (6 mm)  
- 1 x Large push button (12 mm)  
- 1 x DC power supply (8-30 V)  
- 1 x Circular prototyping board (diameter 43 mm)  

### Electrical Diagram and Hardware Setup

<div style="display:flex; gap:20px;">
  <img src="./images/electrical-diagram.jpg" alt="Electrical Diagram" width="47%">
  <img src="./images/components-cables.jpg" alt="Hardware Setup Photo" width="49%">
</div>

The electrical diagram illustrates all power, signal, and communication connections between the Arduino, motor driver, encoder, hall effect sensors, and push buttons. The photograph shows the components cables.

| **Component** | **Connection** |
|------------------------|----------------|
| **Motor driver** | **Arduino UNO** |
| EN (purple) | D8 |
| IN3 (blue) | D5 |
| IN2 (green) | D6 |
| IN1 (yellow) | D9 |
| GND (black) | GND |
| | |
| **Magnetic encoder** | **Arduino UNO** |
| GND (white) | GND |
| VDD (red) | +5V |
| MISO (green) | D12 |
| MOSI (yellow) | D11 |
| SCL (purple) | D13 |
| CS (black) | D10 |
| | |
| **Hall effect sensors** | **Arduino UNO** |
| VCC | +3.3V |
| GND | GND |
| OUT x 5 | A0 - A4 |
| | |
| **Push button (press)** | **Arduino UNO** |
| OUT (brown) | D2 |
| GND (yellow-green) | GND |
| | |
| **Push button (left / right)** | **Arduino UNO** |
| OUT left (green) | D3 |
| OUT right (pink) | D4 |
| GND (yellow-green) | GND |
| | |
| **Motor phase cables** | **Motor driver** |
| A (black) | M1 |
| B (black) | M2 |
| C (black) | M3 |



## Software
### Arduino

The Arduino controls all hardware inputs and outputs of the Ctrl-Wheely system. The complete Arduino implementation can be found in `Ctrl-Wheely.ino`.

The software makes use of the **SimpleFOC** library to control the gimbal motor and to provide haptic feedback based on the active function and user interaction.

Before running the main Arduino code, the correct physical positions of the hall effect sensors must be determined. This is done using the calibration sketch `Magnet_pos_test.ino`. This sketch allows you to read and verify the sensor outputs while moving the magnet position.

The motor driver voltage must be set according to the external power supply used in the setup. Make sure to update the voltage value in the Arduino code to match your power supply.

```cpp
driver.voltage_power_supply = 11,2;
```


### PyGame
The project includes a Head-Up Display (HUD) that projects information onto the windshield, indicating which function is currently being controlled via the dial.

In a previous iteration of the project, the HUD was developed using ProtoPie and connected to an Arduino. Due to the limitations of ProtoPie, all scenes are now fully implemented in Python using the PyGame library.

The user interface that was designed and tested within the course User-Centered Design has been recreated almost one-to-one in PyGame. This approach ensures that the validated and positively received UX is preserved in the final implementation.

Within PyGame, it is possible to receive serial data directly from the Arduino, to which the dial is connected. This incoming serial input is used to dynamically update elements of the HUD in real time.

As a result, the system is not a simulated “Wizard of Oz” interface, but a fully functional and directly responsive HUD that accurately reflects the user’s interactions with the dial.

The PyGame implementation was further extended with additional functionalities, including: Extra buttons, Submenus, Variable values per passenger or driver and Additional configurable interface elements

The full PyGame implementation of the HUD can be found in `UI_HUD_pygame.py`. 

Make sure to set the correct COM port on **line 55**.

```py
ser = serial.Serial('COM5', 115200, timeout=0)
```

For HUD usage, uncomment **lines 1027 and 1028** to mirror the screen over the x-axis.

```py
#canvas = pygame.transform.flip(screen, False, True)
#screen.blit(canvas, (0,0))
```



