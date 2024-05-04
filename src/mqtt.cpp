/*
    MQTT
*/
/*
    1. Wait for Wifi Connection -> 2
    2. Then connect to mqtt server -> 3
    3. Constant check for mqtt connection -> 4
    4. Disconnect from mqtt server when network disconnected -> 1
*/
bool mqttSetup();
void mqttWaitForWifi();
void mqttConnect();
void mqttWaitForConnect();
void mqttCheck();
void mqttDisconnect();
Task tMqtt ( TASK_IMMEDIATE, TASK_FOREVER, &mqttWaitForWifi, &ts, true, &mqttSetup );

bool mqttSetup() {
    Serial.println("Mqtt setup");
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    tMqtt.delay(5000);
    return true;
}

void mqttWaitForWifi() {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Wifi connected, connecting mqtt");
        tMqtt.setCallback(&mqttConnect);
    }
    else {Serial.println("wifi not connected");}
    tMqtt.delay(TASK_SECOND * 2);
};

void mqttConnect() {
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.connect();
    tMqtt.delay(2000);
    tMqtt.setCallback(&mqttWaitForConnect);
};

void mqttWaitForConnect() {
    if (mqttClient.connected()) {
        Serial.println("Mqtt server connected");
        tMqtt.setCallback(&mqttCheck);
    }
    else {Serial.println("Waiting for connection...");}
    tMqtt.delay(TASK_SECOND*2);
}

void mqttCheck() {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Mqtt server connected");
    }
    else {
        Serial.println("Mqtt server disconnected");
        tMqtt.setCallback(&mqttDisconnect);
    }
    tMqtt.delay(TASK_SECOND*5);
};

void mqttDisconnect() {
    mqttClient.disconnect(true);
    tMqtt.setCallback(&mqttWaitForWifi);
    tMqtt.delay(3000);
};