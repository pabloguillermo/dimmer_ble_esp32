#include <math.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Preferences.h> // Upgrade para Preferences (substitui EEPROM)

// UUIDs Fixos
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

const uint8_t pin_led1 = 32;
const uint8_t pin_mais = 34;
const uint8_t pin_menos = 13;
const uint8_t pin_liga = 26;
uint32_t temporiza_botoes = 0;
uint32_t tmp_pinmais = 0;
uint32_t tmp_pinmenos = 0;
uint32_t tmp_pinliga = 0;
// Variáveis de Estado
uint8_t porcentagem = 0;
uint8_t estado_on = 1;
char modo_atual = 'L';
String nome_ble;
uint16_t senha_cadastrada;
const uint16_t SENHA_MESTRA = 1234;

bool deviceConnected = false;
bool auth_ok = false;
uint32_t timer_notificacao = 0;
uint32_t timer_save = 0;
bool pending_save = false;

BLECharacteristic *pTxChar;
Preferences prefs;

// --- A SUA LÓGICA DE POTÊNCIA (PRESERVADA 100%) ---
/*uint32_t calculaPWM(uint8_t valor) {
  if (estado_on == 0 || valor == 0) return 4095; 

  if (modo_atual == 'M') {
    if (valor <= 50) return 4095 - round((0.00018 * pow(valor, 3) + 79.9));
    return 4095 - round((0.000000022 * pow(valor, 5) + 93.9));
  } else {
    if (valor <= 50) return 4095 - round((0.00018 * pow(valor, 3) + 63));
    return 4095 -100 - round((0.000000022 * pow(valor, 5) + 75));
  }
}*/
uint32_t calculaPWM(uint8_t valor) 
{
  if (estado_on == 0 || valor == 0) return 4095;
  {
      if (valor <= 25) {return map(valor, 1,25,4040,4020);}
      if (valor <= 50) {return map(valor, 26,50,4020,4000);}
      if (valor <= 98) {return map(valor, 26,50,4000,3950);}
      else {return 0;}
  }
  /*else {
        if (valor < 1) valor = 1;
        if (valor > 100) valor = 100;
        return (uint32_t)(8145-((0.00444444 * (uint32_t)valor * (uint32_t)valor) + (0.00111111 * (uint32_t)valor) + 4049.99444444));
       }*/
}
// Callbacks de Comunicação do ESP32 BLE:
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) {
    String rx = String(pChar->getValue().c_str());
    rx.trim();
    if (rx.length() == 0) return;

    if (rx.startsWith("p")) {
      uint16_t pass = (uint16_t)rx.substring(1).toInt();
      if (pass == senha_cadastrada || (pass == SENHA_MESTRA && senha_cadastrada == 0xffff)) {
        auth_ok = true;
        Serial.println("Auth OK");
      } else {
        Serial.println("Auth falhou");
        ESP.restart(); // Senha errada reinicia por segurança
      }
      return;
    }

    if (!auth_ok) return;

    if (rx.startsWith("c")) {
      porcentagem = (uint8_t)rx.substring(1).toInt();
      ledcWrite(pin_led1, calculaPWM(porcentagem));
      pending_save = true; timer_save = millis(); 
    } 
    else if (rx.startsWith("s")) {
      estado_on = (rx.charAt(1) == '1') ? 1 : 0;
      ledcWrite(pin_led1, calculaPWM(porcentagem));
      pending_save = true; timer_save = millis();
    }
    else if (rx.startsWith("t")) {
      modo_atual = rx.charAt(1);
      ledcWrite(pin_led1, calculaPWM(porcentagem));
      pending_save = true; timer_save = millis();
    }
    else if (rx.startsWith("w")) { // Gravar nova senha
       senha_cadastrada = (uint16_t)rx.substring(1).toInt();
       prefs.putUInt("senha", senha_cadastrada);
    }
    else if (rx.startsWith("n")) { // Gravar novo nome e reiniciar
       prefs.putString("nome", rx.substring(1));
       delay(500); ESP.restart();
    }
  }
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pS) { deviceConnected = true; }
  void onDisconnect(BLEServer* pS) { 
    deviceConnected = false; 
    auth_ok = false; 
    BLEDevice::startAdvertising(); 
  }
};

