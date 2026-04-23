#include <Arduino.h>
#include <string>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ESP_Mail_Client.h>
#include <DHT.h>
#include <secrets.h>

#define DHT_PIN 4
#define DHT_TYPE DHT11

using namespace std;

SMTPSession smtp;
DHT dht(DHT_PIN, DHT_TYPE);
WebServer server(80);

unsigned long lastReadTime = 0;
unsigned long lastEmailTime = 0;

void sendEmail(const char *recipient, const char *subject, const char *body);
void connectWifi();
void checkTemp();
void handleRoot();

void setup()
{
  Serial.begin(115200);
  connectWifi();
  MDNS.begin("fridge");
  server.on("/", handleRoot);
  dht.begin();
}

void loop()
{
  server.handleClient();
  checkTemp();
}

void connectWifi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void sendEmail(const char *recipient, const char *subject, const char *body)
{
  ESP_Mail_Session session;
  session.server.host_name = "smtp.gmail.com";
  session.server.port = 465;
  session.login.email = SENDER_EMAIL;
  session.login.password = SENDER_PASSWORD;

  SMTP_Message message;
  message.sender.name = "ESP32";
  message.sender.email = SENDER_EMAIL;
  message.subject = subject;
  message.addRecipient("Recipient", recipient);
  message.text.content = body;

  if (!smtp.connect(&session))
  {
    Serial.print("Connection failed: ");
    Serial.println(smtp.errorReason());
    return;
  }

  if (!MailClient.sendMail(&smtp, &message))
  {
    Serial.print("Send failed: ");
    Serial.println(smtp.errorReason());
    return;
  }

  Serial.println("Email sent");
}

void checkTemp()
{
  if (millis() - lastReadTime < 2000) return;
  lastReadTime = millis();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(true);

  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (temperature >= 40.0)
  {
    if (millis() - lastEmailTime < 3600000) return; // only email once per hour
    lastEmailTime = millis();

    String message = "WARNING: Fridge Temperature Below 40 °F\n";
    message += "Current Fridge Temperature: ";
    message += String(temperature);
    message += " °F";

    String subject = "WARNING: Garage Fridge Temperature Low";

    sendEmail(RECIPIENT1, subject.c_str(), message.c_str());
  }
}

void handleRoot()
{
  float temperature = dht.readTemperature(true);

  String html = "<!DOCTYPE html><html>";
  html += "<head><title>ESP32 Temperature</title>";
  html += "<meta http-equiv='refresh' content='5'>"; 
  html += "</head><body>";
  html += "<h1>Fridge Temperature</h1>";
  html += "<p>Temperature: ";
  html += String(temperature);
  html += " °F</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}