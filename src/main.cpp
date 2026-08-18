#include <math.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Preferences.h> // Non-volatile storage (replaces EEPROM)

// Fixed UUIDs
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// GPIO Pin Definitions
const uint8_t pin_pwm_output = 32;
const uint8_t pin_increase = 34;
const uint8_t pin_decrease = 13;
const uint8_t pin_power = 26;

uint32_t button_debounce_timer = 0;
uint32_t increase_button_timer = 0;
uint32_t decrease_button_timer = 0;
uint32_t power_button_timer = 0;

// State Variables
uint8_t power_level = 0;           // 0-100%
uint8_t power_state = 1;           // 1=on, 0=off
char operation_mode = 'L';         // L=Linear, M=Maximum
String ble_device_name;
uint16_t stored_password;
const uint16_t MASTER_PASSWORD = 1234;

bool device_connected = false;
bool authentication_ok = false;
uint32_t notification_timer = 0;
uint32_t save_timer = 0;
bool pending_save = false;

BLECharacteristic *pTxChar;
Preferences prefs;

// PWM Calculation Function
// Original power calculation formula (preserved 100%)
/*uint32_t calculatePWM(uint8_t value) {
  if (power_state == 0 || value == 0) return 4095; 

  if (operation_mode == 'M') {
    if (value <= 50) return 4095 - round((0.00018 * pow(value, 3) + 79.9));
    return 4095 - round((0.000000022 * pow(value, 5) + 93.9));
  } else {
    if (value <= 50) return 4095 - round((0.00018 * pow(value, 3) + 63));
    return 4095 - 100 - round((0.000000022 * pow(value, 5) + 75));
  }
}*/

uint32_t calculatePWM(uint8_t value) 
{
  if (power_state == 0 || value == 0) return 4095;
  {
      if (value <= 25) {return map(value, 1, 25, 4040, 4020);}
      if (value <= 50) {return map(value, 26, 50, 4020, 4000);}
      if (value <= 98) {return map(value, 26, 50, 4000, 3950);}
      else {return 0;}
  }
}

// BLE Communication Callbacks
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) {
    String rx = String(pChar->getValue().c_str());
    rx.trim();
    if (rx.length() == 0) return;

    // Authentication command: p<password>
    if (rx.startsWith("p")) {
      uint16_t pass = (uint16_t)rx.substring(1).toInt();
      if (pass == stored_password || (pass == MASTER_PASSWORD && stored_password == 0xffff)) {
        authentication_ok = true;
        Serial.println("Authentication OK");
      } else {
        Serial.println("Authentication failed");
        ESP.restart(); // Security: restart on wrong password
      }
      return;
    }

    if (!authentication_ok) return;

    // Power level command: c<0-100>
    if (rx.startsWith("c")) {
      power_level = (uint8_t)rx.substring(1).toInt();
      ledcWrite(pin_pwm_output, calculatePWM(power_level));
      pending_save = true; save_timer = millis(); 
    } 
    // Power state command: s<0|1>
    else if (rx.startsWith("s")) {
      power_state = (rx.charAt(1) == '1') ? 1 : 0;
      ledcWrite(pin_pwm_output, calculatePWM(power_level));
      pending_save = true; save_timer = millis();
    }
    // Operation mode command: t<L|M>
    else if (rx.startsWith("t")) {
      operation_mode = rx.charAt(1);
      ledcWrite(pin_pwm_output, calculatePWM(power_level));
      pending_save = true; save_timer = millis();
    }
    // Save new password command: w<password>
    else if (rx.startsWith("w")) {
       stored_password = (uint16_t)rx.substring(1).toInt();
       prefs.putUInt("password", stored_password);
    }
    // Set device name and restart command: n<name>
    else if (rx.startsWith("n")) {
       prefs.putString("name", rx.substring(1));
       delay(500); ESP.restart();
    }
  }
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pS) { 
    device_connected = true; 
  }
  void onDisconnect(BLEServer* pS) { 
    device_connected = false; 
    authentication_ok = false; 
    BLEDevice::startAdvertising(); 
  }
};

