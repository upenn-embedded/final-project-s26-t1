[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/-Acvnhrq)

# Final Project

**Team Number:** 1

**Team Name:** Virtual Etch-a-Sketch

| Team Member Name | Email Address           |
| ---------------- | ----------------------- |
| Cameron Shaw     | cshaw1@seas.upenn.edu   |
| Clayton Hickey   | clhickey@seas.upenn.edu |
| Elan Goldman     | elangol@seas.upenn.edu  |

**GitHub Repository URL:** https://github.com/upenn-embedded/final-project-s26-t1

**GitHub Pages Website URL:** [for final submission]\*

## Final Project Proposal

### 1. Abstract

A device with a large tri-color e-ink display and knobs that will behave like an Etch-a-sketch, a mechanical 2D plotter toy that draws lines on its screen. Our project will reimagine the experience of using an etch-a-sketch while keeping the shake to erase functionality and classic 2 knobs for controlling the drawing cursor. The virtual etch-a-sketch will also have USB support, so that the knobs can be used to create drawings in apps like MS Paint or GIMP.

### 2. Motivation

The goal of this project is to expand what an etch-a-sketch can be while also reducing the durability issues that arise from the completely mechanical design of the original. The etch-a-sketch was originally designed to be a toy, while the virtual etch-a-sketch is targeted at artists who want a more consistent experience and more ways to augment the etch-a-sketch medium.

### 3. System Block Diagram

![alt text](system_diagram.png)

**Key**
Red: Regulated 5V
Yellow: Regulated 3.3V

### 4. Design Sketches

We will 3D print the entire enclosure and knobs for the etch-a-sketch. We will need a soldering iron for making electrical connections, as well as adding heat set inserts if needed.

![alt text](<ESE 3500 Sketch.png>)

### 5. Software Requirements Specification (SRS)

**5.2 Functionality**

| ID         | Description                                                                                                                                                                               |
| :--------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **SRS-01** | The device shall display and persist user-drawn lines on the display; the cursor shall remain at the boundary of the screen if a user attempts to move it beyond the display coordinates. |
| **SRS-02** | The system shall update the cursor position based on simultaneous inputs from both knobs to facilitate diagonal and curved line rendering.                                                |
| **SRS-03** | The system shall respond to knob rotations with a display latency of less than 50 milliseconds to ensure real-time user interaction.                                                      |
| **SRS-04** | Upon USB connection to a host PC, the device shall enumerate as a Human Interface Device (HID) and map knob rotations to mouse-drag and click events.                                     |
| **SRS-05** | The system shall monitor the 3-axis accelerometer and trigger a screen-clear event only when a continuous shaking motion is detected for a duration exceeding 2 seconds.                  |
| **SRS-06** | The system shall change the active drawing color between black and red upon a toggle of the color mode switch, while maintaining the color of previously drawn lines.                     |

---

### 6. Hardware Requirements Specification (HRS)

**6.1 Definitions, Abbreviations**

- **HID:** Human Interface Device
- **IMU:** Inertial Measurement Unit
- **USB-C:** Universal Serial Bus Type-C

**6.2 Functionality**

| ID         | Description                                                                                                                                                                     |
| :--------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **HRS-01** | The device shall be powered by a 9V Alkaline battery and maintain operation for a minimum of 60 minutes under continuous use.                                                   |
| **HRS-02** | The power management circuit shall maintain uninterrupted system operation during the transition of plugging or unplugging a USB-C cable.                                       |
| **HRS-03** | A physical power switch shall be integrated to disconnect the battery from all downstream circuitry, resulting in zero current draw (excluding natural battery self-discharge). |
| **HRS-04** | The device enclosure shall feature two rotary inputs (knobs) positioned at the bottom corners of the display to emulate a mechanical drawing toy.                               |
| **HRS-05** | The rotary encoders used for knobs shall be non-detented to provide smooth, continuous rotation without tactile clicks or bumps.                                                |
| **HRS-06** | The device chassis and internal component mounting shall withstand a 2G shaking force without loss of electrical connectivity or functional failure.                            |

### 7. Bill of Materials (BOM)

https://docs.google.com/spreadsheets/d/18jHvc1XVHpzNt5ybeaLdjjYaHa1GXTPeQhFLGrUgIds/edit?usp=sharing

