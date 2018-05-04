# Bluetooth-LED-Costume
Modification of Arduino's Bluefruit BLE controller code to control different animation states of LEDs in a costume. 

This code goes through four different animation states. 
The description of the project and system overview is included below. Images and the description can also be found at: http://sophiavuw.wixsite.com/portfolio/fairy-lights-costume

PROJECT DESCRIPTION
In the spring of 2018 I had the opportunity to collaborate with the costume shop at the University of Minnesota Department of Theater Art & Dance on their production of A Midsummer Night's Dream. For this project I was tasked with programming lights for Tatiana's costume and crown that could be controlled remotely via Bluetooth. The costume lighting system is comprised of an Arduino with a Bluetooth shield, two strings of fairy lights for the skirt, a string of lights for the crown, and a small power bank.

========================================================================================================================================
LIGHTS 
The director wanted it to look like there were fireflies, which meant that the LEDs had to alternate and all the lights could not be illuminated at the same time. The fairy lights that I chose came with alternating diode configuration. This was desired because it made it possible to use an existing string of fairy lights, and only two input channels on the Arduino, to achieve a fluttering effect. 

I disconnected the two strings of fairy lights from their battery packs and soldered them together in parallel before attaching them to female 3.5 mm audio connectors. This type of connector was chosen to make it easy to connect and disconnect the lights from the Arduino. The Arduino had the corresponding male connectors.

To facilitate wig changes, an audio extender cable was channeled to the back of the costume to create another easy junction. The lights on the wig were connected to a male connector.

========================================================================================================================================
CONTROL 
To control the lights, I used an Arduino Uno with a Bluefruit Bluetooth shield. The fairy lights were programmed to 4 different settings, slow fading, medium fading, twinkle, and off. The different setting were used to create an effect of the lights reacting to the character's emotions. I created an app that would serve as a controller for the lights. 

Since I tied two strings of fairy lights together for the skirt, the current from the Arduino pins was not enough to power the lights.  To get the 5V supply without current limitation, a transistor would be necessary to serve as the power gait for the lights. Since the lights were being controlled by switching which lead the power was going to, simply adding one transistor would not be enough. I decided to use an H-bridge motor controller (L293D) to control the power going to each lead.

Both the lights for the crown and skirt were connected to the same Arduino so that they would be synchronized.

In addition to the lights for Tatiana's costume, I also made lights for Oberon's crown. The lights on this crown were meant to stay on the entire performance and the system had to be small. Instead of an Arduino Uno, I used an Arduino nano. I hollowed out one of the battery holders that I took off the lights and cut a hole out the back for the male 3.5mm connector and the mini USB that powers the nano. I routed the signal wire from the male connector through the switch of the battery holder to make it easy to turn off and on the lights. 