void loadConfiguration() {
  prefs.begin("dimmer", false);
  
  power_level = prefs.getUChar("level", 0);
  power_state = prefs.getUChar("state", 1);
  operation_mode = (char)prefs.getUChar("mode", 'L');
  stored_password = prefs.getUInt("password", 1234);
  ble_device_name = prefs.getString("name", "MyDimmer");
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Allow time for power stabilization
  Serial.println("--- Starting Hardware Initialization ---");

  loadConfiguration();

  ledcWrite(pin_pwm_output, calculatePWM(power_level));

  // Configure GPIO pins
  // Note: GPIO 34 requires external pull-up resistor
  pinMode(pin_increase, INPUT);
  pinMode(pin_decrease, INPUT_PULLUP);
  pinMode(pin_power, INPUT_PULLUP);

  delay(100); // Allow internal pull-ups to stabilize

  // Read initial pin states
  int v_increase = digitalRead(pin_increase);
  int v_decrease = digitalRead(pin_decrease);
  int v_power = digitalRead(pin_power);

  Serial.printf("Initial Status -> Increase: %d | Decrease: %d | Power: %d\n", v_increase, v_decrease, v_power);

  // Check for buttons pressed at startup
  if (v_increase == LOW || v_decrease == LOW || v_power == LOW) {
    Serial.println("WARNING: Button detected as PRESSED at startup!");
    ESP.restart();
  }

  // Initialize BLE
  BLEDevice::init(ble_device_name.c_str());
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  BLECharacteristic *pRx = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pRx->setCallbacks(new MyCallbacks());

  pTxChar = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  pTxChar->addDescriptor(new BLE2902());

  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("Ready - Hardware Active");
}

void loop() {
  // Save timer: Protect flash memory from excessive writes
  if (pending_save && (millis() - save_timer > 5000)) {
    prefs.putUChar("level", power_level);
    prefs.putUChar("state", power_state);
    prefs.putUChar("mode", (uint8_t)operation_mode);
    pending_save = false;
    Serial.println("Preferences saved!");
  }

  // Button polling with debouncing (~3ms interval)
  if ((millis() - button_debounce_timer) >= 3) 
  {
    button_debounce_timer = millis();

    // INCREASE BUTTON (GPIO 34 - REQUIRES 10K EXTERNAL PULL-UP)
    if (digitalRead(pin_increase) == LOW) 
    {
        increase_button_timer++;
    }
    else 
    {
        if (increase_button_timer >= 15) 
        { // Debounce ~45ms
          if (power_level <= 90) 
          {
              power_level = (power_level - (power_level % 10)) + 10;
          } else {
              power_level = 100;
          }
          ledcWrite(pin_pwm_output, calculatePWM(power_level));
          pending_save = true; save_timer = millis();
        }
        increase_button_timer = 0;
    }

    // DECREASE BUTTON
    if (digitalRead(pin_decrease) == LOW) 
    {
        decrease_button_timer++;
    } 
    else 
    {
        if (decrease_button_timer >= 15) 
        { // Debounce ~45ms
          if (power_level >= 10) 
          {
              power_level = (power_level - (power_level % 10)) - 10;
          }
          else 
          {
              power_level = 0;
          }
          ledcWrite(pin_pwm_output, calculatePWM(power_level));
          pending_save = true; save_timer = millis();
        }
        decrease_button_timer = 0;
    }

    // POWER BUTTON
    if (digitalRead(pin_power) == LOW) 
    {
        power_button_timer++;
    }
    else 
    {
      // Short press: Toggle on/off
      if (power_button_timer >= 15 && power_button_timer <= 1000) {
          power_state = !power_state;
          ledcWrite(pin_pwm_output, calculatePWM(power_level));
          pending_save = true; save_timer = millis();
      } 
      // Long press (3 seconds): Factory reset
      else if (power_button_timer >= 1000 && !device_connected) {
          prefs.clear(); // Erase all "dimmer" namespace data
          Serial.println("Factory Reset!");
          delay(500); 
          ESP.restart();
      }
      power_button_timer = 0;
    }
  }

  // Synchronization Notifications to App
  if (device_connected && (millis() - notification_timer > 1000)) {
    notification_timer = millis();
    
    pTxChar->setValue(String(power_level).c_str());
    pTxChar->notify();
    
    delay(20);
    pTxChar->setValue(("t" + String(operation_mode)).c_str());
    pTxChar->notify();

    delay(20);
    pTxChar->setValue(("s" + String(power_state)).c_str());
    pTxChar->notify();
  }
}
