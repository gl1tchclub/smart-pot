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

#define soilColor CRGB::Green
#define tempColor CRGB::Green
#define humidColor CRGB::Green

// Button variables
int ON_BTN_PIN = 2;
int CLIMATE_BTN_PIN = 12;
int SOIL_BTN_PIN = 8;
int HOME_BTN_PIN = 4;
int WATER_BTN_PIN = 9;

int pins[] = { ON_BTN_PIN, CLIMATE_BTN_PIN, SOIL_BTN_PIN, HOME_BTN_PIN, WATER_BTN_PIN }

// Temp/Humid Sensor Variables
#define DHTPIN 3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int MIN_TEMP 10;
int MAX_TEMP 30;
int MIN_HUMID 30;
int MAX_HUMID 75;

// Moisture sensor variables
#define MOIST_SENS_PIN A0
int MIN_MOIST 400;
int MAX_MOIST 700


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
float currentTemp = 0;
float currentHumid = 0;
static float lastTemp = 0;
static float lastHumid = 0;
static int currentMoist = 0;
static int lastMoist = 0;

// Keep button reading consistent for each loop
static int currentBtn = ON_BTN_PIN;

void setup() {
  Serial.begin(9600);

  lcd.init();

  for (int i = 0; i < 5; i++) {
    pinMode(pins[i], INPUT);
  }

  // Init moisture sensor
  pinMode(SOIL_SENS_PIN, INPUT);

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
  handleStateMachine();
  delay(100);
}

void handleMachineState() {
  // If a button is pressed, change state and do something
  if (debounceButton()) {
    changeState();
    stateAction();
  }

  // If machine is on, always check soil/climate, and change LED
  if (machineState != OFF) {
    readSoil();
    readClimate();
    changeLED();
  }
}

// Change machine state depending on button press
void changeState() {
  // if machine state is not off, then allow user functionality
  if (machineState != OFF) {
    switch (currentBtn) {
      case ON_BTN_PIN:
        machineState = OFF;
        break;
      case CLIMATE_BTN_PIN:
        machineState = DISPLAY_CLIMATE;
        break;
      case SOIL_BTN_PIN:
        machineState = DISPLAY_SOIL_INFO;
        break;
      case HOME_BTN_PIN:
        machineState = DISPLAY_HOME;
        break;
        // case WATER_BTN_PIN:
        //   machineState = DISPENSE_WATER;
        //   break;
    }
  }
  //if machine state is off and on btn pressed, then turn on
  else if (machineState == OFF && currentBtn == ON_BTN_PIN) {
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
      machineState = DISPLAY_HOME;
      home();
      break;
    case DISPLAY_CLIMATE:
      readClimate();
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
      home();
      break;
  }
}

// turn off screen w message, change LED to black (do not change led color state)
void turnOff() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Turning Off...");
  delay(1000);
  lcd.noBacklight();
  lcd.clear();
  lcd.noDisplay();
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void turnOn() {
  lcd.backlight();
  lcd.display();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Turning On...");
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  FastLED.show();
  delay(1000);
  home();
}

void displaySoil() {
  if (readingBuffer()) {
    lcd.clear();
    lcd.setCursor(0, 0);  // set the cursor on the first row and column
    lcd.print("Moisture: ");
    lcd.print(moisture);  //print the water content
    lcd.print("%");
    lcd.setCursor(0, 1);  //set the cursor on the second row and first column
    lcd.print("Threshold: ");
    lcd.print(threshold);  //print the temperature
    lcd.print("%");
    delay(1000);
  }
}

void displayClimate() {
  if (readingBuffer()) {
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
    delay(1000);
  }
}

void changeLed() {

  // Update moist color
  if (currentMoist > MAX_MOIST) {
    soilColor = CRGB::Purple
  }
  if (currentMoist <= MAX_MOIST && currentMoist > MIN_MOIST) {
    soilColor = CRGB::Green
  }
  if (currentMoist < MIN_MOIST) {
    soilColor = CRGB::Red
  }

  // Update temp color
  if (currentTemp > MAX_TEMP) {
    tempColor = CRGB::Purple
  }
  if (currentTemp <= MAX_TEMP && currentTemp > MIN_TEMP) {
    tempColor = CRGB::Green
  }
  if (currentTemp < MIN_TEMP) {
    tempColor = CRGB::Red
  }

  // Update humid color
  if (currentHumid > MAX_HUMID) {
    humidColor = CRGB::Purple
  }
  if (currentHumid <= MAX_HUMID && currentHumid > MIN_HUMID) {
    humidColor = CRGB::Green
  }
  if (currentHumid < MIN_HUMID) {
    humidColor = CRGB::Red
  }

  leds[0] = soilColor;
  leds[1] = soilColor;
  leds[2] = humidColor;
  leds[3] = humidColor;
  leds[4] = tempColor;
  leds[5] = tempColor;
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
      return true;
    }
  }

  lastButtonState = reading;
  return false;
}
