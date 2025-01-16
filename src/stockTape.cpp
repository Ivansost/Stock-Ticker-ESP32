//librarys
#include <LiquidCrystal.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

LiquidCrystal lcd(19, 23, 18, 17, 16, 15);

const int ledPin = 14;
float previousPrice = 0;

const char *ssid = "wifissid";
const char *password = "wifipass";
String payload = "";

//function that resets the LCD display
void resetDisplay() {
    lcd.clear();
}

//function that updates the LED if the price changes
void updateLED(float currentPrice) {
    if (currentPrice != previousPrice) {
        digitalWrite(ledPin, HIGH);
        delay(1000);
        digitalWrite(ledPin, LOW);
    }

    previousPrice = currentPrice;
}

//function that connects to WiFi
void connectWiFi() {

    resetDisplay();

    lcd.setCursor(0, 0);
    lcd.print("WIFI");
    Serial.println("WIFI");
    lcd.setCursor(0, 1);

    lcd.print("Connecting...");
    Serial.println("Connecting...");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }

    resetDisplay();

    lcd.setCursor(0, 0);
    lcd.print("WIFI");
    Serial.println("WIFI");

    lcd.setCursor(0, 1);
    lcd.print("Connected");
    Serial.println("Connected");

    delay(2000);
}

//function that displays price data
void displayPriceData(const String &stockName, float currentPrice, float dollarUp, float percentage) {

    updateLED(currentPrice);

    resetDisplay();

    lcd.setCursor(0, 0);
    lcd.print(stockName);

    lcd.setCursor(8, 0);
    lcd.print(currentPrice, 2);

    lcd.setCursor(0, 1);
    lcd.print(dollarUp, 2);

    lcd.setCursor(9, 1);
    lcd.printf("%.2f%%", percentage);

    Serial.println("Current Price: $" + String(currentPrice));
    Serial.println("Dollar Change (24h): $" + String(dollarUp));
    Serial.printf("Percentage Change (24h): %.2f%%\n", percentage);
}

//function that reads the stock price
void readPrice(const String &stockName) {
    String httpRequestAddress = "https://pro-api.coinmarketcap.com/v2/cryptocurrency/quotes/latest?symbol=" + stockName + "API KEY";
    HTTPClient http;
    int httpCode;

    http.begin(httpRequestAddress);
    httpCode = http.GET();

    if (httpCode > 0) {
        DynamicJsonDocument doc(4096);
        String payload = http.getString();

        Serial.println(payload);
        
        deserializeJson(doc, payload);

        JsonArray results = doc["data"][stockName].as<JsonArray>();

        float currentPrice = results[0]["quote"]["USD"]["price"];
        float percentage = results[0]["quote"]["USD"]["percent_change_24h"];
        float dollarUp = (currentPrice * percentage) / 100;

        displayPriceData(stockName, currentPrice, dollarUp, percentage);

    } else {
        Serial.println("HTTP GET request failed");

        resetDisplay();

        lcd.setCursor(0, 0);
        lcd.print("HTTP error");
    }

    http.end();
}

void setup() {

    lcd.begin(16, 2);
    Serial.begin(9600);

    pinMode(ledPin, OUTPUT);
    connectWiFi();
}

void loop() {

    readPrice("BTC");
    delay(4000);
}
