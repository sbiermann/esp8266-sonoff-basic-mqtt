/*************************************************/
/* Debugging                                     */
/*************************************************/
const bool debugOutput = true;  // set to true for serial OUTPUT

/*************************************************/
/* Settings for WLAN                             */
/*************************************************/
const char* ssid = "SSID";
const char* password = "ergerg123erg";


/*************************************************/
/* Update settings                               */
/*************************************************/ 
const char* firmware_version = "mysonoff_0.0.1";
const char* update_server = "myhost";
const char* update_uri = "/path/update.php";

/*************************************************/
/* MQTTCloud data                               */
/*************************************************/
const char* mqtt_host = "mqtthost";
const char* mqtt_id = "ESP8266-Sonoff";
const char* mqtt_topic_state = "sonoff/state";
const char* mqtt_topic_set = "sonoff/set";
const int mqtt_port = 1883;

