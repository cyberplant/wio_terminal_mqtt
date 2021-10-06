# wio_terminal_mqtt
Allow remote control of Wio Terminal via MQTT

This is a quick and dirty arduino program I did in less than 3 hours, to allow remote execution 
of commands via MQTT.

Some example files and the output on the Wio Terminal:

- [test1](tests/test1.txt) ![test1](/tests/test1.jpeg)
- [test2](tests/test2.txt) ![test2](/tests/test2.jpeg)
- [test3](tests/test3.txt) ![test3](/tests/test3.jpeg)

Using this library and Home Assistant I was able to build this simple screen to show status of different entities at my home:

- ![screen.jpeg](/screen.jpeg)

# Needed libraries
- Seeed Arduino rpcUnified
- Seeed Arduino rpcWiFi
- PubSubClient
- LIS3DHTR library - https://github.com/Seeed-Studio/Seeed_Arduino_LIS3DHTR
