
The following link is a demo of two Featherboards talking via Bluetooth:

https://drive.google.com/file/d/12fGEGPmIIOJGqEOBqlB8obmJoS6Mp4RO/view?usp=sharing

The display is driven by the server; the client discovers it, connects, and
sends an updated value every time its input button is pressed.  The server
displays the values received.

While simple, this demo shows initializing the display, setting up a Bluetooth "BLE" server
with a unique service and "characteristic" (settable property) and the client side
discovery and interaction with that service. The next steps are to create a custom API to 
encapsulate the networking to make writing games simpler.
