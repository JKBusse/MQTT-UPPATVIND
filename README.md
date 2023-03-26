# MQTT-UPPATVIND
Hacking the Ikea UPPÅTVIND Air Purifier.

U1 Pinout
```
5V             -|°---|-  Motor
Filter LED     -|    |-  GND
Speed LED      -|    |-  Motor
Filter Button  -|----|-  Speed Button
```
Test Points
```
TP2  GND
TP3  5V
TP4  Speed Button
TP5  Filter Button
TP6  Filter LED
TP7  Speed LED
TP9  Motor PWM
TP10 Motor FG
 ```
 Connection TP > Wemos
 ```
TP2 > 5V
TP3 > GND
TP4 > D1 (5)
TP6 > D2 (4)
TP7 > A0
```
![tempImageywR3NJ](https://user-images.githubusercontent.com/48227459/227752421-3867c94e-72cf-418e-be4f-38cd0624e1b2.gif)


Homebridge Config
```
{
            "accessory": "mqttthing",
            "type": "airPurifier",
            "name": "UPPÅTVIND",
            "url": "mqtt://YOURMQTTSERVER:1883",
            "topics": {
                "setActive": "/home/data/UPPATVIND/setActive",
                "getActive": "/home/data/UPPATVIND/getActive",
                "getCurrentAirPurifierState": "/home/data/UPPATVIND/getCurrentAirPurifierState",
                "setTargetAirPurifierState": "/home/data/UPPATVIND/setCurrentAirPurifierState",
                "getTargetAirPurifierState": "/home/data/UPPATVIND/getTargetAirPurifierState",
                "getFilterChangeIndication": "/home/data/UPPATVIND/getFilterChangeIndication",
                "getRotationSpeed": "/home/data/UPPATVIND/getRotationSpeed",
                "setRotationSpeed": "/home/data/UPPATVIND/setRotationSpeed"
            },
            "integerValue": "true"
        }
