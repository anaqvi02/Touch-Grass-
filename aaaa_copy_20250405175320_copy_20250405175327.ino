#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int trigPin = 9;
const int echoPin = 10;

// Define the button pins
const int button1Pin = 5; // Buy Multiplier
const int button2Pin = 3; // Change Page
const int button3Pin = 4; // Reserved for future use

// Variables to store the previous state of the buttons
int lastButton1State = HIGH;
int lastButton2State = HIGH;
int lastButton3State = HIGH;

float duration, distance;
float lastValidDistance = -1; // Store the last valid distance
int pushUpCounter = 0; // Counter for push-ups
int currentState = 0;  // State machine variable

float multiplierCost = 10; // Initial cost of the multiplier
int multiplier = 1; // Current multiplier level

int fanTimeLeft = 0;
long purchasedFan = 0;

float priceScale = 1.7;
int page = 0;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);

  // Initialize the LCD
  lcd.begin();
  lcd.backlight(); // Turn on the backlight

  // Configure the button pins as inputs with internal pull-up resistors
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
}

void loop() {

  // Debugging output to Serial Monitor
  Serial.println(pushUpCounter);

  // Read the current state of the buttons
  int button1State = digitalRead(button1Pin);
  int button2State = digitalRead(button2Pin);
  int button3State = digitalRead(button3Pin);

  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo pin and calculate the distance
  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;



  // Update the last valid distance if the current reading is valid
  if (distance <= 1200) {
    lastValidDistance = distance; // Store the valid distance
  }

  // Use the last valid distance if the current reading is invalid
  float displayedDistance = (distance > 1200) ? lastValidDistance : distance;

  // State machine logic for push-up counting
  switch (currentState) {
    case 0: // Waiting for distance to be between 35 and 60 cm
      if (displayedDistance > 35 && displayedDistance < 60) {
        currentState = 1; // Move to the next state
      }
      break;

    case 1: // Waiting for distance to go below 10 cm
      if (displayedDistance < 10) {
        currentState = 2; // Move to the next state
      }
      break;

    case 2: // Waiting for distance to be between 35 and 60 cm
      if (displayedDistance > 35 && displayedDistance < 60) {
        pushUpCounter += multiplier; // Increment the push-up counter with multiplier
        currentState = 0; // Reset to the initial state
      }
      break;
  }

  // Handle button presses
  handleButtonPresses(button1State, button2State, button3State);

  // Update the display based on the current page
  if (page == 0) {
    mainScreen(displayedDistance); // Pass the displayed distance
  } else if (page == 1) {
    shopDisplay1();
  } else if (page == 2) {
    shopDisplay2();
  }

  subtractTime();
  delay(50);
}

void handleButtonPresses(int button1State, int button2State, int button3State) {
  // Button 1: Buy Multiplier
  if (button1State != lastButton1State) { // State change detected
    if (button1State == LOW) { // Button pressed
      buyMultiplier();
    }
    lastButton1State = button1State; // Update the last known state
  }

  // Button 2: Change Page
  if (button2State != lastButton2State) { // State change detected
    if (button2State == LOW) { // Button pressed
      page++;
      if (page > 2) {
        page = 0;
      }
    }
    lastButton2State = button2State; // Update the last known state
  }
   
  if (button3State != lastButton3State) { // State change detected
    if (button3State == LOW) { // Button pressed
      buyFan();
    }
    lastButton3State = button3State; // Update the last known state
  }
}

void buyMultiplier() {
  if (pushUpCounter >= multiplierCost) {
    pushUpCounter -= multiplierCost; // Deduct the cost from points
    multiplier++; // Increase the multiplier level
    multiplierCost *= priceScale; // Increase the cost for the next multiplier
    multiplierCost = ceil(multiplierCost); // Round up to the nearest whole number
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
  lcd.print("Muti Cost: " + String((int)multiplierCost)); // Display rounded cost
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

