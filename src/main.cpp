#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <string.h>
#include <FW_updater.hpp>
#include <MQTT_client.hpp>
#include <MD5.hpp>
#include <Servo_motor.hpp>

// debug mode, set to 0 if making a release
#define DEBUG 1

// Logging macro used in debug mode
#if DEBUG == 1
#define LOG(message) Serial.println(message);
#else
#define LOG(message)
#endif

////////////////////////////////////////////////////////////////////////////////
/// CONSTANT DEFINITION
////////////////////////////////////////////////////////////////////////////////

#define WIFI_SSID    "codefactory"
#define WIFI_PASS    "rrv2WxFY"

#define MODULE_TYPE  "SERVO_MOTOR"

#define LOOP_DELAY_MS   10u
#define FW_UPDATE_PORT  5000u

////////////////////////////////////////////////////////////////////////////////
/// GLOBAL OBJECTS
////////////////////////////////////////////////////////////////////////////////

static String module_mac;

static FW_updater *fw_updater = nullptr;
static MQTT_client *mqtt_client = nullptr;
static std::vector<Servo_motor> servo_motors;

static bool standby_mode = false;

static void resolve_mqtt(String &topic, String &payload);

////////////////////////////////////////////////////////////////////////////////
/// SETUP
////////////////////////////////////////////////////////////////////////////////

void setup() {
#if DEBUG == true
  Serial.begin(115200);
#endif
  delay(10);

  module_mac = WiFi.macAddress();
  LOG("Module MAC: " + module_mac);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
    delay(500);
  LOG("Connected to Wi-Fi AP");

  const String gateway_ip = WiFi.gatewayIP().toString();
  LOG("GW IP address: " + gateway_ip);

  // firmware server expected to run on GW
  fw_updater = new FW_updater(gateway_ip.c_str(), FW_UPDATE_PORT);

  // MQTT broker expected to run on GW
  mqtt_client = new MQTT_client(gateway_ip.c_str());
  mqtt_client->setup_mqtt(module_mac.c_str(), MODULE_TYPE, resolve_mqtt);
  LOG("Connected to MQTT broker");
  mqtt_client->publish_module_id();
  LOG("Subscribing to ALL_MODULES ...");
  mqtt_client->subscribe("ALL_MODULES");
  LOG("Subscribing to " + module_mac + "/SET_CONFIG ...");
  mqtt_client->subscribe((module_mac + "/SET_CONFIG").c_str(), 2u);
  LOG("Subscribing to " + module_mac + "/SET_VALUE ...");
  mqtt_client->subscribe((module_mac + "/SET_VALUE").c_str(), 2u);
  LOG("Subscribing to " + module_mac + "/UPDATE_FW ...");
  mqtt_client->subscribe((module_mac + "/UPDATE_FW").c_str(), 2u);
  LOG("Subscribing to " + module_mac + "/REQUEST ...");
  mqtt_client->subscribe((module_mac + "/REQUEST").c_str(), 2u);
}

////////////////////////////////////////////////////////////////////////////////
/// LOOP
////////////////////////////////////////////////////////////////////////////////

void loop() {
  mqtt_client->loop();

  if (!servo_motors.empty() && !standby_mode) {
    DynamicJsonDocument json(1024);

    for (Servo_motor &motor : servo_motors) {
      if (motor.poll()) {
        char uuid[strlen(motor.get_uuid().c_str())];
        sprintf(uuid, "%s", motor.get_uuid().c_str());
        JsonObject motor_data = json.createNestedObject(uuid);
        motor_data["POSITION"] = motor.get_current_position();
      }
    }

    if (!json.isNull()) {
#if DEBUG == 1
      String json_log;
      serializeJson(json, json_log);
      LOG(json_log);
#endif
      mqtt_client->publish_value_update(json);
    }
  }

  delay(LOOP_DELAY_MS);
}

////////////////////////////////////////////////////////////////////////////////
/// MQTT RESOLVER
////////////////////////////////////////////////////////////////////////////////