**AVR64DU32 Curiosity Nano**: This is the main MCU where we will run our bare metal C code. This MCU is responsible for user input, sending drawing commands to the E-Ink MCU, and interfacing with a connected laptop over USB. We chose this board over the Atmega328PB dev board because it has a USB 2.0 interface that we can use to have it act as a keyboard and/or mouse on the connected laptop.
**Rotary Encoder**: We will have two rotary encoders, just as an etch-a-sketch would, one for controlling the x-axis and one for the y-axis. We chose rotary encoders because they allow for many turns in each direction unlike a normal potentiometer and can be read using regular GPIO thanks to their digital output.
**eInk Display**: This is the display of the etch-a-sketch. We chose a large 7.5” display to approximate the dimensions of a full size etch-a-sketch. The eInk display gives the device a look closer to an real etch-a-sketch. The model can also display red pixels, and we will take advantage of this by including a color mode switch.
**IMU Breakout Board**: The IMU will be responsible for detecting when the user shakes the device to clear its screen. We chose this IMU because it is the same as the one used in WS3.
**RP2040 Feather ThinkInk**: This the secondary MCU used to control the eInk display. Because the eInk display requires a full refresh when updating it, we need a separate MCU with enough RAM to store the display state. We chose this board because it is designed for this exact purpose and includes the 24-pin connector that the eInk display uses.
**3.3V/5V Buck Converters**: As shown in the system diagram, we will need 3.3V for most devices on the board, though some require a higher supply voltage for their onboard regulators, hence why we also need a 5V buck converter. The buck converter boards we chose from Sparkfun provide a sufficient amount of current and feature an enable pin that our power switch could use.
**9V Battery Hat + Battery**: We want the device to function wirelessly, so we chose to use a 9V due to the University’s safety requirements. The battery holder will provide 7.2-9V, which is sufficient for the 5V and 3.3V converters.

### 8. Final Demo Goals

The device is battery powered, so it’s sufficient to just have some table space for the device to sit and a connected laptop to demonstrate the additional features of the virtual etch-a-sketch. We should bring an extra battery just in case.

### 9. Sprint Planning

| Milestone  | Functionality Achieved                                                                                                                                                                                                                                                            | Distribution of Work                                                                       |
| ---------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------ |
| Sprint #1  | The display turns on and can be drawn to. The USB HID on the Curiousity Nano functions on its own. A preliminary version of the enclosure has been designed.                                                                                                                      | We will split this work based on our availabilities by meeting before the sprint start.    |
| Sprint #2  | The display can be drawn to by the knobs, though some functionality or latency issues may be present. The Curiousity Nano USB HID functions as a mouse using control from the knobs separately. A physical enclosure draft has been printed and components have been test fitted. | We will split this work based on our availabilities by meeting before the sprint starts.   |
| MVP Demo   | We put all the components in the enclosure and the HID and display programs have been merged so they work simulatenously. The color mode and shake detection has also been added at this point.                                                                                   | We will split this work based on our availabilities by meeting before starting this phase. |
| Final Demo | We satisfy all our required system requirements.                                                                                                                                                                                                                                  | We will split this work based on our availabilities by meeting before starting this phase. |

**This is the end of the Project Proposal section. The remaining sections will be filled out based on the milestone schedule.**

## Sprint Review #1

### Last week's progress

### Current state of project

### Next week's plan

## Sprint Review #2

### Last week's progress

### Current state of project

### Next week's plan

## MVP Demo

## Final Report

Don't forget to make the GitHub pages public website!
If you’ve never made a GitHub pages website before, you can follow this webpage (though, substitute your final project repository for the GitHub username one in the quickstart guide): [https://docs.github.com/en/pages/quickstart](https://docs.github.com/en/pages/quickstart)

### 1. Video

### 2. Images

### 3. Results

#### 3.1 Software Requirements Specification (SRS) Results

| ID     | Description                                                                                               | Validation Outcome                                                                          |
| ------ | --------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| SRS-01 | The IMU 3-axis acceleration will be measured with 16-bit depth every 100 milliseconds +/-10 milliseconds. | Confirmed, logged output from the MCU is saved to "validation" folder in GitHub repository. |

#### 3.2 Hardware Requirements Specification (HRS) Results

| ID     | Description                                                                                                                        | Validation Outcome                                                                                                      |
| ------ | ---------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------- |
| HRS-01 | A distance sensor shall be used for obstacle detection. The sensor shall detect obstacles at a maximum distance of at least 10 cm. | Confirmed, sensed obstacles up to 15cm. Video in "validation" folder, shows tape measure and logged output to terminal. |
|        |                                                                                                                                    |                                                                                                                         |

### 4. Conclusion

## References
