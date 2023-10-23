#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include "ESP32_MailClient.h"

const char* ssid = "GANGATHARAN";
const char* password = "skymars2106";

#define emailSenderAccount    "espantisleepingglass@gmail.com"
#define emailSenderPassword   "tpnliveoeegaynxu"
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort        465
#define emailSubject          "[ALERT] ESP32 Driver Sleep"

String inputMessage = "massganga82@gmail.com";
String enableEmailChecked = "checked";
String inputMessage2 = "true";
String inputMessage3 = "Your Driver was sleeping. Please Content us";


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Email Notification with Sleeping</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h2>ESP Email Notification</h2>
  <form action="/get">
    Email Address <input type="email" name="email_input" value="%EMAIL_INPUT%" required><br>
    Enable Email Notification <input type="checkbox" name="enable_email_input" value="true" %ENABLE_EMAIL%><br>
    ESP32 Output <input type="String" step="0.1" name="esp32_input" value="%ESP32_Output%" required><br>
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

AsyncWebServer server(80);

String processor(const String& var){
  //Serial.println(var);
  if(var == "EMAIL_INPUT"){
    return inputMessage;
  }
  else if(var == "ENABLE_EMAIL"){
    return enableEmailChecked;
  }
  else if(var == "ESP32_Output"){
    return inputMessage3;
  }
  return String();
}

// Flag variable to keep track if email notification was sent or not
bool emailSent = false;

const char* PARAM_INPUT_1 = "email_input";
const char* PARAM_INPUT_2 = "enable_email_input";
const char* PARAM_INPUT_3 = "esp32_input";

SMTPData smtpData;

void setup() {
  pinMode(14, INPUT);
  pinMode(27, OUTPUT);
  pinMode(26, OUTPUT);  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.println("ESP IP Address: http://");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      if (request->hasParam(PARAM_INPUT_2)) {
        inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
        enableEmailChecked = "checked";
      }
      else {
        inputMessage2 = "false";
        enableEmailChecked = "";
      }
      if (request->hasParam(PARAM_INPUT_3)) {
        inputMessage3 = request->getParam(PARAM_INPUT_3)->value();
      }
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    Serial.println(inputMessage2);
    Serial.println(inputMessage3);
    request->send(200, "text/html", "HTTP GET request sent to your ESP.<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  if(digitalRead(14)==LOW)
  {
    delay(3000);
    digitalWrite(27, HIGH);
    digitalWrite(26, HIGH);
  }
  else
  {
    digitalWrite(27, LOW);
    digitalWrite(26, LOW);
  }
  if(digitalRead(14)==LOW && inputMessage2 == "true"){
    String emailMessage = String("Your driver was sleeping ");
    if(sendEmailNotification(emailMessage)) {
      Serial.println(emailMessage);
      emailSent = true;
    }
    else {
      Serial.println("Email failed to send");
    } 
  }
}

bool sendEmailNotification(String emailMessage){
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);

  smtpData.setSender("ESP32", emailSenderAccount);

  smtpData.setPriority("High");

  smtpData.setSubject(emailSubject);

  smtpData.setMessage(emailMessage, true);

  smtpData.addRecipient(inputMessage);

  smtpData.setSendCallback(sendCallback);

  if (!MailClient.sendMail(smtpData)) {
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
    return false;
  }
  smtpData.empty();
  return true;
}

void sendCallback(SendStatus msg) {
  Serial.println(msg.info());

  if (msg.success()) {
    Serial.println("----------------");
  }
}