// this is the code for Master

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

// Initialize the LCD using I2C address (commonly 0x27 or 0x3F)
LiquidCrystal_PCF8574 lcd(0x27);

String receivedData = "";

// Callback function when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  char incomingString[32];   // Buffer to store the incoming data
  memcpy(incomingString, incomingData, len);  // Copy received data to a char array
  incomingString[len] = '\0';  // Null-terminate the string

  // Convert to String and print it
  receivedData = String(incomingString);
  Serial.print("Received: ");
  Serial.println(receivedData);

  // Update the LCD with the received data
  displayDataOnLCD(receivedData);
}

// Function to display data on a 16x2 LCD
void displayDataOnLCD(String data) {
  lcd.clear();  // Clear the display

  // If the string is longer than 16 characters, split it into two lines
  String line1 = data.substring(0, 16);  // Get the first 16 characters
  String line2 = data.length() > 16 ? data.substring(16, 32) : "";  // Get the next 16 characters, if any

  lcd.setCursor(0, 0);  // Set cursor to the first line
  lcd.print(line1);     // Print the first part of the string

  lcd.setCursor(0, 1);  // Set cursor to the second line
  lcd.print(line2);     // Print the second part (if exists)
}

void setup() {
  Serial.begin(115200); // Start Serial Monitor for debugging

  // Initialize the LCD
  lcd.begin(16, 2);   // Initialize 16x2 LCD
  lcd.setBacklight(255);  // Turn on the LCD backlight
  

  // Set WiFi mode to station (to use ESP-NOW)
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the receive callback
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Slave ready to receive data");
}

void loop() {
  // No need for code in the loop; data is received via the callback
}
