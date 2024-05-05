/*
    Library Imports
*/
#include "Arduino.h"
#include "WifiManager.h"
#include <AsyncMqttClient.h>
#include <TaskScheduler.h>
#include <ArduinoHttpClient.h>

#define XSTR(x) #x
#define STR(x) XSTR(x)

// #define DEBUG_
#define ENABLE_MONGODB
#define MOCK_SENSORS

#define MQTT_HOST   "192.168.6.208"
#define MQTT_PORT   8080

#define MONGODB_HOST "192.168.6.208"
#define MONGODB_PORT 8081

/*
    Static Globals
*/
Scheduler ts;
AsyncMqttClient mqttClient;
WiFiClient wifi;
HttpClient client = HttpClient(wifi, MONGODB_HOST, MONGODB_PORT);



char* WIFI_MANAGEMENT_SSID = "WIFI_MANAGER";
int WIFI_TRY = 5;

String contentType = "application/json";

int digitalTempAnalogPin = 32;
int analogMicrophonePin = 39;
int analogLightPin = 38;
int analogPressurePin = 37;
int analogWaterPin = 36;
int analogHumidityPin = 35;


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

    mqttClient.subscribe("hello", 0);
    return true;
};

void sendMqtt() {
    mqttClient.publish("hello", 0, true, "hello");
};


// bool sendHTTPOE();
// void sendHTTP();
// Task tTestHTTP(TASK_IMMEDIATE, TASK_FOREVER, &sendHTTP, &ts, true, &sendHTTPOE );

// bool sendHTTPOE() {
//     return true;
// };

// void sendHTTP() {
//     if (WiFi.status() != WL_CONNECTED) { tTestHTTP.delay(3000); return;}
    
//     if (client->connecting()) { tTestHTTP.delay(300); return;}

//     // if (client->connected()) {

//     //     char* buf = "POST / HTTP/1.1\nHost: 192.168.6.208\nAccept: application/json\nContent-Type: application/json\nContent-Length: 16\n\n{\"key\": \"value\"}";
//     //     client->add(buf, strlen(buf));
//     //     client->send();
//     //     tTestHTTP.delay(3000);
//     // }

//     if (client->disconnected()) {
//         client->connect(MONGODB_HOST, MONGODB_PORT);
//         tTestHTTP.delay(1000);
//     }

// };





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


    #ifdef ENABLE_MONGODB
    if (WiFi.status() == WL_CONNECTED) {        
        String buf2 = "{ \"collection\":\"temperature\", \"database\":\"sensors\", \"dataSource\":\"GrafanaChallenge\", \"document\": { \"value\": \"";

        String postData = buf2 + buf + String("\" } }");

        client.beginRequest();
        client.post("/app/data-oszocdb/endpoint/data/v1/action/insertOne");
        client.sendHeader("Content-Type", "application/json");
        client.sendHeader("Content-Length", postData.length());
        client.sendHeader("Host", "eu-central-1.aws.data.mongodb-api.com");
        client.sendHeader("api-key", "ZYCscf2x3dpZE36bIE7nKO0uxdRnUqU1maLvwsSsSkRwMz6PtxHapcLwbqalwhAy");
        client.beginBody();
        client.print(postData);
        client.endRequest();
    }
    #endif
    

    #endif

};





bool readMicroOE();
void readMicro();
Task tMicrphone(TASK_SECOND, TASK_FOREVER, &readMicro, &ts, true, &readMicroOE);

bool readMicroOE() {

    #ifndef MOCK_SENSORS
    pinMode(analogMicrophonePin, INPUT);
    mqttClient.subscribe("noise", 0);
    #endif

    return true;
};