void carregarConfig() {
  prefs.begin("dimmer", false);
  
  porcentagem = prefs.getUChar("pot", 0);
  estado_on = prefs.getUChar("est", 1);
  modo_atual = (char)prefs.getUChar("mod", 'L');
  senha_cadastrada = prefs.getUInt("senha", 1234);
  nome_ble = prefs.getString("nome", "MeuDimmer");
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Aumente para 1 segundo para estabilizar a fonte
  Serial.println("--- Iniciando Leitura de Hardware ---");

  carregarConfig();

  ledcAttachChannel(pin_led1, 1000, 12, 0); 
  ledcWrite(pin_led1, calculaPWM(porcentagem));

// Configuração dos pinos (Lembrando: 34 precisa de resistor externo!)
  pinMode(pin_mais, INPUT);    // GPIO 34 (INPUT puro)
  pinMode(pin_menos, INPUT_PULLUP);
  pinMode(pin_liga, INPUT_PULLUP);

  // Pequeno delay para os pull-ups internos "puxarem" a tensão para cima
  delay(100); 

  int v_mais = digitalRead(pin_mais);
  int v_menos = digitalRead(pin_menos);
  int v_liga = digitalRead(pin_liga);

  Serial.printf("Status Inicial -> Mais: %d | Menos: %d | Liga: %d\n", v_mais, v_menos, v_liga);

  // Se algum estiver em 0 (pressionado ou erro de pull-up), avisa mas não reinicia ainda
  if (v_mais == LOW || v_menos == LOW || v_liga == LOW) {
    Serial.println("CUIDADO: Botão detectado como PRESSIONADO no boot!");
    ESP.restart(); // Comente esta linha temporariamente para conseguir ver o log!
  }

  BLEDevice::init(nome_ble.c_str());
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  BLECharacteristic *pRx = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pRx->setCallbacks(new MyCallbacks());

  pTxChar = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  pTxChar->addDescriptor(new BLE2902());

  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("Pronto - Hardware Ativo");
}

void loop() {
  // Timer para salvar (Proteção da Memória Flash)
  if (pending_save && (millis() - timer_save > 5000)) {
    prefs.putUChar("pot", porcentagem);
    prefs.putUChar("est", estado_on);
    prefs.putUChar("mod", (uint8_t)modo_atual);
    pending_save = false;
    Serial.println("Preferências salvas!");
  }

if ((millis() - temporiza_botoes) >= 3) 
{
  temporiza_botoes = millis();

  // BOTÃO MAIS (GPIO 34 - PRECISA DE RESISTOR EXTERNO 10K)
  if (digitalRead(pin_mais) == LOW) 
  {
      tmp_pinmais++;
  }
  else 
  {
      if (tmp_pinmais >= 15) 
      { // Debounce de ~45ms
          if (porcentagem <= 90) 
          {
              porcentagem = (porcentagem - (porcentagem % 10)) + 10;
          } else {
              porcentagem = 100;
          }
          ledcWrite(pin_led1, calculaPWM(porcentagem)); // Atualiza o brilho na hora
          pending_save = true; timer_save = millis();
      }
      tmp_pinmais = 0;
  }

  // BOTÃO MENOS
  if (digitalRead(pin_menos) == LOW) 
  {
      tmp_pinmenos++;
  } 
  else 
  {
      if (tmp_pinmenos >= 15) 
      { // Corrigido para tmp_pinmenos
          if (porcentagem >= 10) 
          {
              porcentagem = (porcentagem - (porcentagem % 10)) - 10;
          }
          else 
          {
              porcentagem = 0;
          }
          ledcWrite(pin_led1, calculaPWM(porcentagem));
          pending_save = true; timer_save = millis();
      }
      tmp_pinmenos = 0;
  }

  // BOTÃO LIGA/RESET
  if (digitalRead(pin_liga) == LOW) 
  {
      tmp_pinliga++;
  }
  else 
  {
    // Toque rápido: Liga/Desliga
    if (tmp_pinliga >= 15 && tmp_pinliga <= 1000) {
        estado_on = !estado_on;
        ledcWrite(pin_led1, calculaPWM(porcentagem));
        pending_save = true; timer_save = millis();
    } 
    // Toque longo (3 segundos): Reset de Fábrica
    else if (tmp_pinliga >= 1000 && !deviceConnected) {
        prefs.clear(); // Apaga TUDO do namespace "dimmer"
        Serial.println("Reset de Fábrica!");
        delay(500); 
        ESP.restart();
    }
    tmp_pinliga = 0;
  }
}


  // Notificação de Sincronismo para o App
  if (deviceConnected && (millis() - timer_notificacao > 1000)) {
    timer_notificacao = millis();
    
    pTxChar->setValue(String(porcentagem).c_str());
    pTxChar->notify();
    
    delay(20);
    pTxChar->setValue(("t" + String(modo_atual)).c_str());
    pTxChar->notify();

    delay(20);
    pTxChar->setValue(("s" + String(estado_on)).c_str());
    pTxChar->notify();
  }
}