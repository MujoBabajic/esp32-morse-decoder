#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <project_config.h>

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

unsigned long lastCheckTime = 0;
String lastMessage = "";  // Store the last processed message

#define BUZZER_PIN 12
#define GREEN_DIODE_PIN 13
#define BUZZER_FREQ 800  // Hz
#define SHORT_BEEP 20  // Milliseconds
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
  {'0', "-----"},{' ', "/"}
};

String convertToMorse(String text) {
  String morseMessage = "";
  text.toUpperCase();
  
  for (int i = 0; i < text.length(); i++) {
    char c = text[i];
    for (Morse m : morseAlphabet) {
      if (m.letter == c) {
        morseMessage += m.code;
        morseMessage += " ";  // Space between letters
        break;
      }
    }
  }
  return morseMessage;
}

void playMorse(String morse) {
  digitalWrite(GREEN_DIODE_PIN, HIGH);
  tone(BUZZER_PIN, BUZZER_FREQ);
  delay(2000);
  noTone(BUZZER_PIN);
  digitalWrite(GREEN_DIODE_PIN, LOW);
  delay(1000);

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
    delay(100);  // Pause between signals
  }
}

void checkFirebaseForNewMessage() {
  if (Firebase.RTDB.getJSON(&fbdo, "/")) {  // Fetch entire JSON structure
    Serial.println("Received JSON from Firebase:");
    Serial.println(fbdo.to<FirebaseJson>().raw());  // Print entire JSON for debugging

    FirebaseJson json = fbdo.to<FirebaseJson>();

    FirebaseJsonData jsonData;
    String latestKey = "";
    String latestTimestamp = "";
    String latestMessage = "";

    size_t count = json.iteratorBegin();
    Serial.print("Number of entries: ");
    Serial.println(count);

    for (size_t i = 0; i < count; i++) {
      int type = 0;
      String key, value;
      json.iteratorGet(i, type, key, value);

      Serial.print("Checking key: ");
      Serial.println(key);

      FirebaseJsonData messageData;
      FirebaseJsonData timestampData;

      // Extract the message and timestamp
      if (json.get(messageData, key + "/message") && json.get(timestampData, key + "/timestamp")) {
        Serial.print("Found message: ");
        Serial.println(messageData.stringValue);
        Serial.print("Found timestamp: ");
        Serial.println(timestampData.stringValue);

        // Compare timestamps to find the latest message
        if (timestampData.stringValue > latestTimestamp) {
          latestTimestamp = timestampData.stringValue;
          latestMessage = messageData.stringValue;
          latestKey = key;
        }
      }
    }
    json.iteratorEnd();

    if (latestMessage != "" && latestMessage != lastMessage) {
      Serial.print("New message received: ");
      Serial.println(latestMessage);

      lastMessage = latestMessage;

      String morseCode = convertToMorse(latestMessage);
      Serial.print("Converted to Morse: ");
      Serial.println(morseCode);

      playMorse(morseCode);
    }
  } else {
    Serial.print("Firebase error: ");
    Serial.println(fbdo.errorReason());
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_DIODE_PIN, OUTPUT);

  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to ");
  Serial.println(SSID);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\nConnected to ");
  Serial.println(SSID);

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Connected to Firebase");
    signupOK = true;
  } else {
    Serial.println("Firebase Sign-Up Failed");
    Serial.println(fbdo.errorReason());
  }

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && millis() - lastCheckTime > 5000) {  // Check Firebase every 5 seconds
    lastCheckTime = millis();
    checkFirebaseForNewMessage();
  }
}
