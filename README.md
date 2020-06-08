# IoT_arduino_network

Files walkthrough:
1. Access_point_upload_in_ESP32_  : This code has to be uploaded in the wifi module (ESP32 development board)
2. node__upload_in_arduino__  : This code has to be uploaded in all nodes.
3. node__upload_in_arduino__  : This code has to be uploaded in Disaster management team's node. The diffrence between the previous code and this code is that, this code sends distance vector in the beginning.
4. GPSmap.py : It communicates with the DMT node via serial port and receives messages from DMS node, it then extracts GPS info from each message and plots the location into an HTML file
