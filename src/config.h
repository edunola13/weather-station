#include <ESP8266WiFi.h>
// ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

struct Config {
  char name[20] = "Boiler";
  char access_key[30] = "";
  char mq_server[15] = "";
  char mq_client[15] = "";  // Can be unique
  char mq_user[10] = "";
  char mq_pass[10] = "";

  char ssid[30] = "";
  char passwd[30] = "";
  char ap_ssid[20] = "BoilerPlate";
  char ap_passwd[20] = "BoilerPlate";  // Min 8: dont work if <8

  bool staticIp = false;
  uint8_t ip[4] = {192, 168, 0, 53};
  uint8_t gateway[4] = {192, 168, 0, 1};
  uint8_t subnet[4] = {255, 255, 255, 0};
} config;

struct ConfigStatus {
  char status = 'I';  // I=Iniciando, C=Conectado/ConexionPerdida, A=Access Point
  IPAddress ip;
  long lastWifiTime = 0;
  long lastMqttTime = 0;
} status;

// BearSSL::ESP8266WebServerSecure http_rest_server_ssh(443);
ESP8266WebServer http_rest_server(HTTP_REST_PORT);

/*static const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIC6jCCAlOgAwIBAgIUV4hbn80tCIONpKF4Et6b3Ynq/X8wDQYJKoZIhvcNAQEL
BQAwejELMAkGA1UEBhMCUk8xCjAIBgNVBAgMAUIxEjAQBgNVBAcMCUJ1Y2hhcmVz
dDEbMBkGA1UECgwST25lVHJhbnNpc3RvciBbUk9dMRYwFAYDVQQLDA1PbmVUcmFu
c2lzdG9yMRYwFAYDVQQDDA1lc3A4MjY2LmxvY2FsMB4XDTE5MDkxNjAyMDAwM1oX
DTIwMDkxNTAyMDAwM1owejELMAkGA1UEBhMCUk8xCjAIBgNVBAgMAUIxEjAQBgNV
BAcMCUJ1Y2hhcmVzdDEbMBkGA1UECgwST25lVHJhbnNpc3RvciBbUk9dMRYwFAYD
VQQLDA1PbmVUcmFuc2lzdG9yMRYwFAYDVQQDDA1lc3A4MjY2LmxvY2FsMIGfMA0G
CSqGSIb3DQEBAQUAA4GNADCBiQKBgQDZvgnP4fGGNZ93VUcTlFUEFjumHMPeoQJk
UXlhbYR3pwRtSgpQjKElHe9X9GVOm9PwrHXc604woexcpEQDED297V1/FT5pU0i+
VWXGm1cSs33lR1//3XNfF5oi36HvJGP/Iqz8eH5oRDpfGvAIp9QEcL9S1CWJzidc
No+TiKGbnwIDAQABo20wazAdBgNVHQ4EFgQUDHDXgGd7kfoER5IJf4/g3izoEx8w
HwYDVR0jBBgwFoAUDHDXgGd7kfoER5IJf4/g3izoEx8wDwYDVR0TAQH/BAUwAwEB
/zAYBgNVHREEETAPgg1lc3A4MjY2LmxvY2FsMA0GCSqGSIb3DQEBCwUAA4GBALjo
8Tp9PcCmFrP6JcvnYWwxhIMutaEfFf9c1AX/FWIgmx1CUr+ewncR5heZKbzso8mF
1HmX9N2mukb/0gzOsPge5eSrdZjw56PDQkg4+MQN08Ha8E40SUK+OGpnPMwAVnvd
YPxoQ6GpFURq5rp5ru2E9+n+/9gFLkOVVEeLO8jN
-----END CERTIFICATE-----
)EOF";

static const char serverKey[] PROGMEM =  R"EOF(
-----BEGIN PRIVATE KEY-----
MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBANm+Cc/h8YY1n3dV
RxOUVQQWO6Ycw96hAmRReWFthHenBG1KClCMoSUd71f0ZU6b0/CsddzrTjCh7Fyk
RAMQPb3tXX8VPmlTSL5VZcabVxKzfeVHX//dc18XmiLfoe8kY/8irPx4fmhEOl8a
8Ain1ARwv1LUJYnOJ1w2j5OIoZufAgMBAAECgYEAiHgOCEeRK8+h8ZX2JTRbkGMq
4XK35Gm/aQaTb9fHJYL9SE4WZuOs/+liIBsh/4G09OvyNxMXf22NCYc+xTjBqIcQ
NFLeHlQtTskzW7oHTSfwV1KuOXrATuPENr0CtEo/pld5mLhEvX0lsvbd82xTt/9R
GvpxzxlRCHG+yAng84ECQQDsM5XZe+OVMC4CrmVUTxPykt8eqSnV9qjCX4dePq85
Es+NtZWWmZgujnnC9tGmbta0EHNidJg38JmKj2enw3TBAkEA6/5bHpbIEScMXySe
FE+a/Qo3UiT7Z9yVM1LPs4NQZFpJsf9vMoiEaQrfTQLUbjOMFzWkgZKrRvK4weGu
8ANIXwJANjopnwKoXynuhkMPlGmLRNefTeS8bBjy6Z0Q8PwnMk01RW146Fhe7eFb
5vzPaQxtUm2sb+Agykb8mSkPPR7MgQJBANJm3lpxWEJirBcPkJjPOIGt4BCuxC6f
ba5qgJ+tzbkK/nViJYPKTzNP7DK++SKfdqEixF55o5cHxE2nps56PsUCQGNeROlX
jR0oV96dz/cL0aZYc0mU88N1mWJNu8Dvz4OKREDPzNoWsirJikQNxb3daEBh6EFh
dyHrT0u/Tnxe9kQ=
-----END PRIVATE KEY-----
)EOF";*/

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  DEB_DO_PRINTLN("Message arrived [");
  DEB_DO_PRINTLN(topic);
  DEB_DO_PRINTLN("] ");
  for (unsigned int i = 0; i < length; i++) {
    DEB_DO_PRINT((char)payload[i]);
  }
  DEB_DO_PRINTLN();

  // DO SOMETHING
}