void readMicro() {

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

    #ifdef ENABLE_MONGODB
    if (WiFi.status() == WL_CONNECTED) {        
        String buf2 = "{ \"collection\":\"noise\", \"database\":\"sensors\", \"dataSource\":\"GrafanaChallenge\", \"document\": { \"value\": \"";

        String postData = buf2 + buf + String("\" } }");

        client.beginRequest();
        client.post("/app/data-oszocdb/endpoint/data/v1/action/insertOne");
        client.sendHeader("Content-Type", "application/json");
        client.sendHeader("Content-Length", postData.length());
        client.sendHeader("Host", "eu-central-1.aws.data.mongodb-api.com");
        client.sendHeader("api-key", "ZYCscf2x3dpZE36bIE7nKO0uxdRnUqU1maLvwsSsSkRwMz6PtxHapcLwbqalwhAy");
        client.beginBody();
        client.print(postData);
        client.endRequest();
    }
    #endif

    #endif

};





bool readLightOE();
void readLight();
Task tLight(TASK_SECOND, TASK_FOREVER, &readLight, &ts, true, &readLightOE);

bool readLightOE() {

    #ifndef MOCK_SENSORS
    pinMode(analogLightPin, INPUT);
    mqttClient.subscribe("light", 0);
    #endif

    return true;
};

void readLight() {

    #ifndef MOCK_SENSORS
    
    int V_input = analogRead(analogLightPin);

    Serial.print("Raw: ");
    Serial.println(V_input);

    char buf[8];
    String(V_input).toCharArray(buf, 8);
    mqttClient.publish("light", 1, true, buf);

    #else

    char buf[6];
    (String(random(0,100))+String(".")+String(random(0,99))).toCharArray(buf, 6);
    mqttClient.publish("light", 1, true, buf);

    #ifdef ENABLE_MONGODB
    if (WiFi.status() == WL_CONNECTED) {        
        String buf2 = "{ \"collection\":\"light\", \"database\":\"sensors\", \"dataSource\":\"GrafanaChallenge\", \"document\": { \"value\": \"";

        String postData = buf2 + buf + String("\" } }");

        client.beginRequest();
        client.post("/app/data-oszocdb/endpoint/data/v1/action/insertOne");
        client.sendHeader("Content-Type", "application/json");
        client.sendHeader("Content-Length", postData.length());
        client.sendHeader("Host", "eu-central-1.aws.data.mongodb-api.com");
        client.sendHeader("api-key", "ZYCscf2x3dpZE36bIE7nKO0uxdRnUqU1maLvwsSsSkRwMz6PtxHapcLwbqalwhAy");
        client.beginBody();
        client.print(postData);
        client.endRequest();
    }
    #endif

    #endif

};



bool readPressureOE();
void readPressure();
Task tPressure(TASK_SECOND, TASK_FOREVER, &readPressure, &ts, true, &readPressureOE);

bool readPressureOE() {

    #ifndef MOCK_SENSORS
    pinMode(analogPressurePin, INPUT);
    mqttClient.subscribe("pressure", 0);
    #endif

    return true;
};

void readPressure() {

    #ifndef MOCK_SENSORS
    
    int V_input = analogRead(analogPressurePin);

    Serial.print("Raw: ");
    Serial.println(V_input);

    char buf[8];
    String(V_input).toCharArray(buf, 8);
    mqttClient.publish("pressure", 1, true, buf);

    #else

    char buf[6];
    (String("1.00")+String(random(0,5))).toCharArray(buf, 6);
    mqttClient.publish("pressure", 1, true, buf);

    #ifdef ENABLE_MONGODB
    if (WiFi.status() == WL_CONNECTED) {        
        String buf2 = "{ \"collection\":\"pressure\", \"database\":\"sensors\", \"dataSource\":\"GrafanaChallenge\", \"document\": { \"value\": \"";

        String postData = buf2 + buf + String("\" } }");

        client.beginRequest();
        client.post("/app/data-oszocdb/endpoint/data/v1/action/insertOne");
        client.sendHeader("Content-Type", "application/json");
        client.sendHeader("Content-Length", postData.length());
        client.sendHeader("Host", "eu-central-1.aws.data.mongodb-api.com");
        client.sendHeader("api-key", "ZYCscf2x3dpZE36bIE7nKO0uxdRnUqU1maLvwsSsSkRwMz6PtxHapcLwbqalwhAy");
        client.beginBody();
        client.print(postData);
        client.endRequest();
    }
    #endif

    #endif

};


