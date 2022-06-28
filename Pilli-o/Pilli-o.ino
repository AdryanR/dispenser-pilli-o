// ---- INCLUSÃO DE BIBLIOTECAS ---- //
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <elapsedMillis.h>

// ---- DEFINIÇÕES DE PINOS ---- //
//pino do botão para nova conexão WIFI...
int PIN_AP = 555;
int analogPin = A0;
int pinSensorComp = 5;
int pinSensorDetecte = 12;
int Buzzer = 13;
int pinServo = 14;

// ---- DEFINIÇÕES DE VARIAVEIS  ----
int sGirarAnti = 65; // esquerda
int sGirarHora = 110; // direita
int ServoParado = 90;
int switchstate = 0;
int comp = 0;
String MAC;
boolean ativarGirarAnti = false;
boolean pararBuzzer;
boolean ativarBuzzer;
int controle = 0; // controla a chamada das funcoes
//int compLiberar;
String resposta;
int compLiberar;
int idDisparo;
int cont = 0;
int frequencia = 0;
boolean alarmeAtivo = false;
// MQTT Broker //
const char *mqtt_broker = "cb12f099.eu-central-1.emqx.cloud";
const char *topic = "30";
const char *mqtt_username = "admin";
const char *mqtt_password = "h8xkjEDU$oC%";
const int mqtt_port = 15629;

//flag para indicar se foi salva uma nova configuração de rede
bool shouldSaveConfig = false;

// ---- INSTANCIANDO OBJETOS ----
Servo servo;
WiFiClient espClient;
PubSubClient client(espClient);
elapsedMillis tempo;
elapsedMillis tempo2;
elapsedMillis tempo3;
elapsedMillis tempob1;
elapsedMillis tempob2;

// ---- DECLARAÇÃO DE FUNÇÕES ----
void ConfigWifiEsp();
// callback do MQTT
void EscutaMQTT(char *topic, byte *payload, unsigned int length);
void ObterComp();
void TomouRemedio();
void DisparaAlarme();
void ResetaComps();
//void DisparaByTampa();

// ---- / ---- //

void setup() {
  Serial.begin(9600);
  pinMode(PIN_AP, INPUT); // seria um botao pra resetar a conexao
  pinMode(pinSensorComp, INPUT);
  pinMode(pinSensorDetecte, INPUT);
  servo.attach(pinServo);
  WiFiManager wifiManager;

  // ---- CONFIGURAÇÃO WIFI - ESP ---- //

  //callback para quando entra em modo de configuração AP
  wifiManager.setAPCallback(configModeCallback);
  //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.autoConnect("Pilli-o"); //cria uma rede sem senha
  wifiManager.setMinimumSignalQuality(10); // % minima para ele mostrar no SCAN
  //wifiManager.setConfigPortalTimeout(10); //timeout para o ESP nao ficar esperando para ser configurado para sempre

  // ---- FIM CONFIG WIFI ----

  // ---- CONFIGURAÇÃO MQTT ---- //

  MAC = String(WiFi.macAddress());
  const char* topico = MAC.c_str();

  String topicReconfig = "reconfig/" + MAC;
  const char* topicoReconfig = topicReconfig.c_str();

  String topicConfig = "dispenser/verificacao/" + MAC;
  const char* topicoConfig = topicConfig.c_str();

  if (WiFi.status() == WL_CONNECTED) {
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(EscutaMQTT);
    while (!client.connected()) {
      String client_id = "Pilli-o:";
      client_id += MAC;
      Serial.printf("Cliente %s conectado ao mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
        Serial.println("Public emqx mqtt broker connected");
      } else {
        Serial.print("failed with state ");
        Serial.print(client.state());
        delay(2000);
      }
    }
  }
  // adiciona o prótotipo no mesmo tópico MQTT que sua idenfiticação
  client.subscribe(topico);
  client.subscribe(topicoReconfig);
  client.subscribe(topicoConfig);

}


