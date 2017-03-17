# arduinobluebot

This is an Arduino implementations of the Bluetooth-controlled rover.  Check out the Raspberry Pi version [here](https://github.com/vrestivo/pibluebot_public).  The rover is controlled by [this Arndoid App](https://github.com/vrestivo/bluebotpublic).

## Bluetooth
The rover uses an HC-06 Bluetooth module, which can be found [here](https://www.amazon.com/Pass-Through-Communication-Compatible-Atomic-Market/dp/B00TNOO438).  The module is connected to the Arduino through the software serial interface.  The hardware serial is used for serial monitor debugging.

__NOTE: The HC-06 and Arduino run at 3.3 volts and 5 volts respectively.  A logic level converter, or voltage divider should be implemented. Check out this [example](http://www.martyncurrey.com/arduino-and-hc-06-zs-040/).__  

## Diagnostics Segment
There is a diagnostics section inside the Arduino sketch.  Feel free to uncomment it for debugging purposes.  The segment checks if the HC-06 is operational by sending an `AT` command to it.  Under normal conditions the modules should respond with "OK."

NOTE: the diagnostic will not work if HC-06 is already connected to another Bluetooth device, since it automatically switched to the pass-through mode.

