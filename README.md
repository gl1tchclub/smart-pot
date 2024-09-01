# Smart Pot!

An automatic irrigation system that does the following:
- Monitor temperature, humidity, and soil moisture,
- Displays environment conditions
- Changes displayed info through button press
- Automatically waters plant when below threshold
- Alerts user through display when there's not enough water in the tank

The purpose of the Smart Pot is to assist plant owners of any experience by providing efficient plant care and sustainable water usage. Many plant owners often throw out their plants due as they can be complex to take care of, especially for amateurs. Because of this, many are hesitant to become long-term plant owners. To change this, the Smart Pot project hopes to make a portable product focused on efficient plant care for household plants.

## Hook Up Guide
**Items:**
-	1x LCD screen 2x16
-	5x buttons
-	30x wires
- 5x 10k resistors
- 1x soil moisture sensor
- 1x DHT11 temp/humidity sensor
- 1x LED strip
- 1x relay
- 1x water solenoid
- 2x alligator clips
- 1x hose 

**Pinout:**

| Component    | Pin # |
| -------- | ------- |
| ON/OFF button  | 2    |
| Display Moisture button  | 8    |
| Display Climate button  | 12    |
| Home button  | 4    |
| Water Dispense button  | 9    |
| Relay  | 10    |
| DHT11  | 3    |
| Relay  | 10    |
| LED strip  | 5    |
| Moisture sensor  | A0    |

The water solenoid and relay are not in the wiring diagram as tinkerCAD did not have the parts.

![final diagram](https://github.com/gl1tchclub/smart-pot/blob/9d05f40109e20dcf5d040cecf57edd64e31cd5bf/wiring%20diagrams/5-more-buttons.PNG)

## Future Scope
For the next iteration, we hope to continue working on the physical design with a focus on portability and aesthetics while ensuring durability. We also plan to incorporate a water tank with a water level sensor to ensure dispensing does not repeatedly occur when there's no water available.

Aside from design, we plan to make a mobile/web app to go along with the physical product. This app ideally will notify the user of any changes/needs of the plant, the current status of the plant, and will allow the user to interact with the plant at any time such as watering, changing plant details, and turning the product off/on. Additionally, the app will also allow the user to connect multiple Smart Pots to the app and will fetch the plant species data from an API to personalize each plant's care regime.


