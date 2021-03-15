# SomethingAwesomeCOMP6841

For my something awesome, I trying to mimic a USB rubber Ducky. A rubber Ducky is a usb device that presents itself as a HID keyboard device to the target pc and injects malicious keystrokes on the target system. 

## Part List
1) Arduino Pro Micro
2) DIP switch
3) Micro SD card reader for Arduino.
4) Breadboard
5) Jumper Cables

![image](https://user-images.githubusercontent.com/80434391/111103239-18129800-85a2-11eb-94be-062867abc9d7.png)

The file arduinoScript.ino containes code that is flashed on the arduino pro micro and runs every time it is connected to PC. The code depending on the switch configuration executes the relevant script stored in the micro SD card.

