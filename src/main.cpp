/*
    Library Imports
*/
#include "Arduino.h"
#include "WifiManager.h"
#include <AsyncMqttClient.h>
// #include <ArduinoJson.h>
#include <TaskScheduler.h>

#define XSTR(x) #x
#define STR(x) XSTR(x)

/*
    Static Globals
*/
Scheduler ts;
AsyncMqttClient mqttClient;
Task sensors[5];

// #define DEBUG_

char* WIFI_MANAGEMENT_SSID = "WIFI_MANAGER";
int WIFI_TRY = 5;
#define MQTT_HOST IPAddress(192, 168, 6, 208)
#define MQTT_PORT 1883



#include "wifi.cpp"

#include "mqtt.cpp"

void onMqttConnect(bool _x) {
    Serial.println("mqtt connected!");
}

void setup() {
    Serial.begin(115200);
    while(!Serial){delay(100);}
    mqttClient.onConnect(onMqttConnect);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
};

void loop() {
    ts.execute();
};


bool readDigitalTempOE();
void readDigitalTemp();
Task digitalTemperature(TASK_SECOND, TASK_FOREVER, &readDigitalTemp, &ts, false, &readDigitalTempOE);

int digitalTempAnalogPin = 32;
int digitalTempDigitalPin = 39;


bool readDigitalTempOE() {

    pinMode(digitalTempAnalogPin, INPUT);
    pinMode(digitalTempDigitalPin, INPUT);
    mqttClient.subscribe("esp32/temp", 0);

    return true;
};

void readDigitalTemp() {

    // Read value from module connected at pin A0
    int V_input = analogRead(digitalTempAnalogPin);
    // calculate thermometer resistance by comparing it to the modules onboard 10K-ohm resistor
    float R_ohms = 10000 * (1023.0 / (float)V_input - 1.0);
    // calculate temperature in Celsius
    float temp_C = (1.0 / (0.001129148 + (0.000234125*log(R_ohms)) + 0.0000000876741*log(R_ohms)*log(R_ohms)*log(R_ohms)))-273.15;
    // output result to serial

    Serial.print("Temperature: "); 
    Serial.print(temp_C);
    Serial.println(" C");
    Serial.print("Raw: "); 
    Serial.println(V_input);

    mqttClient.publish("esp32/temp", 0, true, dtostrf(temp_C));
    


    // Serial.printf("Analog Temp: %d\nThreshold: %d\n",
    //     analogRead(digitalTempAnalogPin),
    //     digitalRead(digitalTempDigitalPin)
    // );

};


