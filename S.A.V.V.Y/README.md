S.A.V.V.Y. - Semi-Autonomous Versatile Vihicle, Yielding

Project source code:

https://github.com/jonathanwoolf/CS122A/tree/master/S.A.V.V.Y

Author and contribution list: Jonathan Woolf 

Any bugs may be reported to jwool003@ucr.edu

How to run S.A.V.V.Y.:

* Use the left joystick to move forwards and backwards
* Use the right joystick to turn, using both joysticks allows you to perform a more gradual turn as you move forward
* The vehicle will stop to avoid obstacles

Summary:

* The goal of S.A.V.V.Y. is to avoid incoming obstacles
* When there is no risk of a crash occuring, you can control S.A.V.V.Y. as you would any other RC car

Complexity:

* USART communication between the controller and car by way of RF radio waves
* Redundancy and parity bit to ensure only the proper signals are acted upon by S.A.V.V.Y.
* Joysticks to allow complete control of the vehicle
* An 8x8 RGB LED matrix was used to test the controller (RF transmission and joysticks)
* Ultrasonic sensor to detect obstacles
* Two H-bridges and 4 DC motors to provide motion

Bugs:

* When the controller is turned off, S.A.V.V.Y. gets overrun with noise from the motors that causes it to move on it's own. 
  This cannot occur when the remote is on as it is constantly transmitting a stronger signal than the noise 

Demo Videos:

https://www.youtube.com/watch?v=caX4SWuiUFY

https://www.youtube.com/watch?v=-sieFKYgOlw
