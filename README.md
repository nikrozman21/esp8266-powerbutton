# PC case power button controller:

### Brief explanation
This project is made to imitate the functions of a PC case power button.
It can send a short button press (200 ms), a long press (10s) or to hold it until you turn it back off.

Additionally it probes if the power LED is on or off.

Upon boot, it connects to set WiFi AP and displays its IP in serial console (baud 115200).
After that, a web server is set up on port 80.


### Variable explanation:
- ssid: WiFi access point name,
- password: WiFi password,
- espName: Device identifier,
- outputPin: Power header pin,
- analogInPin: LED voltage probe pin,
- ledOnTreshold: Treshold between LED on/off.

### Debugging:
You may use /update in your browser to get raw LED pin data.
This can be useful to determine the on/off treshold.
