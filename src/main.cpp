#include <Arduino.h>
#include <Wire.h>

float readTemperature();

void setup() {
  Serial.begin(115200); // Inicializa la comunicación serial
}

void loop() {
  if (Serial.available()) {
    int opcion = Serial.parseInt(); // Lee el número ingresado por teclado
    
    switch (opcion) {
      case 1:
        tarea1();
        break;
        
      case 2:
        tarea2();
        break;
        
      case 3:
        tarea3();
        break;
        
      case 4:
        tarea4();
        break;
        
      case 5:
        float temperature = readTemperature();
        Serial.print("Temperatura: ");
        Serial.print(temperature);
        Serial.println(" °C");
        delay(1000);
        break;
        
      default:
        Serial.println("Opción inválida");
        break;
    }
  }
}

void tarea1() {
  Serial.println("Realizando tarea 1");
  // Coloca aquí el código para la tarea 1
}

void tarea2() {
  Serial.println("Realizando tarea 2");
  // Coloca aquí el código para la tarea 2
}

void tarea3() {
  Serial.println("Realizando tarea 3");
  // Coloca aquí el código para la tarea 3
}

void tarea4() {
  Serial.println("Realizando tarea 4");
  // Coloca aquí el código para la tarea 4
}

float readTemperature() {
  Wire.beginTransmission(0x68); // Dirección del sensor de temperatura interno del ESP32
  Wire.write(0x41); // Registro que contiene la temperatura
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 2);
  
  byte msb = Wire.read();
  byte lsb = Wire.read();
  
  float temp = ((msb << 8) | lsb) / 340.0;
  temp -= 273.15; // Conversión a Celsius
  
  return temp;
}


