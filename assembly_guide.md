# Assembly Guide for oxymeter

This guide provides detailed instructions on how to assemble the oxymeter device.

## Prerequisites

Before starting the assembly process, ensure you have the following tools and components ready:

- Arduino IDE installed on your computer
- All electronic components listed in the [Components List](hardware/components.md)
- 3D printer for printing support structures
- Laser cutting for support structures
- Soldering equipment for assembling electronic circuits
- Basic hand tools (screwdrivers, pliers, etc.)

## Step 1: Clone the Repository

To get started, clone the project repository to your local machine:

```bash
git clone https://github.com/EnssatPhotonicsProjects/Dissolved-oxygen-sensor.git
```
## Step 2: Assemble the Electronic Components

### Using a Breadboard

If you are prototyping or prefer not to use a PCB, you can assemble the circuit on a breadboard. Refer to the [Schematic.pdf](hardware/Schematic.pdf) for detailed connections.

### Using a PCB

1. Download the PCB design file from the repository:
2. Order the PCB from a manufacturer or print it if you have the necessary equipment.
3. Solder the components onto the PCB following the schematic diagram.

## Step 3: Assemble the Support Structures

Use the provided STL file to 3D print the necessary support structures with PLA filament, and a filling density of ??%. Printing takes about ?? hours : 

- [LED and Photodiode Support](hardware/3D_ready_to_print_LED-Photodiode-Support.stl)

Assemble the parts according to the instructions provided in the [Assembly Manual](structure_assembly_guide.md).

## Step 4: Upload the Arduino Code

1. Open the `.ino` file in the Arduino IDE.
2. Connect your Arduino board to the computer.
3. Select the correct board and port in the Arduino IDE.
4. Upload the code to the Arduino board.

## Step 5: Calibration


## Troubleshooting

If you encounter any issues during the assembly or operation of the device, open an issue in the repository.
