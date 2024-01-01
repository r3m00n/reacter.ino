const uint8_t P1ButtonPin = 2;
const uint8_t P2ButtonPin = 3;
const uint8_t P1LedPin = 8;
const uint8_t P2LedPin = 9;
const uint8_t RedPin = 10;
const uint8_t GreenPin = 11;
const uint8_t BluePin = 12;

unsigned long goTime, currentMillis;
bool playing, go;
uint8_t p1score, p2score;

void startSequence()
{
  analogWrite(RedPin, 25);
  delay(250);
  digitalWrite(RedPin, LOW);
  delay(250);
  // analogWrite(BluePin, 25);
  // delay(250);
  // digitalWrite(BluePin, LOW);
  // delay(250);
  analogWrite(GreenPin, 25);
  delay(250);
  digitalWrite(GreenPin, LOW);
  Serial.println("Game Started");
}

void fastBlink(uint8_t ledPin)
{
  for (int i = 0; i < 7; ++i)
  {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}

void checkPlayerInput()
{
  if (digitalRead(P1ButtonPin) == LOW)
  {
    digitalWrite(P1LedPin, HIGH);
    playing = false;

    if (go)
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
    digitalWrite(P1LedPin, LOW);
    delay(500);
  }
  if (digitalRead(P2ButtonPin) == LOW && playing)
  {
    digitalWrite(P2LedPin, HIGH);
    playing = false;
    if (go)
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
    digitalWrite(P2LedPin, LOW);
    delay(500);
  }
}

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
    digitalWrite(P1LedPin, LOW);
    digitalWrite(P2LedPin, LOW);
    delay(highestScore > 5 ? 150 : 250);
  }
}

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
}

void loop()
{
  startSequence();

  goTime = millis() + random(100, 8000);
  playing = true;

  while (playing)
  {
    go = millis() >= goTime;
    if (go)
    {
      analogWrite(GreenPin, 25);
    }
    checkPlayerInput();
  }

  digitalWrite(GreenPin, LOW);
  showScore();

  delay(1000);
}
