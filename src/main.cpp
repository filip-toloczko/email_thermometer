#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <secrets.h>

SMTPSession smtp;

// put function declarations here:
void sendEmail(const char *, const char *, const char *);

void setup()
{
  // put your setup code here, to run once:
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void loop()
{
  // put your main code here, to run repeatedly:
}

// put function definitions here:
void sendEmail(const char *recipient, const char *subject, const char *body)
{
  ESP_Mail_Session session;
  session.server.host_name = "smpt.gmail.com";
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
