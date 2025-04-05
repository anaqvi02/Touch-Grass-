#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int trigPin = 9;
const int echoPin = 10;

const int button1Pin = 5;
const int button2Pin = 3;
const int button3Pin = 4;

int lastButton1State = HIGH;
int lastButton2State = HIGH;
int lastButton3State = HIGH;

float duration, distance;
float lastValidDistance = -1;
int pushUpCounter = 0;
int currentState = 0;

float multiplierCost = 10;
int multiplier = 1;

int fanTimeLeft = 0;
long purchasedFan = 0;

float priceScale = 1.7;
int page = 0;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);

  lcd.begin();
  lcd.backlight();

  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
}

void loop() {

  Serial.println(pushUpCounter);

  int button1State = digitalRead(button1Pin);
  int button2State = digitalRead(button2Pin);
  int button3State = digitalRead(button3Pin);

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;

  if (distance <= 1200) {
    lastValidDistance = distance;
  }

  float displayedDistance = (distance > 1200) ? lastValidDistance : distance;

  switch (currentState) {
    case 0:
      if (displayedDistance > 35 && displayedDistance < 60) {
        currentState = 1;
      }
      break;

    case 1:
      if (displayedDistance < 10) {
        currentState = 2;
      }
      break;

    case 2:
      if (displayedDistance > 35 && displayedDistance < 60) {
        pushUpCounter += multiplier;
        currentState = 0;
      }
      break;
  }

  handleButtonPresses(button1State, button2State, button3State);

  if (page == 0) {
    mainScreen(displayedDistance);
  } else if (page == 1) {
    shopDisplay1();
  } else if (page == 2) {
    shopDisplay2();
  }

  subtractTime();
  delay(50);
}

void handleButtonPresses(int button1State, int button2State, int button3State) {
  if (button1State != lastButton1State) {
    if (button1State == LOW) {
      buyMultiplier();
    }
    lastButton1State = button1State;
  }

  if (button2State != lastButton2State) {
    if (button2State == LOW) {
      page++;
      if (page > 2) {
        page = 0;
      }
    }
    lastButton2State = button2State;
  }
   
  if (button3State != lastButton3State) {
    if (button3State == LOW) {
      buyFan();
    }
    lastButton3State = button3State;
  }
}

void buyMultiplier() {
  if (pushUpCounter >= multiplierCost) {
    pushUpCounter -= multiplierCost;
    multiplier++;
    multiplierCost *= priceScale;
    multiplierCost = ceil(multiplierCost);
  }
}

void buyFan(){
  if (pushUpCounter >= 10) {
    pushUpCounter -= 10;
    fanTimeLeft += 30;
    purchasedFan = millis();
  }
}

void subtractTime(){
  if(millis() - purchasedFan >= 1000 && fanTimeLeft > 0){
    fanTimeLeft -= 1;
    purchasedFan = millis();
  }
}

void mainScreen(float displayedDistance) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance: " + String(round(displayedDistance)));
  lcd.setCursor(0, 1);
  lcd.print("Pts: " + String(pushUpCounter));
}

void shopDisplay1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Muti Cost: " + String((int)multiplierCost));
  lcd.setCursor(0, 1);
  lcd.print("x" + String(multiplier));
}

void shopDisplay2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fan Cost: 10");
  lcd.setCursor(0, 1);
  lcd.print("Time Left: " + String(fanTimeLeft));
}
