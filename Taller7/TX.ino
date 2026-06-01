/**
 * @file TX_LoRa_HeltecV3.ino
 * @brief Transmisor LoRa para Heltec WiFi LoRa 32 (V3)
 * 
 * Proyecto: Taller  7- Redes Inalámbricas de Sensores (WSN)
 * Universidad de Cuenca
 * 
 * Envía periódicamente paquetes con número de secuencia y temperatura simulada.
 * Compatible con el receptor RX_LoRa_HeltecV3.
 * 
 * @author Luis Miguel Corte
 * @date Junio 2026
 * @version 1.0
 */

#include <Arduino.h>
#include <Wire.h>
#include <RadioLib.h>
#include "SSD1306Wire.h"

// ====================== PARÁMETROS LORA ======================
#define RF_FREQUENCY          915.9
#define LORA_BANDWIDTH        125.0
#define LORA_SPREADING_FACTOR 7       //7 9 12
#define LORA_CODINGRATE       5
#define LORA_PREAMBLE_LENGTH  8
#define TX_OUTPUT_POWER       10      // dBm (2 a 22)
#define INTERVALO_TX_MS       2000    // Intervalo entre transmisiones

// ====================== PINES HELTEC WIFI LORA 32 V3 ======================
#define SDA_OLED   17
#define SCL_OLED   18
#define RST_OLED   21
#define VEXT_PIN   36

#define LORA_CS    8
#define LORA_DIO1  14
#define LORA_RST   12
#define LORA_BUSY  13

// ====================== OBJETOS ======================
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);
SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);

int seqNum = 0;   // Contador de paquetes

// ====================== FUNCIONES OLED ======================
void oledInit() {
  pinMode(VEXT_PIN, OUTPUT);
  digitalWrite(VEXT_PIN, LOW);
  delay(100);

  pinMode(RST_OLED, OUTPUT);
  digitalWrite(RST_OLED, LOW);
  delay(50);
  digitalWrite(RST_OLED, HIGH);
  delay(50);

  Wire.begin(SDA_OLED, SCL_OLED);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.display();
}

void oledShow(String l0, String l1 = "", String l2 = "", String l3 = "") {
  display.clear();
  display.drawString(0,  0, l0);
  display.drawString(0, 16, l1);
  display.drawString(0, 32, l2);
  display.drawString(0, 48, l3);
  display.display();
}

// ====================== SETUP ======================
void setup() {
  Serial.begin(115200);
  delay(1500);

  oledInit();
  oledShow("LoRa TX - UCuenca", "Taller 10 WSN", "Iniciando...");

  int state = radio.begin(RF_FREQUENCY);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.println("ERROR radio: " + String(state));
    oledShow("ERROR RADIO", String(state));
    while (true);
  }

  radio.setBandwidth(LORA_BANDWIDTH);
  radio.setSpreadingFactor(LORA_SPREADING_FACTOR);
  radio.setCodingRate(LORA_CODINGRATE);
  radio.setPreambleLength(LORA_PREAMBLE_LENGTH);
  radio.setOutputPower(TX_OUTPUT_POWER);

  Serial.println("LoRa TX Configurado correctamente");
  Serial.println("SEQ,TEMP_C");   // Cabecera CSV

  oledShow("TX CONFIG OK",
           "SF" + String(LORA_SPREADING_FACTOR),
           String(RF_FREQUENCY,1) + " MHz",
           String(TX_OUTPUT_POWER) + " dBm");
  
  delay(2000);
}

// ====================== LOOP ======================
void loop() {
  seqNum++;

  // Temperatura simulada (reemplazar con sensor real cuando corresponda)
  float temp = random(200, 350) / 10.0;

  // Crear payload
  String payload = String(seqNum) + "," + String(temp, 1);

  // Transmitir
  int state = radio.transmit(payload.c_str());
  String status = (state == RADIOLIB_ERR_NONE) ? "OK" : "ERR:" + String(state);

  // Log por Serial
  Serial.println(String(seqNum) + "," + String(temp, 1));

  // Mostrar en OLED
  oledShow(
    "=== TX #" + String(seqNum) + " ===",
    "Temp: " + String(temp, 1) + " C",
    "SF" + String(LORA_SPREADING_FACTOR) + "  " + String(TX_OUTPUT_POWER) + " dBm",
    status
  );

  delay(INTERVALO_TX_MS);
}