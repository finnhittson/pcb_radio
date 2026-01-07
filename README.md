# PCB Radio

PCB radio is an FM receiver operating from 88MHz to 108MHz. It uses the Si4735 radio IC and is controlled with PIC32. PCB radio initially started as my final project for a class called Board Level Design (EE256) offered at Stanford which I took during the 2023 fall quarter. 

<p align="center">
  <img src="images/pcb_radio.JPG" width="400">
</p>

## Eletrical

The radio itself is the Si4735 made by Skywork Solutions. This IC is responsible for all signal demodulation and filtering. It is controlled with a PIC32 through an I2C bus. Two rotary encoders control the volume and frequency selection. The buttons on the volume rotary encoder mutes and unmutes the audio. The button on the frequency rotary encoder seeks the next station in the direction of the last frequency change. There is no onboard amplifier or speaker so the audio is passed to an AUX output. Lastly there is a 128x64 OLED display made by Adafruit that is used to display volume and frequency selected.

<p align="center">
  <img src="images/schematic" width="400">
</p>

## Software

Software includes all code used to program and run PCB Radio's microcontroller and other hardware.

## Case
