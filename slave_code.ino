#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT.h>

// Define DHT11 Sensor
#define DHTPIN D3  // GPIO Pin connected to DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Pin for soil moisture sensor
const int soilMoisturePin = A0;

// Structure for sending data to master
typedef struct struct_message {
  float temperature;
  float humidity;
  int soilMoisture; // Assuming this is in percentage (0-100)
} struct_message;

// Create a struct_message to hold sensor data
struct_message dataToSend;

// MAC address of the master ESP8266 (replace this with your master's MAC address)
uint8_t masterAddress[] = {0xF4, 0xCF, 0xA2, 0xDA, 0x27, 0x62};  // Example MAC Address

void sendData() {
  // Read data from DHT11
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Read soil moisture value
  int rawSoilMoisture = analogRead(soilMoisturePin);
  
  // Normalize soil moisture value to percentage (0-100)
  // Assuming the sensor outputs a value between 0 and 1023
  int soilMoisture = map(rawSoilMoisture, 0, 1023, 0, 100); 

  // Check if any reading failed
  if (isnan(temp) || isnan(hum)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Prepare data to send
  dataToSend.temperature = temp;
  dataToSend.humidity = hum;
  dataToSend.soilMoisture = soilMoisture; // Assign normalized soil moisture

  // Send the data to the master
  esp_now_send(masterAddress, (uint8_t *)&dataToSend, sizeof(dataToSend));

  // Debug info
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" C, Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Soil Moisture: ");
  Serial.println(soilMoisture);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize DHT11 sensor
  dht.begin();

  // Set device as a Wi-Fi station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set device as a sender
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(masterAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  // Send data every 10 seconds
  sendData();
  delay(10000);
}
