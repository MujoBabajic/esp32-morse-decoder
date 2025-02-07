# ESP32 Morse Encoder

ESP32 Morse Encoder is an IoT project used for sending text messages from one device, which are then converted into Morse code and played on another device.

The frontend page was designed in a plain HTML/CSS/JS, Arduino IDE for programming the ESP32 module, and Firebase for realtime database support.

# Parts

The project consists of 3 physical parts, which include:

- ESP32 module
- LED
- Passive buzzer

![image](https://github.com/user-attachments/assets/0566b1e9-be0b-4694-ac21-b3cd63f0d63e)

The buzzer is connected to pin 12, and the LED to pin 13.

# How it works

1. Connecting to Wi-Fi and Firebase
- The module connects to the Wi-Fi connection with the provided credentials
- Then, it connects to Firebase with the provided auth database
2. Listening for changes on Firebase
- After connecting to the database, the module checks every 5 seconds if there is a new message in the database
3. Entering the text
- The user enters the text message on the web page
- Input validation (only alphanumeric characters are allowed, and the user cannot send an empty message)
4. Sending the message to Firebase
- The message is then sent to Firebase, along with the message timestamp
5. Finding a new message and playing it 
- The ESP32 module finds a new message and stores it locally
- The original alphanumeric message is then converted into Morse code
- The buzzer and the diode activate for 2 seconds to signal that the message is ready to play
- After the signal, the Morse code is played

