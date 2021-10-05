#include "rpcWiFi.h"
#include <PubSubClient.h>
#include <WiFiClient.h>
#include "TFT_eSPI.h"
#include "LIS3DHTR.h"

const char *ssid = "wifi-ssid";      // your network SSID
const char *password = "wifi-password"; // your network password

const char *ID = "Wio-Terminal-Client";  // Name of our device, must be unique
const char *TOPIC = "WioTerminal";  // Topic to subcribe to
const char *subTopic = "inTopic";  // Topic to subcribe to

const char *server = "mqtt"; // Server URL

long lastMsg = 0;

int buzzerStatus = 0;
long buzzerStop = 0;

boolean buttonStatus[32];

LIS3DHTR<TwoWire> lis;
WiFiClient wifiClient;
PubSubClient client(wifiClient);
TFT_eSPI tft;
#define LCD_BACKLIGHT (72Ul) // Control Pin of LCD

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String parameters[10];
  int param_count = 0;
  parameters[0] = (char)0;
  for (int i = 0; i < length; i++)
  {
    char c = (char)payload[i];
    Serial.print(c);
    switch (c) {
      case '|':
        param_count++;
        parameters[param_count] = (char) 0;
        break;
      case '\n':
        process(parameters, param_count+1);
        param_count=0;
        parameters[0] = (char) 0;
        break;
      default:
        parameters[param_count]+= c;
    }
  }

  // Process last command
  process(parameters, param_count+1);

}

void error(String errorMessage, String parameter[]) {
  Serial.print("ERROR ");
  Serial.print(errorMessage);
  Serial.print(" for command ");
  Serial.println(parameter[0]);
}

int getColor(String color) {
  if (color=="BLACK") return TFT_BLACK;
  if (color=="NAVY") return TFT_NAVY;
  if (color=="DARKGREEN") return TFT_DARKGREEN;
  if (color=="DARKCYAN") return TFT_DARKCYAN;
  if (color=="MAROON") return TFT_MAROON;
  if (color=="PURPLE") return TFT_PURPLE;
  if (color=="OLIVE") return TFT_OLIVE;
  if (color=="LIGHTGREY") return TFT_LIGHTGREY;
  if (color=="DARKGREY") return TFT_DARKGREY;
  if (color=="BLUE") return TFT_BLUE;
  if (color=="GREEN") return TFT_GREEN;
  if (color=="CYAN") return TFT_CYAN;
  if (color=="RED") return TFT_RED;
  if (color=="MAGENTA") return TFT_MAGENTA;
  if (color=="YELLOW") return TFT_YELLOW;
  if (color=="WHITE") return TFT_WHITE;
  if (color=="ORANGE") return TFT_ORANGE;
  if (color=="GREENYELLOW") return TFT_GREENYELLOW;
  if (color=="PINK") return TFT_PINK;
//  if (color=="BROWN") return TFT_BROWN;
//  if (color=="GOLD") return TFT_GOLD;
//  if (color=="SILVER") return TFT_SILVER;
//  if (color=="SKYBLUE") return TFT_SKYBLUE;
//  if (color=="VIOLET") return TFT_VIOLET;

  // Fallback
  return TFT_BLACK;
}

void startBuzzer(int freq, int duration) {
  analogWrite(WIO_BUZZER, freq);
  buzzerStatus = 1;
  long now = millis();
  buzzerStop = now + duration * 1000;
}

void stopBuzzer() {
  analogWrite(WIO_BUZZER, 0);
  buzzerStatus = 0;
}

void setDisplayBacklight(boolean b) {
  if (b) {
    digitalWrite(LCD_BACKLIGHT, HIGH);
  } else {
    digitalWrite(LCD_BACKLIGHT, LOW);
  }
}

