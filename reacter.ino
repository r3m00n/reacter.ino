// Pin-Definitionen für Tasten und LEDs
const uint8_t P1ButtonPin = 12;
const uint8_t P2ButtonPin = 2;
const uint8_t P1LedPin = 8;
const uint8_t P2LedPin = 4;
const uint8_t RedPin = 11;
const uint8_t GreenPin = 10;
const uint8_t BluePin = 9;

// Helligkeit für die RGB-LED
const uint8_t RGB_Brightness = 40;

// Variablen für das Spiel
unsigned long goTime, currentMillis, lastButtonPress;
bool playing, go;
uint8_t p1score, p2score;
uint8_t gameMode; // 0: nur grün, 1: alle Farben
uint8_t signal;   // Farbe der Led die angeht. Wenn grün: gedrücken!

// Funktion zum Ausschalten der RGB-LED
void turnOffRgb()
{
  digitalWrite(RedPin, 0);
  digitalWrite(GreenPin, 0);
  digitalWrite(BluePin, 0);
}

// Funktion zum Zuordnen der Pin-Nummer zu einer Farbe
uint8_t getPinByColor(uint8_t color)
{
  switch (color)
  {
  case 1:
    return RedPin;
  case 2:
    return GreenPin;
  case 3:
    return BluePin;
  }
}

// Funktion zur Auswahl des Spielmodus durch die Spieler
void selectGameMode()
{
  bool gameSelected = false;
  unsigned long previousMillis = 0;
  uint16_t interval = 500;
  uint8_t ledState, ledColor, ledPin;

  while (!gameSelected)
  {
    currentMillis = millis();

    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      ledState = ledState == 0 ? RGB_Brightness : 0;

      if (gameMode == 0)
      {
        ledPin = GreenPin;
      }
      else if (gameMode == 1)
      {
        if (ledState == RGB_Brightness)
        {
          ledColor = ((ledColor + 1) % 3) + 1;

          ledPin = getPinByColor(ledColor);
        }
      }
      if (ledState == 0)
      {
        turnOffRgb();
      }
      else
      {
        analogWrite(ledPin, ledState);
      }
    }

    // Spieler 1 wechselt den Spielmodus
    if (digitalRead(P1ButtonPin) == 0 && millis() >= lastButtonPress + 500)
    {
      lastButtonPress = millis();
      gameMode = (gameMode + 1) % 2;
    }
    // Spieler 2 bestätigt den Spielmodus und startet das Spiel
    if (digitalRead(P2ButtonPin) == 0)
    {
      gameSelected = true;
      turnOffRgb();

      if (gameMode == 0)
      {
        // Grünes LED-Blinken für Spielmodus 0
        for (int i = 0; i < 3; ++i)
        {
          delay(250);
          analogWrite(GreenPin, RGB_Brightness);
          delay(250);
          analogWrite(GreenPin, 0);
        }
      }
      else if (gameMode == 1)
      {
        // RGB-LEDs blinken für Spielmodus 1
        delay(250);
        analogWrite(RedPin, RGB_Brightness);
        delay(250);
        analogWrite(RedPin, 0);
        delay(250);
        analogWrite(GreenPin, RGB_Brightness);
        delay(250);
        analogWrite(GreenPin, 0);
        delay(250);
        analogWrite(BluePin, RGB_Brightness);
        delay(250);
        analogWrite(BluePin, 0);
      }
    }
  }
  delay(1000);
}

// Funktion für die Startsequenz des Spiels
void startSequence()
{
  analogWrite(RedPin, RGB_Brightness);
  delay(300);
  digitalWrite(RedPin, 0);
  delay(300);
  analogWrite(RedPin, RGB_Brightness / 1.25);
  analogWrite(GreenPin, RGB_Brightness);
  delay(300);
  digitalWrite(RedPin, 0);
  digitalWrite(GreenPin, 0);
  delay(300);
  analogWrite(GreenPin, RGB_Brightness);
  delay(300);
  digitalWrite(GreenPin, 0);
  Serial.println("Spiel gestartet");
}

