#ifndef __DEMO_MQTT_H_
#define __DEMO_MQTT_H_


#define BROKER_IP  "iot-servercu.alascloud.com"
#define BROKER_PORT 1883
#define CLIENT_ID  "54569640-1c31-11ee-88e6-71f9c89d69cd"
#define CLIENT_USER "XOrel5W8zDZxmya7Z9eY"
#define CLIENT_PASS ""
#define SUBSCRIBE_TOPIC "$v1/devices/me/attributes"
#define PUBLISH_TOPIC   "$v1/devices/me/telemetry"
#define PUBLISH_INTERVAL 10000 //10s
#define PUBLISH_PAYLOEAD "{\"C_SDK\": \"A9G\"}"


#endif