void loop() {

  // ---- CONFIGURAÇÃO WIFI - ESP ---- //
  ConfigWifiEsp();
  // ---- FIM CONFIG WIFI ----
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    // -- serve para enviar msg de configuração inicial -- //
    String Mac = MAC;
    Mac = "dispenser/" + Mac;
    const char* topico = Mac.c_str();

    //DisparaByTampa();

    if (controle == 1) {
      ObterComp();
    }
    if (controle == 2) {
      DisparaAlarme();
      TomouRemedio();
    }
    if (controle == 3) {
      ResetaComps();
    }
    if (controle == 4) {
      TomouRemedio();
    }
    // -- serve para enviar msg de configuração inicial -- //
    if (controle == 11) {
      String ssidString = String(WiFi.SSID());
      String msg = "{\"redeConectada\":\"true\",\"nomeRedeConectada\":\"" + ssidString + "\"}";
      const char* msgchar = msg.c_str();
      Serial.print(WiFi.SSID());
      client.publish(topico, msgchar );
      String topicConfig = "dispenser/verificacao/" + MAC;
      const char* topicoConfig = topicConfig.c_str();
      client.unsubscribe(topicoConfig);
      controle = 0;
    }
    // -- serve para resetar config wifi apos receber msg -- //
    if (controle == 15) {
      PIN_AP = 2;
      digitalWrite(PIN_AP, HIGH);
      ConfigWifiEsp();
    }
  }
}

// escutando MQTT - verifica se há disparo de algum alarme para essa máquina
void EscutaMQTT(char *topic, byte *payload, unsigned int length) {

  String msg;

  for (int i = 0; i < length; i++)
  {
    char c = (char)payload[i];
    msg += c;
  }

  Serial.println(msg);

  const char* MacChar = MAC.c_str();
  if (strcmp(topic, MacChar) == 0) {
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, msg);
    resposta = doc["resposta"].as<char*>();
    //Serial.println("Mensagem response: \n" + resposta);
    //String resp1 = "Sim";
    if (resposta.equals("Sim")) {
      compLiberar = doc["compartimento"];
      idDisparo = doc["idDisparo"];
      String resposta = "Não";
      controle = 1;
    }
  }
  String topicReconfig = "reconfig/" + MAC;
  const char* topicoReconfig = topicReconfig.c_str();
  if (strcmp(topic, topicoReconfig) == 0) {
    controle = 15;
  }

  String topicConfig = "dispenser/verificacao/" + MAC;
  const char* topicoConfig = topicConfig.c_str();
  if (strcmp(topic, topicoConfig) == 0) {
    controle = 11;
  }
}

// faz o compartimento rodar até o remédio
void ObterComp() {
  if (compLiberar != comp) {
    servo.write(sGirarHora);
  }
  //Serial.print("Entrou na funcao");
  if (digitalRead(pinSensorComp) == HIGH && switchstate == 0) {
    switchstate = 1;
    comp ++;
    Serial.print("Compartimento: ");
    Serial.println(comp);
  }
  if (digitalRead(pinSensorComp) == LOW && switchstate == 1) {
    switchstate = 0;
  }
  if (comp == compLiberar) {
    servo.write(ServoParado);
    //comp = 0;
    controle = 2;
    tempo = 0;
    tempob1 = 0;
    ativarBuzzer = true;
    return;
  }

}

void ResetaComps() {
  if (tempo2 == 15000) {
    ativarGirarAnti = true;
  }
  if (ativarGirarAnti == true) {
    if (comp != 0) {
      servo.write(sGirarHora);
    }
    if (digitalRead(pinSensorComp) == HIGH && switchstate == 0) {
      switchstate = 1;
      comp ++;
      Serial.print("Compartimento: ");
      Serial.println(comp);
    }
    if (digitalRead(pinSensorComp) == LOW && switchstate == 1) {
      switchstate = 0;
    }
    if (comp == 16) {
      servo.write(ServoParado);
      controle = 0;
      comp = 0;
      ativarGirarAnti = false;
      return;
    }
  }
}


