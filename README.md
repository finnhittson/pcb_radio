# PCB Radio

PCB radio is an FM receiver operating from 88MHz to 108MHz. It uses the Si4735 radio IC and is controlled with PIC32. PCB radio initially started as my final project for a class called Board Level Design (EE256) offered at Stanford which I took during the 2023 fall quarter. 

<p align="center">
  <img src="images/pcb_radio.png" width="600">
</p>

## Eletrical

The radio itself is the Si4735 made by Skywork Solutions. This IC is responsible for all signal demodulation and filtering. It is controlled with a PIC32 through an I2C bus. Two rotary encoders control the volume and frequency selection. The buttons on the volume rotary encoder mutes and unmutes the audio. The button on the frequency rotary encoder seeks the next station in the direction of the last frequency change. There is no onboard amplifier or speaker so the audio is passed to an AUX output. Lastly there is a 128x64 OLED display made by Adafruit that is used to display volume and frequency selected.

<p align="center">
  <img src="images/schematic.png" width="600">
</p>

## Software

The code for this was written in C and programmed onto the PIC32 using the MPLABX IDE with the SNAP programmer. The rotary encoders operate on an interrupt basis to increment or decrement a counter. The updated value is passed to the Si4735 IC which correspondingly changes the volume or frequency. The frequency and volume buttons are periodically checked and the mute/seek commands are also sent along to the Si4735 IC.  
  
The dispaly is also controlled using the I2C bus. The display shows three lines being the title, volume, and frequency. Due to speed issues, only parts of the display are updated that need to changes while the remaining pixels remain unchanged. For example, when increasing the volume the only pixels that change are the pixels that compose the volume digits. The title, "vol: ", and frequency line all stay the same. The process for updating the frequency is the exact same. All characters are an 8x8 bitmapping.

## Case

The case was designed using Autodesk Fusion. It consists of three parts. The bottom case, top case, and button hat. The bottom case has heatset inserts to secure the pcb to the case frame. The top case has cutouts for the rotary encoders, display and power switch. The swtich had is needed to bridge the gap between the pcb and the height of the top case. 
<p align="center">
  <img src="images/cad_assembly.png" width="600">
</p>
