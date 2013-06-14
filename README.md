BeerCooler
==========

Arduino app to cool beer with fans

Usage
-----
Displaying 0 to 9 means 60 to 69F, 0. to 9. means 70 to 79F
Press high button to switch to high limit setting. Press low button to switch to low limit setting.
If low limit < high limit, fans turn on when temperature > high limit and turn off when temperature < low limit
If low limit > high limit, fans turn on when temperature < high limit and turn off when temperature > low limit

Components
-------------------

Some of the components used:

 - DS18B20 https://www.sparkfun.com/products/11050
 - 2 12V 120mm PC fans
 - 2 transistors NPN 2N3904 https://www.sparkfun.com/products/521
 - optoisolator https://www.sparkfun.com/products/314

Libraries
---------

 - https://github.com/fgaudin/Led7Seg
 - https://github.com/fgaudin/PushButton
