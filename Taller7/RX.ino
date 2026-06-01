/**
 * @file RX_LoRa_HeltecV3.ino
 * @brief Receptor LoRa para Heltec WiFi LoRa 32 (V3)
 * 
 * Proyecto: Taller 7 - Redes Inalámbricas de Sensores (WSN)
 * Universidad de Cuenca
 * 
 * Recibe paquetes LoRa enviados por el transmisor, extrae número de secuencia
 * y temperatura, muestra información en OLED y envía datos por Serial en formato CSV.
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
#define RF_FREQUENCY          915.0    // Frecuencia en MHz (coordinar con TX)
#define LORA_BANDWIDTH        125.0    // kHz
#define LORA_SPREADING_FACTOR 7        // SF7, SF9 o SF12
#define LORA_CODINGRATE       5        // 5 = 4/5
#define LORA_PREAMBLE_LENGTH  8

// ====================== PINES HELTEC WIFI LORA 32 V3 ======================
#define SDA_OLED   17
#define SCL_OLED   18
#define RST_OLED   21
#define VEXT_PIN   36     // Alimentación de la pantalla OLED

#define LORA_CS    8
#define LORA_DIO1  14
#define LORA_RST   12
#define LORA_BUSY  13

// ====================== OBJETOS ======================
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);
SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);

// ====================== VARIABLES GLOBALES ======================
volatile bool paqueteListo = false;   // Flag de interrupción

float rssiUltimo = 0;
float snrUltimo  = 0;
float tempUltima = 0;
int   seqUltimo  = 0;
int   pktRecibidos = 0;

// ====================== CALLBACK DE INTERRUPCIÓN ======================
/**
 * @brief Callback ejecutado cuando llega un paquete LoRa
 */
void IRAM_ATTR onReceive() {
  paqueteListo = true;
}

// ====================== FUNCIONES OLED ======================
void oledInit() {
  pinMode(VEXT_PIN, OUTPUT);
  digitalWrite(VEXT_PIN, LOW);   // Encender OLED
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
  oledShow("LoRa RX - UCuenca", "Taller 10 WSN", "Iniciando...");

  int state = radio.begin(RF_FREQUENCY);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.println("ERROR radio: " + String(state));
    oledShow("ERROR RADIO", String(state));
    while (true);
  }

  // Configuración LoRa
  radio.setBandwidth(LORA_BANDWIDTH);
  radio.setSpreadingFactor(LORA_SPREADING_FACTOR);
  radio.setCodingRate(LORA_CODINGRATE);
  radio.setPreambleLength(LORA_PREAMBLE_LENGTH);

  // Modo receptor continuo con interrupción
  radio.setPacketReceivedAction(onReceive);
  radio.startReceive();

  Serial.println("SEQ,RSSI,SNR,TEMP");  // Cabecera CSV

  oledShow("RX CONFIG OK", 
           "SF" + String(LORA_SPREADING_FACTOR),
           "Esperando paquetes...");
}

// ====================== LOOP ======================
void loop() {
  if (!paqueteListo) return;

  paqueteListo = false;

  String payload = "";
  int state = radio.readData(payload);
  
  radio.startReceive();  // Volver a escuchar inmediatamente

  if (state != RADIOLIB_ERR_NONE) {
    Serial.println("RX ERROR: " + String(state));
    return;
  }

  rssiUltimo = radio.getRSSI();
  snrUltimo  = radio.getSNR();

  // Parsear payload "SEQ,TEMP"
  int idx = payload.indexOf(',');
  if (idx > 0) {
    seqUltimo  = payload.substring(0, idx).toInt();
    tempUltima = payload.substring(idx + 1).toFloat();
  }

  pktRecibidos++;

  // Enviar datos por Serial (CSV)
  Serial.print(seqUltimo);   Serial.print(",");
  Serial.print(rssiUltimo);  Serial.print(",");
  Serial.print(snrUltimo);   Serial.print(",");
  Serial.println(tempUltima, 1);

  // Mostrar en OLED
  oledShow(
    "SEQ: "  + String(seqUltimo),
    "RSSI: " + String(rssiUltimo, 1) + " dBm",
    "SNR: "  + String(snrUltimo,  1) + " dB",
    "TEMP: " + String(tempUltima, 1) + " C"
  );
}