// Include the librarys
#include <LiquidCrystal.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Pins for lcd Display
LiquidCrystal lcd(19, 23, 18, 17, 16, 15);

const int ledPin = 14; // Led pin

float previousPrice = 0;

// Wifi name and pass
const char *ssid = "nachowifi";
const char *password = "nachotaco";
String payload = "";

// Function that resets the ldc display
void resetDisplay()
{

  lcd.clear();
}

// Function that updates the LED if the price changes
void updateLED(float currentPrice)
{

  if (currentPrice != previousPrice)
  {

    digitalWrite(ledPin, HIGH); // Turn LED ON
    delay(1000);                // Keep LED ON for 1 second
    digitalWrite(ledPin, LOW);  // Turn LED OFF
  }

  // Update the previous price for the next comparison
  previousPrice = currentPrice;
}

// Function that connects to wifi
void connectWiFi()
{

  resetDisplay();

  // Prints when wifi is connecting
  lcd.setCursor(0, 0);
  lcd.print("WIFI");
  Serial.println("WIFI");
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");
  Serial.println("Connecting...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }

  resetDisplay();

  // Prints when wifi is connected
  lcd.setCursor(0, 0);
  lcd.print("WIFI");
  Serial.println("WIFI");
  lcd.setCursor(0, 1);
  lcd.print("Connected");
  Serial.println("Connected");

  delay(2000);
}

// Function that displays price data
void displayPriceData(const String &stockName, float currentPrice, float dollarUp, float percentage)
{

  // Update LED
  updateLED(currentPrice);

  resetDisplay();

  // Display results on the LCD
  lcd.setCursor(0, 0);
  lcd.print(stockName);

  lcd.setCursor(8, 0);
  lcd.print(currentPrice, 2); // Print current price with 2 decimal places

  lcd.setCursor(0, 1);
  lcd.print(dollarUp, 2); // Print dollar change with 2 decimal places

  lcd.setCursor(9, 1);
  lcd.printf("%.2f%%", percentage); // Print percentage change

  // Print results to Serial Monitor
  Serial.println("Current Price: $" + String(currentPrice));
  Serial.println("Dollar Change (24h): $" + String(dollarUp));
  Serial.printf("Percentage Change (24h): %.2f%%\n", percentage);
}

// Function the reads the stock price
void readPrice(const String &stockName)
{
  String httpRequestAddress = "https://pro-api.coinmarketcap.com/v2/cryptocurrency/quotes/latest?symbol=" + stockName + "API KEY";
  HTTPClient http;
  int httpCode;

  http.begin(httpRequestAddress);
  httpCode = http.GET();

  if (httpCode > 0)
  {
    DynamicJsonDocument doc(4096); // Adjust size as needed
    String payload = http.getString();
    Serial.println(payload); // Debug: print the payload for inspection
    deserializeJson(doc, payload);

    // Access the JSON structure
    JsonArray results = doc["data"][stockName].as<JsonArray>();

    // Get the data
    float currentPrice = results[0]["quote"]["USD"]["price"];
    float percentage = results[0]["quote"]["USD"]["percent_change_24h"];
    float dollarUp = (currentPrice * percentage) / 100;

    // Call the display function to print data
    displayPriceData(stockName, currentPrice, dollarUp, percentage);
  }

  else
  {
    Serial.println("HTTP GET request failed");
    resetDisplay();
    lcd.setCursor(0, 0);
    lcd.print("HTTP error");
  }

  http.end();
}

void setup()
{
  lcd.begin(16, 2); // Initializes the LCD screen 16x2

  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  connectWiFi(); // Connects to wifi
}

void loop()
{
  readPrice("BTC");
  delay(4000); // Delay to avoid spamming API requests
}
