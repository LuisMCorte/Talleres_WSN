#include <Wire.h>
#include <Controllino.h>

int valorPot = 0;

void setup() {
  Wire.begin(9);                // Dirección esclavo 9
  Wire.onRequest(enviarPot);    // Cuando el PLC pida datos
  Wire.onReceive(recibirOrden); // Cuando el PLC envíe órdenes
  
  Serial.begin(9600);           // Monitor serial del Controllino
  pinMode(CONTROLLINO_A0, INPUT);
  pinMode(CONTROLLINO_D0, OUTPUT); 
  Serial.println("Controllino: Enviando datos en crudo y esperando ordenes...");
}

void loop() {
  // Leemos el valor del potenciómetro (0 a 1023)
  valorPot = analogRead(CONTROLLINO_A0);
  delay(50); 
}

// Función que se ejecuta cuando el PLC pide el dato
void enviarPot() {
  byte buffer[2];
  buffer[0] = valorPot >> 8;    // Parte alta del número
  buffer[1] = valorPot & 0xFF; // Parte baja del número
  
  Wire.write(buffer, 2);
  
  // Mostrar el dato enviado
  Serial.print("Dato en crudo enviado al PLC: ");
  Serial.println(valorPot);
}

void recibirOrden(int cuantosBytes) {
  while (Wire.available()) {
    int orden = Wire.read();
    if (orden == 1) {
      digitalWrite(CONTROLLINO_D0, HIGH);
      Serial.println("-> PLC ordena: ENCENDER LED");
    } else {
      digitalWrite(CONTROLLINO_D0, LOW);
      Serial.println("-> PLC ordena: APAGAR LED");
    }
  }
}