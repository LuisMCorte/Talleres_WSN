#include <Wire.h>

// Umbral para decidir encender o apagar el LED
const int UMBRAL = 5;

void setup() {
  // Iniciar comunicación I2C con pines específicos del ESP32
  Wire.begin(21, 22);     // SDA en GPIO 21, SCL en GPIO 22
  
  Serial.begin(115200);   // Iniciar monitor serial
  Serial.println("Maestro PLC 14: Procesando datos del Controllino");
}

void loop() {
  // Solicitar 2 bytes al esclavo (Controllino) con dirección 9
  Wire.requestFrom(9, 2);
 
  // Verificar si se recibieron los 2 bytes esperados
  if (Wire.available() >= 2) {
    byte a = Wire.read();   // Leer byte alto
    byte b = Wire.read();   // Leer byte bajo
    
    // Reconstruir el valor original del potenciómetro
    int valorCrudo = (a << 8) | b;
    
    // Escalar el valor de 0-255 a un rango de 1-10 (simplificado)
    int valorEscalado = map(valorCrudo, 0, 255, 1, 10);
    
    // Mostrar información en el monitor serial
    Serial.print("Recibido Crudo: "); 
    Serial.print(valorCrudo);
    Serial.print(" | Escala (1-10): "); 
    Serial.println(valorEscalado);
    
    // Enviar orden al esclavo
    Wire.beginTransmission(9);
    if (valorEscalado < UMBRAL) {
      Wire.write(1);        // Orden: Encender LED
    } else {
      Wire.write(0);        // Orden: Apagar LED
    }
    Wire.endTransmission();
  }
  
  delay(500);   // Esperar 500ms antes de la siguiente lectura
}