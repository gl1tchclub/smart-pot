#include <FastLED.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "DHT.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define LED properties
#define LED_PIN 5
#define NUM_LEDS 6  // cahnge to 25
#define BRIGHTNESS 70
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// Button variables
int ON_BTN_PIN 2
int CLIMATE_BTN_PIN 12
int MOIST_BTN_PIN 8

int pins[] = {ON_BTN_PIN, CLIMATE_BTN_PIN, MOIST_BTN_PIN}

// Temp/Humid Sensor Variables
#define DHTPIN 3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Moisture sensor variables
#define MOIST_SENS_PIN A0
#define MOIST_THRESHOLD 530


// All possible machine states
enum State {
  OFF,
  ON,
  DISPLAY_CLIMATE,         // Climate of plant i.e. temp and humid
  DISPLAY_SOIL_INFO,       // Soil moisture and light percent
  DISPLAY_WATER_DISPENSE,  // "Dispensing water..."
  DISPLAY_PLANT            // Plant name, mood i.e. "Good!", "Alright", etc...
};

State machineState = ON;

// Compare climate differences for debouncing
float currentTemp = 0;
float currentHumid = 0;
static float lastTemp = 0;
static float lastHumid = 0;

// Keep button reading consistent for each loop
static int currentBtn = ON_BTN_PIN;

void setup() {
  Serial.begin(9600);

  lcd.init();

  for (int i = 0; i < 4; i++) {
    pinMode(pins[i], INPUT);
  }

  // Init moisture sensor
  pinMode(MOIST_SENS_PIN, INPUT);

  // Init DHT11 sens
  dht.begin();

  // Init LED strip and set starting LED state
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

}

void loop() {
  handleStateMachine();
  delay(100);
}

void handleMachineState() {
  if (debounceButton()) {
    pressedButton();
    transition();
  }
}

void transisiton() {
  switch (machineState) {
    case OFF: 
      if (currentBtn == )
  }
  if (machineState == OFF) {
      machineState = ON;
    } else if (machineState == ON) {
      machineState = OFF;
    } else if (machineState == ) {
      machineState = OFF;
    } else if (machineState == ON) {
      machineState = OFF;
    }
}

void stateAction() {
  switch (machineState) {
    case OFF:

  }
}

// Delay button/state change by .5 seconds to count for button noise, change state if any button has been pressed
bool debounceButton() {
  static int lastButtonState = 0;  // Initialize to 1 to detect falling edge
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;  // milliseconds
  int reading = 0;

  for (int i = 0; i < 3; i++) {
    reading = digitalRead(pins[i]);
    if (reading == HIGH) {
      currentBtn = pins[i];
    }
  }

  unsigned long elapsedTime = millis() - lastDebounceTime;

  if (reading != lastButtonState) {
    // Button state has changed, update debounce time
    lastDebounceTime = millis();
  }

  // If debounce delay has passed
  if (elapsedTime >= debounceDelay) {
    // Return true only if the button is pressed (reading is HIGH) AND If the current state is different from the last state
    if (reading == HIGH && reading != lastButtonState) {
      lastButtonState = reading;
      return true;
    }
  }

  lastButtonState = reading;
  return false;
}
