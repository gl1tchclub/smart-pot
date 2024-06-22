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

CRGB soilColor = CRGB::Green;
CRGB tempColor = CRGB::Green;
CRGB humidColor = CRGB::Green;

// Button variables
int ON_BTN_PIN = 2;
int CLIMATE_BTN_PIN = 12;
int SOIL_BTN_PIN = 8;
int HOME_BTN_PIN = 4;
int WATER_BTN_PIN = 9;

int pins[] = { ON_BTN_PIN, CLIMATE_BTN_PIN, SOIL_BTN_PIN, HOME_BTN_PIN, WATER_BTN_PIN };

// Temp/Humid Sensor Variables
#define DHTPIN 3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float MIN_TEMP = 10.00;
float MAX_TEMP = 30.00;
float MIN_HUMID = 25.00;
float MAX_HUMID = 75.00;

// Moisture sensor variables
#define MOIST_SENS_PIN A0
int MIN_MOIST = 30;
int MAX_MOIST = 75;


// All possible machine states
enum State {
  OFF,
  ON,
  DISPLAY_CLIMATE,    // Climate of plant i.e. temp and humid
  DISPLAY_SOIL_INFO,  // Soil moisture and water threshold
  DISPENSE_WATER,     // "Dispensing water..."
  DISPLAY_HOME        // Plant name, mood i.e. "Good!", "Alright", etc...
};

// Automatically turn on when power first received
State machineState = ON;

// Compare climate differences for debouncing
static float currentTemp = 0;
static float currentHumid = 0;
static float lastTemp = 0;
static float lastHumid = 0;
static int currentMoist = 0;
static int lastMoist = 0;

static int happiness = 100;

// Keep button reading consistent for each loop
static int currentBtn = 2;
static int lastBtn = 0;

void setup() {
  Serial.begin(9600);

  lcd.init();

  for (int i = 0; i < 5; i++) {
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

  turnOn();
}

void loop() {
  handleMachineState();
  delay(100);
}

void handleMachineState() {
  // If machine is on, always check soil/climate, and change LED
  if (machineState != OFF && machineState != ON) {
    // If 5 seconds have passed since last reading
    if (readingBuffer()) {
      readSoil();
      readClimate();
      stateAction();
      changeLed();
    }
  }

  // If a button is pressed, change state and do something
  if (debounceButton()) {
    changeState();
    stateAction();
  }
}

// Change machine state depending on button press
void changeState() {
  // if machine state is not off, then allow user functionality
  if (machineState != OFF) {
    switch (currentBtn) {
      case 2:
        machineState = OFF;
        break;
      case 12:
        machineState = DISPLAY_CLIMATE;
        break;
      case 8:
        machineState = DISPLAY_SOIL_INFO;
        break;
      case 4:
        machineState = DISPLAY_HOME;
        break;
      case 9:
        if (machineState != DISPENSE_WATER) {
          machineState = DISPENSE_WATER;
        }
        break;
    }
  }
  //if machine state is off and on btn pressed, then turn on
  else if (machineState == OFF && currentBtn == 2) {
    machineState = ON;
  }
}

void stateAction() {
  switch (machineState) {
    case OFF:
      turnOff();
      break;
    case ON:
      turnOn();
      break;
    case DISPLAY_CLIMATE:
      displayClimate();
      break;
    case DISPLAY_SOIL_INFO:
      displaySoil();
      break;
    //display happiness, plant name
    case DISPLAY_HOME:
      home();
      break;
    case DISPENSE_WATER:
      dispense();  //display message, dispense water, if no soil change "water not dispensed", else "dispense complete"
      machineState = DISPLAY_HOME;
      currentBtn = 4;
      home();
      break;
  }
}

void home() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("House Plant");
  lcd.setCursor(0, 1);
  lcd.print("Feeling Good");
}

void dispense() {
  Serial.println("Watering...");
  lcd.clear();
  lcd.home();
  lcd.print("Watering...");
  fill_solid(soilLeds, NUM_LEDS, CRGB::Blue);
  int reading = 0;
  // Low moisture
  if (currentMoist <= 60) {
    // while low moist
    while (currentMoist <= 60) {
      // keep reading soil levels until moist enough
      readSoil();
      // if 5 seconds have passed and not watered, print failed and exit loop
      if (readingBuffer() && currentMoist <= 60) {
        lcd.clear();
        Serial.println("Failed to water");
        lcd.setCursor(0, 0);
        lcd.print("No water");
        delay(1000);
        return;
      }
    }
    lcd.clear();
    Serial.println("Complete");
    lcd.setCursor(3, 0);
    lcd.print("Plant");
    lcd.setCursor(1, 1);
    lcd.print("Watered!");
    delay(1000);
  }
  // if someone pressed the button and not low
  if (currentMoist >= 60) {
    lcd.clear();
    Serial.println("Do not need watering");
    lcd.setCursor(0, 0);
    lcd.print("Already full");
  }
  delay(1000);
  machineState = DISPLAY_HOME;
  home();
}

