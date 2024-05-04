/*
    Library Imports
*/
#include "Arduino.h"
#include "WifiManager.h"
#include <AsyncMqttClient.h>
#include <TaskScheduler.h>

#define XSTR(x) #x
#define STR(x) XSTR(x)


/*
    Static Globals
*/
Scheduler ts;
AsyncMqttClient mqttClient;

// #define DEBUG_
#define MOCK_SENSORS

char* WIFI_MANAGEMENT_SSID = "WIFI_MANAGER";
int WIFI_TRY = 5;

#define MQTT_HOST   "192.168.6.208"
#define MQTT_PORT   8080

int digitalTempAnalogPin = 32;
int analogMicrophonePin = 39;


#include "wifi.cpp"
#include "mqtt.cpp"


void setup() {
    Serial.begin(115200);
    while(!Serial){delay(100);}
};

void loop() {
    ts.execute();
};


bool sendMqttOE();
void sendMqtt();
Task tTestMqtt(TASK_SECOND * 3, TASK_FOREVER, &sendMqtt, &ts, true, &sendMqttOE );

bool sendMqttOE() {

    mqttClient.subscribe("test/hello", 0);
    return true;
};

void sendMqtt() {
    mqttClient.publish("test/hello", 0, true, "hello");
};



bool readDigitalTempOE();
void readDigitalTemp();
Task tDigitalTemperature(TASK_SECOND, TASK_FOREVER, &readDigitalTemp, &ts, true, &readDigitalTempOE);

bool readDigitalTempOE() {
    #ifndef MOCK_SENSORS
    pinMode(digitalTempAnalogPin, INPUT);
    mqttClient.subscribe("temperature", 0);
    #endif

    return true;
};

void readDigitalTemp() {

    #ifndef MOCK_SENSORS
    // Read value from module connected at pin A0
    int V_input = analogRead(digitalTempAnalogPin);
    // calculate thermometer resistance by comparing it to the modules onboard 10K-ohm resistor
    float R_ohms = 10000 * (1023.0 / (float)V_input - 1.0);
    // calculate temperature in Celsius
    float temp_C = (1.0 / (0.001129148 + (0.000234125*log(R_ohms)) + 0.0000000876741*log(R_ohms)*log(R_ohms)*log(R_ohms)))-273.15;
    // output result to serial

    char str[8];
    dtostrf(temp_C, 5, 2, str);
    mqttClient.publish("temperature", 1, true, str);

    #else

    char buf[6];
    (String(random(23,25))+String(".")+String(random(0,99))).toCharArray(buf, 6);
    mqttClient.publish("temperature", 1, true, buf);

    #endif

};





bool readDigitalMicroOE();
void readDigitalMicro();
Task tMicrphone(TASK_SECOND, TASK_FOREVER, &readDigitalMicro, &ts, true, &readDigitalMicroOE);

bool readDigitalMicroOE() {

    #ifndef MOCK_SENSORS
    pinMode(analogMicrophonePin, INPUT);
    mqttClient.subscribe("noise", 0);
    #endif

    return true;
};

void readDigitalMicro() {

    #ifndef MOCK_SENSORS
    
    int V_input = analogRead(analogMicrophonePin);

    Serial.print("Raw: ");
    Serial.println(V_input);

    char buf[8];
    String(V_input).toCharArray(buf, 8);
    mqttClient.publish("noise", 1, true, buf);

    #else

    char buf[6];
    (String(random(30,70))+String(".")+String(random(0,99))).toCharArray(buf, 6);
    mqttClient.publish("noise", 1, true, buf);

    #endif

};





