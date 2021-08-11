# BB8_disco_droid
Firmware to support the Disco Droid BB-8 sound and light system from Jim on the BB-8 builders forum.

This version of the software is designed to run with the iPhone version of the app, and most likely will not work with the android version.

The iPhone commands do not match the original code and as such did not work with the code on the shipped version of the Disco Droid Module.

Bug Fixes in this version include:

 * Fixed a bug with the Module name not being updated
 * Fixed issues with the commands received from iPhone not matching the command parser in the code
 * Reconfigured the LED definitions to match the 3 per element config on the board
 * Updated the debug output (via serial monitor) to be clearer and prevent flooding the monitor

Hopefully this is useful to someone.
