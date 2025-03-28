# ENG3039 TDP3 Line Following Robot

The robot is designed to traverse a predefined track through use of a line-following algorithm and infrared sensor input being continuously collected in real time. The sensors detect the contrast between the black line and the white floor and adjust the course accordingly. When a barcode is encountered along the track, the robot must scan the card and respond correctly, with each barcode supposed to enforce a pause in movement for a predetermined amount of time. 

This repository contains the source code for this project.

## Flowchart

<img width="400" alt="image" src="https://github.com/user-attachments/assets/cb22ee61-8af6-40d1-b33b-1d2494637ffb" />

## PID

#### Error Mapping of the Proposed PID Line-following Algorithm

| Sensor Input | Binary | Decimal | Output                  |
|--------------|--------|---------|--------------------------|
| WBW          | 101    | 5       | `error = error * 0.5`    |
| BBW          | 001    | 1       | `error = -1`             |
| BWW          | 011    | 3       |                          |
| WWB          | 110    | 6       | `error = 1`              |
| WBB          | 100    | 4       |                          |
| WWW          | 111    | 7       | `error = error`          |

<img width="400" alt="image" src="https://github.com/user-attachments/assets/d5b1f3fe-024f-403a-abf4-f161accf90cf" />

## Barcode Detection

<img width="468" alt="image" src="https://github.com/user-attachments/assets/1b61d44f-77a4-499c-b35a-4d59318a51e5" />

<img width="468" alt="image" src="https://github.com/user-attachments/assets/334a762d-091b-4ba3-a1c6-f0d728f0d7b2" />

## Bluetooth Communication and Remote Control

### Table X: Remote Control

| Input Control Mode                     | Key   | Description                                          |
|---------------------------------------|-------|------------------------------------------------------|
| Free Control Mode/Memory Updating Mode| W     | Accelerate.                                          |
|                                       | S     | Deaccelerate                                         |
|                                       | A     | Turn left.                                           |
|                                       | D     | Turn right.                                          |
|                                       | C     | Reset the speed difference to zero.                 |
| Free Control Mode/Memory Updating Mode| U     | Enable/Disable Memory Updating Mode                 |
| Free Control Mode/Memory Loading Mode | L     | Enable/Disable Memory Loading Mode                  |
| Free Control Mode                     | F     | Enable/Disable Line-following PID Mode              |
| Line-following PID Mode               | [1~8] | For test only. Tune parameters of PID algorithm.    |
| All Modes                             | Q     | Stop and quit.                                      |

## Building and running

1. Connect a USB cable between the USB port on the board and the host computer.
2. Build the project with [Keil Studio Cloud](https://studio.keil.arm.com).
3. Flash the program into the MCU.

Your PC may take a few minutes to compile your code.

Alternatively, you can manually copy the binary to the board, which you mount on the host computer over USB. The binary is located at: `./bin/TDP3_software.KL25Z.bin`</br>
