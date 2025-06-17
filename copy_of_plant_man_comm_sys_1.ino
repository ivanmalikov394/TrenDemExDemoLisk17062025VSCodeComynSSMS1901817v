#include <LiquidCrystal_I2C.h>

const int SENSOR_PIN = A0; //CHANGE THIS

const int RED_PIN = 11; //CHANGE THIS
const int GREEN_PIN = 9;  //CHANGE THIS
const int BLUE_PIN = 10;  //CHANGE THIS

const int BLINK_LED_PIN = 6; //CHANGE THIS

const int SENSOR_MIN_VALUE = 0;  //CHANGE THIS 
const int SENSOR_MAX_VALUE = 876; //CHANGE THIS

const int MOISTURE_THRESHOLD_BLINK_ON = map(75, 0, 100, SENSOR_MIN_VALUE, SENSOR_MAX_VALUE); 
const int MOISTURE_THRESHOLD_VERY_DRY = map(20, 0, 100, SENSOR_MIN_VALUE, SENSOR_MAX_VALUE); 

const int BLINK_DELAY_SLOW = 1000; //CHANGE THIS
const int BLINK_DELAY_FAST = 200;  //CHANGE THIS

LiquidCrystal_I2C lcd(0x27, 16, 2); //CHANGE THIS

unsigned long lastRgbUpdate = 0;       
const unsigned long RGB_UPDATE_INTERVAL = 100; 

int previousMoisturePercent = -1; 

int sensorValue = 0;             
int constrainedSensorValue = 0;  

int moisturePercent = 0;         

int red = 0;
int green = 0;
int blue = 0;

void updateLcd(); 

void setup() {

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BLINK_LED_PIN, OUTPUT);

  Serial.begin(9600); 

  lcd.init();      
  lcd.backlight(); 

  lcd.print("Plant Monitor");
  lcd.setCursor(0, 1); 
  lcd.print("Initializing...");
  delay(2000); 
}

void loop() {
  unsigned long currentMillis = millis(); 

  sensorValue = analogRead(SENSOR_PIN); 

  constrainedSensorValue = constrain(sensorValue, SENSOR_MIN_VALUE, SENSOR_MAX_VALUE);

  moisturePercent = map(sensorValue, SENSOR_MIN_VALUE, SENSOR_MAX_VALUE, 0, 100);

  if (currentMillis - lastRgbUpdate >= RGB_UPDATE_INTERVAL) {
    lastRgbUpdate = currentMillis; 

    int point1 = map(255, 0, 1023, SENSOR_MIN_VALUE, SENSOR_MAX_VALUE);
    int point2 = map(511, 0, 1023, SENSOR_MIN_VALUE, SENSOR_MAX_VALUE); 
    int point3 = map(766, 0, 1023, SENSOR_MIN_VALUE, SENSOR_MAX_VALUE); 

    if (constrainedSensorValue >= SENSOR_MIN_VALUE && constrainedSensorValue <= point1) {

      red = map(constrainedSensorValue, SENSOR_MIN_VALUE, point1, 255, 127); 
      blue = map(constrainedSensorValue, SENSOR_MIN_VALUE, point1, 0, 127);  
      green = 0; 
    } else if (constrainedSensorValue > point1 && constrainedSensorValue <= point2) {

      red = map(constrainedSensorValue, point1, point2, 127, 0);  
      blue = map(constrainedSensorValue, point1, point2, 127, 255); 
      green = 0; 
    } else if (constrainedSensorValue > point2 && constrainedSensorValue <= point3) {

      red = 0; 
      blue = map(constrainedSensorValue, point2, point3, 255, 127); 
      green = map(constrainedSensorValue, point2, point3, 0, 127);  
    } else if (constrainedSensorValue > point3 && constrainedSensorValue <= SENSOR_MAX_VALUE) {

      red = 0; 
      blue = map(constrainedSensorValue, point3, SENSOR_MAX_VALUE, 127, 0); 
      green = map(constrainedSensorValue, point3, SENSOR_MAX_VALUE, 127, 255); 
    } else {

      red = 0; green = 0; blue = 0; 
    }

    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
  }

  if (constrainedSensorValue < MOISTURE_THRESHOLD_BLINK_ON) {

    int blinkDelay = map(constrainedSensorValue, MOISTURE_THRESHOLD_VERY_DRY, MOISTURE_THRESHOLD_BLINK_ON, BLINK_DELAY_FAST, BLINK_DELAY_SLOW);

    blinkDelay = constrain(blinkDelay, BLINK_DELAY_FAST, BLINK_DELAY_SLOW);

    digitalWrite(BLINK_LED_PIN, HIGH); 
    delay(blinkDelay);                 
    digitalWrite(BLINK_LED_PIN, LOW);  
    delay(blinkDelay);                 
  } else {

    digitalWrite(BLINK_LED_PIN, LOW);
  }

  if (moisturePercent != previousMoisturePercent) {
    updateLcd(); 
    previousMoisturePercent = moisturePercent; 
  }

  Serial.print("Sensor Value: ");
  Serial.print(sensorValue); 
  Serial.print(" (");
  Serial.print(moisturePercent); 
  Serial.print("% moisture)");

  Serial.print("\tRGB - R:");
  Serial.print(red);    
  Serial.print(", G:");
  Serial.print(green);  
  Serial.print(", B:");
  Serial.print(blue);   

  Serial.print("\tBlink LED (Pin ");
  Serial.print(BLINK_LED_PIN);
  Serial.print("): ");
  if (constrainedSensorValue < MOISTURE_THRESHOLD_BLINK_ON) {
    Serial.print("Blinking (Delay: ");
    Serial.print(map(constrainedSensorValue, MOISTURE_THRESHOLD_VERY_DRY, MOISTURE_THRESHOLD_BLINK_ON, BLINK_DELAY_FAST, BLINK_DELAY_SLOW));
    Serial.println("ms)");
  } else {
    Serial.println("Off");
  }

  delay(5); 
}

void updateLcd() {
  lcd.clear(); 
  lcd.setCursor(0, 0); 

  if (moisturePercent <= 10) {
    lcd.print("CRITICAL DRY"); 
    lcd.setCursor(0, 1);           
    lcd.print(moisturePercent);    
    lcd.print("% MOISTURE");       
  } else if (moisturePercent <= 25) {
    lcd.print("I'm very thirsty"); 
    lcd.setCursor(0, 1);
    lcd.print(moisturePercent);
    lcd.print("% MOISTURE");
  } else if (moisturePercent <= 45) {
    lcd.print("Water me, please"); 
    lcd.setCursor(0, 1);
    lcd.print(moisturePercent);
    lcd.print("% MOISTURE");
  } else if (moisturePercent <= 65) {
    lcd.print("Moisture good"); 
    lcd.setCursor(0, 1);
    lcd.print(moisturePercent);
    lcd.print("% MOISTURE");
  } else if (moisturePercent <= 100) { 
    lcd.print("Optimal moisture"); 
    lcd.setCursor(0, 1);
    lcd.print(moisturePercent);
    lcd.print("% MOISTURE");
  } else { 
    lcd.print("OVERWATERED"); 
    lcd.setCursor(0, 1);
    lcd.print(moisturePercent); 
    lcd.print("% MOISTURE");
  }
}