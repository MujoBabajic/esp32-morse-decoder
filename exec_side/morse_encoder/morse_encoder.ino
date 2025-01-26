#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "jomu"; // Name of wifi network
const char* password = "123qweasd"; // Network password

WebServer server(80); // Server on port 80

#define BUZZER_PIN 12
#define RED_DIODE_PIN 25
#define GREEN_DIODE_PIN 13
#define BUZZER_FREQ 800 // Hz
#define SHORT_BEEP 20 // Miliseconds
#define LONG_BEEP 200

struct Morse {
  char letter;
  const char *code;
};

Morse morseAlphabet[] = {
  {'A', ".-"},   {'B', "-..."}, {'C', "-.-."}, {'D', "-.."},  {'E', "."},
  {'F', "..-."}, {'G', "--."},  {'H', "...."}, {'I', ".."},   {'J', ".---"},
  {'K', "-.-"},  {'L', ".-.."}, {'M', "--"},   {'N', "-."},   {'O', "---"},
  {'P', ".--."}, {'Q', "--.-"}, {'R', ".-."},  {'S', "..."},  {'T', "-"},
  {'U', "..-"},  {'V', "...-"}, {'W', ".--"},  {'X', "-..-"}, {'Y', "-.--"},
  {'Z', "--.."}, {'1', ".----"},{'2', "..---"},{'3', "...--"},{'4', "....-"},
  {'5', "....."},{'6', "-...."},{'7', "--..."},{'8', "---.."},{'9', "----."},
  {'0', "-----"},{' ', " "}
};

String convertToMorse(String text) {
  String morseMessage = "";
  text.toUpperCase();
  
  for (int i = 0; i < text.length(); i++) {
    char c = text[i];
    for (Morse m : morseAlphabet) {
      if (m.letter == c) {
        morseMessage += m.code;
        morseMessage += " "; // Space between letters
        break;
      }
    }
  }
  return morseMessage;
}

void playMorse(String morse) {
  // LED and buzzer are active for 2 seconds to indicate that the morse is ready
  digitalWrite(GREEN_DIODE_PIN, HIGH);
  tone(BUZZER_PIN, BUZZER_FREQ);
  delay(2000);
  noTone(BUZZER_PIN);
  digitalWrite(GREEN_DIODE_PIN, LOW);
  delay(1000);

  // Playing the morse code
  for (int i = 0; i < morse.length(); i++) {
    char symbol = morse[i];
    if (symbol == '.') {
      tone(BUZZER_PIN, BUZZER_FREQ);
      digitalWrite(GREEN_DIODE_PIN, HIGH);
      delay(SHORT_BEEP);
      noTone(BUZZER_PIN);
      digitalWrite(GREEN_DIODE_PIN, LOW);
    } else if (symbol == '-') {
      tone(BUZZER_PIN, BUZZER_FREQ);
      digitalWrite(GREEN_DIODE_PIN, HIGH);
      delay(LONG_BEEP);
      noTone(BUZZER_PIN);
      digitalWrite(GREEN_DIODE_PIN, LOW);
    }
    delay(100); // Pause between signals
  }
}

void handleReceive() {
    if (server.method() == HTTP_OPTIONS) {
        // Respond to preflight request
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(204);
        return;
    }

    if (server.method() == HTTP_POST) {
        String message = server.arg("plain");
        Serial.print("JSON body: ");
        Serial.println(message);

        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, message);
        if (error) {
            Serial.println("JSON Parsing failed!");
            server.send(400, "text/plain", "Invalid JSON");
            return;
        }

        String receivedText = doc["message"].as<String>();
        Serial.print("Received message: ");
        Serial.println(receivedText);

        // Send response with CORS headers
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "text/plain", "Message received!");

        String morseMessage = convertToMorse(receivedText);
        Serial.print("Morse: ");
        Serial.println(morseMessage);

        playMorse(morseMessage);
    } else {
        server.send(405, "text/plain", "Method Not Allowed");
    }
}

void handleOptions() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(204);
}

void setup() {
    Serial.begin(115200);

    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RED_DIODE_PIN, OUTPUT);
    pinMode(GREEN_DIODE_PIN, OUTPUT);

    // WiFi connection setup
    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.println(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected to WiFi!");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());

    // Run handleReceive function when a POST request arrives to /receive endpoint
    server.on("/receive", HTTP_POST, handleReceive);
    server.on("/receive", HTTP_OPTIONS, handleOptions); // Handle CORS preflight

    server.begin();
}

void loop() {
    server.handleClient();
}