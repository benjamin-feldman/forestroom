# forestroom

This is the documentation for the `forestroom` project, an installation combining sound and robotics, created by Alexandre Contini and Juliette Gelli. It is located at the Fonds régional d'art contemporain.
## Project Description

The main feature of the installation is a one-hour audio recording of fake rainforest sounds, made with small electronic devices. The project aims to create an immersive experience for visitors, simulating the atmosphere of a rainforest through sound.

The system involves multiple synchronous OSC (Open Sound Control) signals that are sent over WiFi to ESP32 boards, which then control 12 motors in the room.


## Files

- `esp_generique_dc.ino`: Controls a DC motor.
- `esp_generique_bounce.ino`: Controls a Stepper motor.

## Stepper Modes

The Stepper motor has three modes:

- **constant**: The stepper rotates at a constant speed received from Vezer.
- **bounce**: The stepper oscillates with a certain speed and amplitude received from Vezer.
- **disabled**: The stepper is unlocked to prevent overheating.

## Vezer Configuration

In Vezer, for the ESP10 steppers, it is important not to interpolate the points to avoid a "queueing" phenomenon that desynchronizes Vezer and the motor.

## ESP Configuration

The following table shows the configuration of the ESP boards used in the project:

| ESP | Function    | Motor Type | Number of Motors | IP         |
| --- | ----------- | ---------- | ---------------- | ---------- |
| 10  | Pluie       | Stepper    | 1                | 10.10.10.10 |
| 11  | Tonnerre    | DC         | 1                | 10.10.10.11 |
| 12  | Billes A    | DC         | 2                | 10.10.10.12 |
| 13  | Billes B    | DC         | 2                | 10.10.10.13 |
| 14  | Tonnerre    | DC         | 1                | 10.10.10.14 |
| 15  | Tonnerre    | DC         | 1                | 10.10.10.15 |
| 16  | Feuilles    | Stepper    | 1                | 10.10.10.16 |
| 17  | Grenouilles | Stepper    | 1                | 10.10.10.17 |
| 18  | Ailes       | Stepper    | 1                | 10.10.10.18 |
| 19  | Vent        | Stepper    | 1                | 10.10.10.19 |


## TODO (Software)

- Create a battery of tests for the stepper motor to verify edge cases.
- In the **constant** mode, remove the `for` loop and perform one step per loop.
- Continue working on the `random_variation` branch.
- Create a composition for "global stop".

## Random Variations

Random variations are only applicable to the Feuilles and Ailes motors. The values are in percentages, representing the probability of a change in amplitude, delay, or offset.

- `int probsAmplitude = 80;`
- `int probsDelay = 80;`
- `int probsOffset = 30;`

