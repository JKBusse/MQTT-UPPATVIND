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
