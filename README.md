# DaVinciBot - French Robotics Cup

![Logos ESILV, Coupe de France de Robotique, DaVinciBot](/images/logos_readme.png)

Welcome to the repository of DaVinciBot for the French Robotics Cup (*Coupe de France de Robotique*)!

[Link to the competition's website](https://www.coupederobotique.fr/edition-2022/le-concours/)


# Repository structure

This repository includes 3 main directories:
```bash
├─ core
├─ firmware
└─ python_packages
```

## `core`

The `core` directory includes programs running on the Raspberry Pi cards of the robots. These files are mainly, but not exclusively, written in Python.

It can also contains utilities or debugging programs, like `core/visualizer` that are made to run on a separate computer. As the rules of the competition say, the robots must be fully autonomous and theses utilities are not required to start the robots.

## `firmware`

This directory contains C/C++ programs that are compiled and sent to the *Teensy* or *Arduino* boards. Each folder contains a *PlatformIO* project that can easily be opened with Visual Studio Code for example.

It also contains the `firmware/librairies` folder that contains the libraries used by multiple boards or projects.

## `python_packages`

The `python_packages` directory contains the Python librairies used for this project, including *intercom* (see *Architecture below*) or maths and pygame utils.

During the competition, robots will try to score points by executing *actions*: this is the smallest thing a robot can do. These actions are grouped together in *missions* also defined in this directory.

# Hardware

Each robot contains:
- 2 motors ([Worm Gearmotor, Ratio 100:1, with encoder](https://www.motionco.co.uk/worm-gearmotor-ratio-100-1-encoder.html)
- [1 Raspberry Pi 4 Model B](https://www.raspberrypi.org/products/raspberry-pi-4-model-b/)
- [1 Teensy 4.1 boards](https://www.pjrc.com/store/teensy41.html)
- ...specific components like servos.

# Architecture

This year, our team decided to drop ROS (*Robot Operating System*) for a custom system. ROS may be the *state of the art* in its domain, we found it was a white elephant.

We then decided to use *intercom* as our communication system.
It does not replace every feature of ROS, but they might be added in other packages of the project.

It works by using *UDP multicast sockets* to send and receive messages. Currently, *Teensy* boards support only the communication via a USB cable to a Raspberry Pi running a *proxy*.

**TODO:** Include a schematic of the architecture.