// turn off screen w message, change LED to black (do not change led color state)
void turnOff() {
  Serial.println("Turning off");
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.println("Turning Off...");
  delay(1000);
  lcd.clear();
  lcd.noBacklight();
  lcd.noDisplay();
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void turnOn() {
  Serial.println("Turning on");
  lcd.backlight();
  lcd.display();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.println("Turning On...");
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  FastLED.show();
  delay(1000);
  machineState = DISPLAY_HOME;
  currentBtn = 4;
  home();
}

void displaySoil() {
  Serial.println("Displaying soil!");
  lcd.clear();
  lcd.setCursor(0, 0);  // set the cursor on the first row and column
  lcd.print("Moisture: ");
  lcd.print(currentMoist);  //print the water content
  lcd.print("%");
  lcd.setCursor(0, 1);  //set the cursor on the second row and first column
  lcd.print("Threshold: ");
  lcd.print(MAX_MOIST);  //print the temperature
  lcd.print("%");
}

void displayClimate() {
  Serial.println("Displaying climate");
  lcd.clear();
  lcd.setCursor(0, 0);  // set the cursor on the first row and column
  lcd.print("Humidity: ");
  lcd.print(currentHumid);  //print the humidity
  lcd.print("%");
  lcd.setCursor(0, 1);  //set the cursor on the second row and first column
  lcd.print("Temp: ");
  lcd.print(currentTemp);  //print the temperature
  lcd.print((char)0b11011111);
  lcd.print("C");
}

void changeLed() {
  if (currentMoist > MAX_MOIST || currentTemp > MAX_TEMP || currentHumid > MAX_HUMID) {
    fill_solid(soilLeds, NUM_LEDS, CRGB::Red);
  } else if (currentMoist < MIN_MOIST || currentTemp < MIN_TEMP || currentHumid < MIN_HUMID) {
    fill_solid(soilLeds, NUM_LEDS, CRGB::Yellow);
  } else {
    fill_solid(soilLeds, NUM_LEDS, CRGB::Green);
  }
  FastLED.show();
}

bool readSoil() {
  currentMoist = (analogRead(MOIST_SENS_PIN)) / 10;  // read the analog value from sensor and convert

  // Check if readings are valid
  if (isnan(currentMoist)) {
    Serial.println("Failed to read from Soil sensor!");
    lcd.print("Failed");
    return false;
  }

  // If the latest reading differs from the previous readings...
  if (currentMoist != lastMoist) {
    // Update the last moisture readings
    lastMoist = currentMoist;
    Serial.println("Current moist: ");
    Serial.println(currentMoist);
    return true;
  }

  return false;
}

bool readClimate() {
  // Read current temperature and humidity from the sensor
  currentTemp = (float)dht.readTemperature();
  currentHumid = (float)dht.readHumidity();

  // Check if readings are valid
  if (isnan(currentTemp) || isnan(currentHumid)) {
    Serial.println("Failed to read from DHT sensor!");
    lcd.print("Failed");
    return false;
  }

  // If the latest temperature or humidity differs from the previous readings...
  if (currentTemp != lastTemp || lastHumid != currentHumid) {
    // Update the last temperature and humidity readings
    lastHumid = currentHumid;
    lastTemp = currentTemp;
    Serial.print("Temp: ");
    Serial.println(currentTemp);
    Serial.print("Humid: ");
    Serial.println(currentHumid);
    return true;
  }

  return false;
}

bool readingBuffer() {
  unsigned long currentCheckTime = millis();
  static unsigned long lastCheckTime = 0;

  // If 5 seconds have passed...
  if (currentCheckTime >= (lastCheckTime + 5000)) {
    // Change last time
    lastCheckTime = currentCheckTime;
    // Return true to indicate a 5 second passing
    return true;
  }

  // Return false if 5 seconds have not passed
  return false;
}

// Delay button/state change by .5 seconds to count for button noise, return if any button has been pressed
bool debounceButton() {
  static int lastButtonState = 0;  // Initialize to 1 to detect falling edge
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;  // milliseconds
  int reading = 0;

  for (int i = 0; i < 5; i++) {
    reading = digitalRead(pins[i]);
    if (reading == HIGH) {
      currentBtn = pins[i];
      break;
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
    if (reading == HIGH && reading != lastButtonState) {  // is  && reading != lastButtonState needed?
      lastButtonState = reading;
      Serial.println("Button pressed!");
      return true;
    }
  }
  Serial.println("Waiting...");
  lastButtonState = reading;
  return false;
}
