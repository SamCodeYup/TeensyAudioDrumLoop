This is the code for a drum loop that is 4 beats with 16 individual subdivisions. It is used on a Teensy 4.0 microcontroller along with its corresponding audio 
shield for quick and easy audio output. 

When using the Teensy and audio shield, be sure NOT to put the devices together(if header pins have been soldered on already). Instead one should place each device
in a breadboard and wire up only the necessary pins.
      -    those are pins: 7, 8, 18, 19, 20, 21, 23   |   each pin on the Teensy is to be lined up with the same number pin on the audio shield
      -    for example, pin 7 on the teensy would be connected to pin 7 on the audio shield, and same goes for every other pin

Furthermore, be sure that you are properly powering and grounding each of the ports on the audio shield, since the Teensy is only powered via microUSB


Otherwise mess around with it and have some fun! Lots more to be done with this