static void resolve_mqtt(String &topic, String &payload) {

  LOG("Received message: " + topic + " - " + payload);

  DynamicJsonDocument payload_json(256);
  DeserializationError json_err = deserializeJson(payload_json, payload);

  if (json_err) {
    LOG("JSON error: " + String(json_err.c_str()));
    return;
  }

  if (topic.equals("ALL_MODULES") || topic.equals(module_mac + "/REQUEST")) {
    const char *request = payload_json["request"];

    if (request != nullptr) {
      if (String(request) == "module_discovery")
        mqtt_client->publish_module_id();
      else if (String(request) == "stop") {
        const uint16_t sequence_number = payload_json["sequence_number"];

        LOG("Switching to standby mode");
        // switch to standby mode
        standby_mode = true;
        mqtt_client->publish_request_result(sequence_number, true);
      } else if (String(request) == "start") {
        const uint16_t sequence_number = payload_json["sequence_number"];

        LOG("Switching to active mode");
        // switch to active mode
        standby_mode = false;
        mqtt_client->publish_request_result(sequence_number, true);
      }
    }
  } else if (topic.equals(module_mac + "/SET_CONFIG")) {
    JsonObject json_config = payload_json.as<JsonObject>();
    if (!servo_motors.empty()) {
      LOG("Deleting previous configuration");
      std::vector<Servo_motor>().swap(servo_motors);
    }

    // create devices according to received configuration
    for (const JsonPair &pair : json_config) {
      const char *device_uuid = pair.key().c_str();
      const JsonObject device_config = pair.value().as<JsonObject>();
      // read the the Servo motor signal pin
      const uint8_t signal_pin = device_config["address"];
      const uint16_t poll_rate = device_config["poll_rate"];

      LOG("Creating device with parameters: ");
      LOG(String("\t uuid:\t") + device_uuid);
      LOG(String("\t address:\t") + signal_pin);
      LOG(String("\t interval_rate:\t") + ((poll_rate * 1000) / LOOP_DELAY_MS));

      servo_motors.emplace_back(device_uuid, signal_pin, ((poll_rate * 1000) / LOOP_DELAY_MS));
    }

    // calculate config MD5 chuecksum
    std::string payload_cpy(payload.c_str());
    const std::string &md5_str = MD5::make_digest(MD5::make_hash(&payload_cpy[0]), 16);

    LOG(String("Config MD5 checksum: ") + md5_str.c_str());

    mqtt_client->publish_config_update(md5_str);
  } else if (topic.equals(module_mac + "/SET_VALUE")) {
    const char *device_uuid = payload_json["device_uuid"];
    const char *datapoint = payload_json["datapoint"];
    const uint16_t sequence_number = payload_json["sequence_number"];
    const uint8_t value = payload_json["value"];

    LOG("Setting value:");
    LOG(String("\t device_uuid: ") + device_uuid);
    LOG(String("\t datapoint: ") + datapoint);
    LOG(String("\t value: ") + value);

    for (Servo_motor &motor: servo_motors) {
      if (motor.get_uuid() == device_uuid) {
        int ok = motor.resolve_datapoint(datapoint, value);
        LOG("resolved")
        LOG(ok)
        if (ok != 0) mqtt_client->publish_request_result(sequence_number, false, "unknown datapoint");
        else mqtt_client->publish_request_result(sequence_number, true);
      }
    }
//    mqtt_client->publish_request_result(sequence_number, false, "no matching device");
  } else if (topic.equals(module_mac + "/UPDATE_FW")) {
    const char *version = payload_json["version"];
    const uint16_t sequence_number = payload_json["sequence_number"];

    LOG(String("Updating firmware to version: ") + version);
    bool result = fw_updater->update(version);
    String log_msg = result ? "\t result: ok" : "\t result: error";
    LOG(log_msg);

    mqtt_client->publish_request_result(sequence_number, result);
  }
}
