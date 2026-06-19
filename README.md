<p align="center">
  <img src="images/Enssat-UnivRennes_RVB.png" alt="ENSSAT Logo" width="500"/>
</p>

# Dissolved-oxygen-sensor

## Description

This project aims to develop a low-cost optode for measuring dissolved oxygen concentration in water, a key indicator of the health of marine environments. The project is conducted by students in photonics from Enssat in collaboration with the European Institute for Marine Studies (IUEM) and is part of an open-source initiative to facilitate easy and cost-effective replication of the device.

This GitHub repository contains all the necessary resources and instructions for making the device.

The document [Optode overview](Optode_overview.pdf) aims to provide an overview of the technical aspects of our sensor.

## Features


## Principle of dissolved oxygen sensor
The measurement principle is based on the blue light absorbed by a membrane exposed to water. The membrane then emits red light. By measuring the phase shift between the blue and red light, the concentration of dissolved oxygen can be determined.


## Functionality




## Components

The main components are:

| Component | Description |
|-----------|-------------|
| LED | Light Emitting Diode for blue excitation |
| Photodiode | Detects red fluorescence |
| Arduino | Microcontroller for processing |
| ADC | Analog to Digital Converter |
| Transistor | For LED power supply |
| ......... | ......... |

For a detailed list of components with the references, please refer to the [Components List](hardware/components.md).


## Construction of the fluorometer

### Prerequisites

- Arduino IDE
- Electronic components
- 3D printer
- ...

### Installation Steps

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/EnssatPhotonicsProjects/Dissolved-oxygen-sensor.git
   ```

### Assembling the dissolved oxygen sensor

Follow the steps of the [assembly guide](assembly_guide.md) to build the dissolved oxygen sensor.



## Avenues for Improvement

- ...

Contributions are welcomed to improve these points.

## To contribute

Contributions to this project are welcomed. Follow this procedure contribute:

1. **Fork the repository** and create your branch:

   ```bash
   git checkout -b my-new-feature
   ```

2. **Make your changes** and test them thoroughly.

3. **Document your changes** in the [EXTERNAL_CONTRIBUTIONS](EXTERNAL_CONTRIBUTIONS.md) file. Begin the file with your name, location and date.

4. **Submit a Pull Request** with a detailed description of your changes.

## Contributors

Team 2025-2026 :
- Arthur Allaeys (IntricationQuantique)
- Elisa Charron (Quazou54)
- Brice Chupin
- Léon Dolaine
- Constant Ekpo
- Ethan Geffroy
- Nathan Maës
- Alexandre Martin-Lefèvre
- Adam Monzon
- Lucile Pointud
- Sean Swidurski




## Acknowledgments

The authors warmly thank Thierry Chartier, Laurent Bramerie, Antoine Courtay supervisors of the project at Enssat, Philippe Laborie (Laboratoire de physique corpusculaire, Caen, France) and Etienne Poirier (IUEM, Plouzané, France), members of the steering committee, for their advice and support throughout the project. Special thanks to Etienne Poirier for giving the idea of the project, trusting us and welcome us at IUEM in the last days of the project.

Authors are also very grateful to Pierre Guilleme teacher at Enssat, and Jean-Philippe Lesault, technician at Enssat, for their advice and technical support. 

The dissolved oxygen sensor project started in september 2025 with a first team of Enssat students.

## License