// Funktion für schnelles Blinken einer LED
void fastBlink(uint8_t ledPin)
{
  for (int i = 0; i < 7; ++i)
  {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, 0);
    delay(100);
  }
}

// Funktion zum Überprüfen der Spieler-Eingabe
void checkPlayerInput()
{
  if (digitalRead(P1ButtonPin) == 0)
  {
    digitalWrite(P1LedPin, HIGH);
    playing = false;

    if (go && signal == 2)
    {
      p1score++;
      Serial.println("P1 pressed first");
      delay(1000);
    }
    else
    {
      p2score++;
      Serial.println("P1 pressed too early");
      fastBlink(P1LedPin);
    }
    digitalWrite(P1LedPin, 0);
    delay(500);
  }
  if (digitalRead(P2ButtonPin) == 0 && playing)
  {
    digitalWrite(P2LedPin, HIGH);
    playing = false;
    if (go && signal == 2)
    {
      p2score++;
      Serial.println("P2 pressed first");
      delay(1000);
    }
    else
    {
      p1score++;
      Serial.println("P2 pressed too early");
      fastBlink(P2LedPin);
    }
    digitalWrite(P2LedPin, 0);
    delay(500);
    signal = 0;
  }
}

// Funktion zur Anzeige der Punktestände
void showScore()
{
  Serial.print("P1: ");
  Serial.print(p1score);
  Serial.print(" P2: ");
  Serial.println(p2score);

  uint8_t highestScore = p1score > p2score ? p1score : p2score;

  for (int i = 1; i <= highestScore; i++)
  {
    if (p1score >= i)
    {
      digitalWrite(P1LedPin, HIGH);
    }
    if (p2score >= i)
    {
      digitalWrite(P2LedPin, HIGH);
    }
    delay(highestScore > 5 ? 300 : 500);
    digitalWrite(P1LedPin, 0);
    digitalWrite(P2LedPin, 0);
    delay(highestScore > 5 ? 150 : 250);
  }
}

// Einrichtungsfunktion für den Arduino
void setup()
{
  Serial.begin(9600);
  pinMode(P1ButtonPin, INPUT_PULLUP);
  pinMode(P2ButtonPin, INPUT_PULLUP);
  pinMode(P1LedPin, OUTPUT);
  pinMode(P2LedPin, OUTPUT);
  pinMode(BluePin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(RedPin, OUTPUT);

  // Auswahl des Spielmodus durch die Spieler
  selectGameMode();
}

// Hauptprogrammschleife für den Arduino
void loop()
{
  // Startsequenz des Spiels
  startSequence();

  // Zufällige Wartezeit bis zum Startsignal
  goTime = millis() + random(100, 8000);
  playing = true;

  // Hauptschleife während des Spiels
  while (playing)
  {
    go = millis() >= goTime;
    if (go)
    {
      // Festlegen der Signal-LED-Farbe entsprechend dem Spielmodus
      if (gameMode == 0)
      {
        signal = 2; // grün
      }
      else if (gameMode == 1)
      {
        // Zufällige Auswahl der Farbe für Spielmodus 1
        if (signal == 0)
        {
          signal = random(1, 4); // Zufall R,G,B
        }
        // Ausschalten der Farbe nach einer bestimmten Zeit
        if (millis() >= goTime + 2500)
        {
          goTime = millis() + random(100, 8000);
          analogWrite(getPinByColor(signal), 0);
          signal = 0;
        }
      }
      // Einschalten der Signal-LED
      analogWrite(getPinByColor(signal), RGB_Brightness);
    }
    // Überprüfung der Spieler-Eingabe
    checkPlayerInput();
  }

  // Ausschalten der Signal-LED und Anzeige der Punktestände
  turnOffRgb();
  signal = 0;
  showScore();
  delay(1000);
}
