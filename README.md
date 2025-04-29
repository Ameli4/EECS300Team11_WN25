# EECS300 Team 11 Final Demo Code
This is the code for Team 11's occupant counter. The code for this project assumes there are 2 ESP32 devices being used - one for the client, and one for the server. The client file, client_main.ino, does all interpretation and algorithms and transmits the data to the server-side ESP32. The "server" ESP32, or the ESP32 running the server_main.ino code, then reads this data and prints it on the Ardiuno IDE to display the number of people. This code also assumes that there is only one VL53L1X ToF sensor. I will now discuss each file in the repo.

## client_main.ino
This file is considered the main file for the client ESP32, and imports other helping libraries that take care of the logic and WiFi configurations.

## server_main.ino
This file is what the server side ESP32 uses to receive data sent by the client ESP32. Note that a zero will be outputted if the read value is less than or equal to zero.

## VL53L1X_Interface
This initializes the VL53L1X device for data interpretation and sets up a struct to load data into a grid of values corresponding to the pixel location of the Region of Interest (ROI) being scanned and its corresponding coordinates. It then takes the highest value and outputs it. Currently, the functions are set up to obtain 4 ROI's, each corresponding to a different section along the doorway.

## Scan_logic
This is where the counting algorithm lives. The code utilizes the VL53L1X_Interface to get a scan of the 4 ROI's (see VL53L1X_Interface).

## shared_variable & Wireless_Communication
These were provided to us to enable Wifi functionality.





