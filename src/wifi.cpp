/*
    Wifi Manager
*/
/*
    Check for connection
    if yes: resume normal
    if no: try reconnect
    if no reconnect: launch management server
    if reconnect: resume normal and check for connection again
*/

void wifiCheckConnection();
void wifiTryReconnect();
void wifiLaunchManagementServer();
Task tWifi ( TASK_IMMEDIATE, TASK_FOREVER, &wifiCheckConnection, &ts, true );

void wifiCheckConnection() {
    #ifdef DEBUG_
    Serial.print("Checking wifi connection.\n");
    #endif

    if (WiFi.status() == WL_CONNECTED) {
        #ifdef DEBUG_
        Serial.print("Wifi connected.\n");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        #endif
        tWifi.delay(5000U);
    }
    else {
        #ifdef DEBUG_
        Serial.print("Wifi not connected.\n");
        #endif
        WifiManager.connectToWifi(true);
        tWifi.setCallback(&wifiTryReconnect);
        tWifi.delay(1000U);
    }

};

int tries = 0;
void wifiTryReconnect() {

    if (WiFi.status() == WL_CONNECTED) {
        #ifdef DEBUG_
        Serial.print("Wifi reconnected.\n");
        Serial.print("Wifi IP: ");
        Serial.println(WifiManager.getIP());
        #endif
        tries = 0;
        tWifi.setCallback(&wifiCheckConnection);
        tWifi.delay(5000U);
    }
    else if (tries < WIFI_TRY) {
        #ifdef DEBUG_
        Serial.print("Retrying connection...\n");
        #endif
        tries++;
        tWifi.delay(1000);
    }
    else {
        #ifdef DEBUG_
        Serial.print("Couldn't connect to wifi. Launching Wifi Management web server on port 80.\n");
        #endif
        WifiManager.startNetworkScan();
        tWifi.delay(1000);
        tWifi.setCallback(&wifiLaunchManagementServer);
    }

};

bool wait_for_scan = true;
void wifiLaunchManagementServer() {

    if (wait_for_scan) {
        wait_for_scan = !WifiManager.checkNetworkScan();
        tWifi.delay(100);        
        return;
    }
    else {
        #ifdef DEBUG_
        Serial.print("Wifi Management server launched on port 80.\n");
        #endif
        WifiManager.startManagementServer(WIFI_MANAGEMENT_SSID, true);
        tWifi.disable();
    }
};