// fica verificando se o usuário "tomou o remédio"
void TomouRemedio() {
  WiFiClientSecure client;
  HTTPClient  https;
  client.setInsecure();
  if (digitalRead(pinSensorDetecte) == LOW && switchstate == 1) {
    switchstate = 0;
    Serial.println("Abriu tampa!");
    ativarBuzzer = false;
    noTone(Buzzer);
  }
  if (digitalRead(pinSensorDetecte) == HIGH && switchstate == 0) {
    switchstate = 1;
    pararBuzzer = true;
    noTone(Buzzer);
    Serial.println("Fechou tampa!");
    https.begin(client, "https://api-pillio.herokuapp.com/api/disparos/alter/" + String(idDisparo));
    https.addHeader("Content-Type", "application/json");
    DynamicJsonDocument doc(2048);
    doc["tomouRemedio"] = "1";

    // Serialize JSON document
    String json;
    serializeJson(doc, json);
    //Serial.print(json);
    int httpsCode = https.PUT(json);
    if (httpsCode > 0) {
      Serial.print(https.getString());
      Serial.print("\n");
      https.end();
      tempo2 = 0;
      controle = 3;
      return; // sai da função
    }
  }
}


void DisparaAlarme() {
  pararBuzzer = false;
  if (tempo >= 60000 && pararBuzzer == false) {
    Serial.println("Entrou IFF buzzer");
    noTone(Buzzer); //Parar a geração de sinais no buzzer
    controle = 4;
    return;
  }
  if (ativarBuzzer) {
  while (tempob1 <= 150 && ativarBuzzer) {
      tone(Buzzer, 1350); //Gerar um sinal de 1350Hz no pino A5
      tempob2 = 0;
    }
    while (tempob2 <= 150 && ativarBuzzer) {
      tone(Buzzer, 1050); //Gerar um sinal de 1050Hz no pino A5
      tempob1 = 0;
    }
  }
}
//
//void DisparaByTampa() {
//  Serial.println("DisparaByTampa");
//  if (digitalRead(pinSensorDetecte) == LOW && switchstate == 0) {
//    alarmeAtivo = true;
//    switchstate = 1;
//  }
//  while (alarmeAtivo == true) {
//    for (frequencia = 150; frequencia < 1800; frequencia += 1)
//    {
//      tone(Pinofalante, frequencia, tempo);
//      delay(1);
//    }
//    for (frequencia = 1800; frequencia > 150; frequencia -= 1)
//    {
//      tone(Pinofalante, frequencia, tempo);
//      delay(1);
//    }
//    if (digitalRead(pinSensorDetecte) == HIGH && switchstate == 1) {
//      switchstate = 0;
//      noTone(Buzzer);
//      alarmeAtivo = false;
//    }
//  }
//}

// caso a conexão MQTT caia - essa funcao reconecta
void reconnect() {
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(EscutaMQTT);
  while (!client.connected()) {
    String client_id = "Pilli-o:";
    client_id += MAC;
    Serial.printf("Cliente %s conectado ao mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  // adiciona o prótotipo no mesmo tópico MQTT que sua idenfiticação
  client.subscribe(topic);
}


void ConfigWifiEsp() {
  WiFiManager wifiManager;
  //se o botão foi pressionado
  if ( digitalRead(PIN_AP) == HIGH ) {
    Serial.println("resetar"); //tenta abrir o portal
    if (!wifiManager.startConfigPortal("Pilli-o") ) {
      Serial.println("Falha ao conectar");
      delay(2000);
      ESP.restart();
      delay(1000);
    }
    Serial.println("Conectou ESP_AP!!!");
    if (controle = 15) {
      PIN_AP = 555;
      controle = 0;
    }
  }
}

//WIFI ESP - callback que indica que o ESP entrou no modo AP
void configModeCallback (WiFiManager * myWiFiManager) {
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); //imprime o SSID criado da rede

}

//WIFI ESP - callback que indica que salvamos uma nova rede para se conectar (modo estação)
void saveConfigCallback () {
  Serial.println("Configuração salva");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP

  //  WiFiClientSecure clients;
  //  HTTPClient  https;
  //  clients.setInsecure();
  //  String Mac = String(WiFi.macAddress());
  //  https.begin(clients, "https://api-pillio.herokuapp.com/api/machines/verificaconfig-maq-user/" + Mac);
  //  int httpCode = https.GET();
  //  if (httpCode > 0) {
  //    String result = https.getString();
  //    Serial.println(result);
  //    if (result.equals("true")) {
  //      https.end();
  //      controle = 11;
  //      return;
  //    } else {
  //      https.end();
  //      controle = 12;
  //      return;
  //    }
  //  }
}