void process(String parameter[], int param_count) {
  Serial.println("Received ");
  Serial.print(param_count);
  Serial.println(" parameters to process: ");
  for (int i=0; i<param_count; i++) {
    Serial.print("Param ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(parameter[i]);
  }
  String command = parameter[0];
  if (command=="clear") {
      tft.fillScreen(TFT_BLACK);
  } else if (command=="playBuzzer") {
    if (param_count<2) {
      error("Parameter count incorrect for playBuzzer (must be 2)", parameter);
      return;
    }
    startBuzzer(parameter[1].toInt(), parameter[2].toInt());
  } else if (command=="stopBuzzer") {
    stopBuzzer();
  } else if (command=="setDisplayBacklight") {
    if (param_count<1) {
      error("Parameter count incorrect for setDisplayBacklight (must be 1)", parameter);
      return;
    }
    setDisplayBacklight((boolean)parameter[1].toInt());
  } else if (command=="setRotation") {
    if (param_count<1) {
      error("Parameter count incorrect for setRotation (must be 1)", parameter);
      return;
    }
    tft.setRotation(parameter[1].toInt());
  } else if (command=="fillScreen") {
    if (param_count<1) {
      error("Parameter count incorrect for fillScreen (must be 1)", parameter);
      return;
    }
    tft.fillScreen(getColor(parameter[1]));
  } else if (command=="setTextColor") {
    if (param_count<1) {
      error("Parameter count incorrect for setTextColor (must be 1)", parameter);
      return;
    }
    tft.setTextColor(getColor(parameter[1]));
  } else if (command=="setTextSize") {
    if (param_count<1) {
      error("Parameter count incorrect for setTextSize (must be 1)", parameter);
      return;
    }
    tft.setTextSize(parameter[1].toInt());
  } else if (command=="drawString") {
    if (param_count<2) {
      error("Parameter count incorrect for drawString (must be 3)", parameter);
      return;
    }
    tft.drawString(parameter[1], parameter[2].toInt(), parameter[3].toInt());
  } else if (command=="drawNumber") {
    if (param_count<2) {
      error("Parameter count incorrect for drawNumber (must be 3)", parameter);
      return;
    }
    tft.drawNumber(parameter[1].toInt(), parameter[2].toInt(), parameter[3].toInt());
  } else if (command=="drawFastHLine") {
    if (param_count<3) {
      error("Parameter count incorrect for drawFastHLine (must be 4)", parameter);
      return;
    }
    tft.drawFastHLine(parameter[1].toInt(), parameter[2].toInt(), parameter[3].toInt(), getColor(parameter[4]));
  } else if (command=="drawFastVLine") {
    if (param_count<3) {
      error("Parameter count incorrect for drawFastVLine (must be 4)", parameter);
      return;
    }
    tft.drawFastVLine(parameter[1].toInt(), parameter[2].toInt(), parameter[3].toInt(), getColor(parameter[4]));
  } else if (command=="fillRect") {
    if (param_count<4) {
      error("Parameter count incorrect for fillRect (must be 5)", parameter);
      return;
    }
    tft.fillRect(parameter[1].toInt(), parameter[2].toInt(),
                 parameter[3].toInt(), parameter[4].toInt(), getColor(parameter[5]));
  } else {
    error("Unknown command", parameter);
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(TOPIC, "{\"message\": \"Wio Terminal is connected!\"}");
      Serial.println("Published connection message successfully!");
      // ... and resubscribe
      client.subscribe(subTopic);
      Serial.print("Subcribed to: ");
      Serial.println(subTopic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  pinMode(WIO_BUZZER, OUTPUT);
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);

  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial); // Wait for Serial to be ready
  delay(1000);

  lis.begin(Wire1);

  if (!lis) {
    Serial.println("ERROR");
    while(1);
  }
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); //Data output rate
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); //Scale range set to 2g

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    WiFi.begin(ssid, password);
    // wait 1 second for re-trying
    delay(1000);
  }
  Serial.print("Connected to ");
  Serial.println(ssid);

  IPAddress ip = WiFi.localIP();

  Serial.print("IP: ");
  Serial.println(ip);

  client.setServer(server, 1883);
  client.setCallback(callback);
  client.setBufferSize(16384);
}

void checkButton(int button, String buttonName) {
  int buttonInArray = button-28;
  boolean buttonCurrentStatus = digitalRead(button) == LOW;
  boolean buttonPreviousStatus = buttonStatus[buttonInArray];
  if (buttonCurrentStatus != buttonPreviousStatus) {
    String data = String(buttonCurrentStatus);
    String topic = String(TOPIC) + "/" + buttonName;
    Serial.print("Output button info:");
    Serial.println(topic);

    if (!client.publish(topic.c_str(), data.c_str())) {
      Serial.println("Message failed to send.");
    }

    buttonStatus[buttonInArray] = buttonCurrentStatus;
  }
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  float x_values, y_values, z_values;

  // Sending Data
  long now = millis();

  if (buzzerStatus && now > buzzerStop) {
     stopBuzzer();
  }

  if (now - lastMsg > 5000) {
    lastMsg = now;

    x_values = lis.getAccelerationX();
    y_values = lis.getAccelerationY();
    z_values = lis.getAccelerationZ();
    String data="{\"x-axis\": "+String(x_values)+","+"\"y-axis\": "+String(y_values)+","+"\"z-axis\": "+String(z_values)+"}";

    if (!client.publish(TOPIC, data.c_str())) {
      Serial.println("Message failed to send.");
    }

    Serial.printf("Message Send [%s] ", TOPIC);
    Serial.println(data);
  }

  checkButton(WIO_5S_UP,    "5way_button_up");
  checkButton(WIO_5S_DOWN,  "5way_button_down");
  checkButton(WIO_5S_LEFT,  "5way_button_left");
  checkButton(WIO_5S_RIGHT, "5way_button_right");
  checkButton(WIO_5S_PRESS, "5way_button_press");

  checkButton(WIO_KEY_A, "key_A");
  checkButton(WIO_KEY_B, "key_B");
  checkButton(WIO_KEY_C, "key_C");

  client.loop();
}