void initWifi() {
    WiFi.setAutoConnect(false);
    WiFi.disconnect();
    int retries = 0;

    DEB_DO_PRINTLN("Connecting to WiFi AP..........");

    if (config.staticIp) {
      IPAddress ip = IPAddress(config.ip[0], config.ip[1], config.ip[2], config.ip[3]);
      IPAddress gateway = IPAddress(config.gateway[0], config.gateway[1], config.gateway[2], config.gateway[3]);
      IPAddress subnet = IPAddress(config.subnet[0], config.subnet[1], config.subnet[2], config.subnet[3]);
      WiFi.config(ip, gateway, subnet);
    }
    WiFi.begin(config.ssid, config.passwd);
    WiFi.mode(WIFI_STA);
    // check the status of WiFi connection to be WL_CONNECTED
    while ((WiFi.status() != WL_CONNECTED) && (retries < MAX_WIFI_INIT_RETRY)) {
        retries++;
        delay(WIFI_RETRY_DELAY);
        DEB_DO_PRINT("#");
    }
    if (WiFi.status() == WL_CONNECTED) {
        DEB_DO_PRINT("Connected to ");
        DEB_DO_PRINT(config.ssid);
        DEB_DO_PRINT("--- IP: ");
        status.ip = WiFi.localIP();
        DEB_DO_PRINTLN(status.ip);
        status.status = 'C';
    }
    else {
        status.lastWifiTime = millis();

        DEB_DO_PRINT("Creating AP --- IP: ");
        WiFi.mode(WIFI_AP);
        WiFi.softAP(config.ap_ssid, config.ap_passwd);
        status.ip = WiFi.softAPIP();
        status.status = 'A';
        DEB_DO_PRINTLN(status.ip);
    }

    if (String(config.mq_server) != "") {
      mqttClient.setServer(config.mq_server, 1883);
      mqttClient.setCallback(callback);
      status.lastMqttTime = millis() + WIFI_REINTENT_AFTER_SETUP;
    }
}

void reconnect_wifi(){
  if(millis() - status.lastWifiTime >= WIFI_REINTENT_AFTER_SETUP || millis() - status.lastWifiTime < 0) {
    initWifi();
  }
}

void reconnect() {
  if (String(config.mq_server) == "") {
    return;
  }

  if(!(millis() - status.lastMqttTime >= MQTT_REINTENT_AFTER_SETUP || millis() - status.lastMqttTime < 0)) {
    return;
  }

  status.lastMqttTime = millis();
  DEB_DO_PRINT("Attempting MQTT connection...");
  // Attempt to connect
  if (mqttClient.connect(config.mq_client, config.mq_user, config.mq_pass)) {
    DEB_DO_PRINTLN("connected");
    // ... and resubscribe
    // mqttClient.subscribe("/xxx/xxx/xxx");
  }
}