bool readWaterOE();
void readWater();
Task tWater(TASK_SECOND, TASK_FOREVER, &readWater, &ts, true, &readWaterOE);

bool readWaterOE() {

    #ifndef MOCK_SENSORS
    pinMode(analogWaterPin, INPUT);
    mqttClient.subscribe("water", 0);
    #endif

    return true;
};

void readWater() {

    #ifndef MOCK_SENSORS
    
    int V_input = analogRead(analogWaterPin);

    Serial.print("Raw: ");
    Serial.println(V_input);

    char buf[8];
    String(V_input).toCharArray(buf, 8);
    mqttClient.publish("water", 1, true, buf);

    #else

    char buf[6];
    (String(random(0,100))+String(".")+String(random(0,99))).toCharArray(buf, 6);
    mqttClient.publish("water", 1, true, buf);

    #ifdef ENABLE_MONGODB
    if (WiFi.status() == WL_CONNECTED) {        
        String buf2 = "{ \"collection\":\"water\", \"database\":\"sensors\", \"dataSource\":\"GrafanaChallenge\", \"document\": { \"value\": \"";

        String postData = buf2 + buf + String("\" } }");

        client.beginRequest();
        client.post("/app/data-oszocdb/endpoint/data/v1/action/insertOne");
        client.sendHeader("Content-Type", "application/json");
        client.sendHeader("Content-Length", postData.length());
        client.sendHeader("Host", "eu-central-1.aws.data.mongodb-api.com");
        client.sendHeader("api-key", "ZYCscf2x3dpZE36bIE7nKO0uxdRnUqU1maLvwsSsSkRwMz6PtxHapcLwbqalwhAy");
        client.beginBody();
        client.print(postData);
        client.endRequest();
    }
    #endif

    #endif

};


bool readHumidityOE();
void readHumidity();
Task tHumidity(TASK_SECOND, TASK_FOREVER, &readHumidity, &ts, true, &readHumidityOE);

bool readHumidityOE() {

    #ifndef MOCK_SENSORS
    pinMode(analogHumidityPin, INPUT);
    mqttClient.subscribe("humidity", 0);
    #endif

    return true;
};

void readHumidity() {

    #ifndef MOCK_SENSORS
    
    int V_input = analogRead(analogHumidityPin);

    Serial.print("Raw: ");
    Serial.println(V_input);

    char buf[8];
    String(V_input).toCharArray(buf, 8);
    mqttClient.publish("humidity", 1, true, buf);

    #else

    char buf[6];
    (String(random(0,100))+String(".")+String(random(0,99))).toCharArray(buf, 6);
    mqttClient.publish("humidity", 1, true, buf);

    #ifdef ENABLE_MONGODB
    if (WiFi.status() == WL_CONNECTED) {        
        String buf2 = "{ \"collection\":\"humidity\", \"database\":\"sensors\", \"dataSource\":\"GrafanaChallenge\", \"document\": { \"value\": \"";

        String postData = buf2 + buf + String("\" } }");

        client.beginRequest();
        client.post("/app/data-oszocdb/endpoint/data/v1/action/insertOne");
        client.sendHeader("Content-Type", "application/json");
        client.sendHeader("Content-Length", postData.length());
        client.sendHeader("Host", "eu-central-1.aws.data.mongodb-api.com");
        client.sendHeader("api-key", "ZYCscf2x3dpZE36bIE7nKO0uxdRnUqU1maLvwsSsSkRwMz6PtxHapcLwbqalwhAy");
        client.beginBody();
        client.print(postData);
        client.endRequest();
    }
    #endif

    #endif

};