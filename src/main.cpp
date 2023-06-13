#include <Arduino.h>
#include <FastLED.h>

#define DATA_PIN 5         // Pin de conexión de los LEDs
#define NUM_LEDS 9         // Número total de LEDs en la cadena

bool hayGanador();
void mostrarTablero();
int mirar_led(int fila, int columna);
void Tablero_terminal(char jugador);
bool tableroCompleto();
void animacionVictoria();
void reiniciarPartida();

static int totalpartides = 0;


CRGB leds[NUM_LEDS];
const CRGB COLOR_VERDE = CRGB::Green;
const CRGB COLOR_AZUL = CRGB::Blue;
const CRGB COLOR_ROJO = CRGB::Red;
const CRGB COLOR_NARANJA = CRGB::Orange;
const CRGB COLOR_VIOLETA = CRGB::Violet;

// Matriz para el tablero
char tablero[3][3] = {
	{' ', ' ', ' '},
	{' ', ' ', ' '},
	{' ', ' ', ' '}
};

// Variables para el estado del juego
bool juegoTerminado = false;
bool empate = false;
char jugadorActual = 'X';

void setup() {
	// Inicialización de los LED Neopixel
	FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(200);  // Ajusta el brillo de los LED Neopixel
	
	
	// Mostrar el tablero vacío al inicio
	mostrarTablero();
	Serial.begin(115200);  // Inicializar la comunicación serial
}


void loop() {
	
	// Verificar si el juego ha terminado y se presionó el botón de escape
		if (juegoTerminado && Serial.available() && Serial.read() == 27) {
			reiniciarPartida();
			return;
		}

	// Esperar a que el jugador ingrese una jugada
	if (Serial.available()) {
		char jugada = Serial.read();
		int fila, columna;
		
		// Asignar la fila y columna correspondientes a la jugada
		if (jugada == 'Q' || jugada == 'q') {
    fila = 0;
    columna = 0;
		} else if (jugada == 'W' || jugada == 'w') {
				fila = 0;
				columna = 1;
		} else if (jugada == 'E' || jugada == 'e') {
				fila = 0;
				columna = 2;
		} else if (jugada == 'A' || jugada == 'a') {
				fila = 1;
				columna = 0;
		} else if (jugada == 'S' || jugada == 's') {
				fila = 1;
				columna = 1;
		} else if (jugada == 'D' || jugada == 'd') {
				fila = 1;
				columna = 2;
		} else if (jugada == 'Z' || jugada == 'z') {
				fila = 2;
				columna = 0;
		} else if (jugada == 'X' || jugada == 'x') {
				fila = 2;
				columna = 1;
		} else if (jugada == 'C' || jugada == 'c') {
				fila = 2;
				columna = 2;
		} else {
				Serial.println("Jugada inválida");
				return;
		}

		
		// Verificar si la casilla está vacía
		if (tablero[fila][columna] != ' ') {
			Serial.println("Casilla ocupada");
			return;
		}
		
		// Realizar la jugada
		tablero[fila][columna] = jugadorActual;
		mostrarTablero();

		// Cambiar el turno del jugador
		if (jugadorActual == 'X')
			Tablero_terminal('O');
		else if (jugadorActual == 'O')
			Tablero_terminal('X');

			Serial.println();
			Serial.print("Total de partides jugades = ");
			Serial.println(totalpartides);
			Serial.println();

		// Verificar si hay un ganador
		if (hayGanador()) {
			Serial.print("¡Jugador ");
			Serial.print(jugadorActual);
			Serial.println(" ha ganado!");

			animacionVictoria();
			juegoTerminado = true;
		}

		empate = tableroCompleto();
		jugadorActual = (jugadorActual == 'X') ? 'O' : 'X';
		if (empate){
			Serial.println("EMPATE!!");
			juegoTerminado = true;
			return;
		}
	}
}


//Funcio per a mostrar el tauler
void mostrarTablero() 
{
	int indice;

	indice = 0;
	for (int fila = 0; fila < 3; fila++) {
		for (int columna = 0; columna < 3; columna++) {
			if (tablero[fila][columna] == 'X') {
				indice = mirar_led(fila, columna);
				leds[indice] = CRGB::Red;    // Color rojo para el jugador 1
			} else if (tablero[fila][columna] == 'O') {
				indice = mirar_led(fila, columna);
				leds[indice] = CRGB::Blue;   // Color azul para el jugador 2
			} else {
				indice = mirar_led(fila, columna);
				leds[indice] = CRGB::Black;  // LED apagado para casillas vacías
			}
		}
	}
	FastLED.show();
}

// Función para verificar si hay un ganador
bool hayGanador() 
{
		// Verificar filas
		for (int fila = 0; fila < 3; fila++) {
			if (tablero[fila][0] != ' ' && tablero[fila][0] == tablero[fila][1] && tablero[fila][0] == tablero[fila][2]) {
				return true;
			}
		}
		// Verificar columnas
		for (int columna = 0; columna < 3; columna++) {
			if (tablero[0][columna] != ' ' && tablero[0][columna] == tablero[1][columna] && tablero[0][columna] == tablero[2][columna]) {
				return true;
			}
		}
		// Verificar diagonales
		if (tablero[0][0] != ' ' && tablero[0][0] == tablero[1][1] && tablero[0][0] == tablero[2][2]) {
			return true;
		}
		if (tablero[0][2] != ' ' && tablero[0][2] == tablero[1][1] && tablero[0][2] == tablero[2][0]) {
			return true;
		}
		return false;
}

int mirar_led(int fila, int columna)
{
	int indice;

	indice = 0;
	if (fila == 0 && columna == 0)
		indice = 0;
	else if (fila == 0 && columna == 1)
		indice = 1;
	else if (fila == 0 && columna == 2)
		indice = 2;
	else if (fila == 1 && columna == 2)
		indice = 3;
	else if (fila == 1 && columna == 1)
		indice = 4;
 	else if (fila == 1 && columna == 0)
		indice = 5;
	else if (fila == 2 && columna == 0)
		indice = 6;
	else if (fila == 2 && columna == 1)
		indice = 7;
	else if (fila == 2 && columna == 2)
		indice = 8;

	return (indice);
}

void Tablero_terminal(char jugador)
{
    for (int fila = 0; fila < 3; fila++) {
        for (int columna = 0; columna < 3; columna++) {
            Serial.print(tablero[fila][columna]);
            if (columna < 2) {
                Serial.print(" | ");
            }
        }
        Serial.println();
        if (fila < 2) {
            Serial.println("---------");
        }
    }
		Serial.println();
    Serial.print("Turno del jugador: ");
    Serial.println(jugador);
		Serial.println();
}

bool tableroCompleto() {
  for (int fila = 0; fila < 3; fila++) {
    for (int columna = 0; columna < 3; columna++) {
      if (tablero[fila][columna] == ' ') {
        return false;  // Hay al menos una casilla vacía, no está completo
      }
    }
  }
  return true;
}

void animacionVictoria() {
  for (int i = 0; i < 8; i++) {  // Repetir la animación 5 veces
    // Encender todas las luces en verde
    fill_solid(leds, NUM_LEDS, COLOR_VERDE);
    FastLED.show();
    delay(100); 
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(100);  // Mantener el estado apagado durante 500 ms

		fill_solid(leds, NUM_LEDS, COLOR_NARANJA);
    FastLED.show();
    delay(100); 
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(100);  // Mantener el estado apagado durante 500 ms

		fill_solid(leds, NUM_LEDS, COLOR_VIOLETA);
    FastLED.show();
    delay(100); 
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(100);  // Mantener el estado apagado durante 500 ms

		fill_solid(leds, NUM_LEDS, COLOR_ROJO);
    FastLED.show();
    delay(100); 
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(100);  // Mantener el estado apagado durante 500 ms

		fill_solid(leds, NUM_LEDS, COLOR_AZUL);
    FastLED.show();
    delay(100); 
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(100);  // Mantener el estado apagado durante 500 ms
  }
}

void reiniciarPartida() {
  // Reiniciar variables de estado del juego
  juegoTerminado = false;
  empate = false;
  jugadorActual = 'X';
	totalpartides++;

  // Limpiar el tablero
  for (int fila = 0; fila < 3; fila++) {
    for (int columna = 0; columna < 3; columna++) {
      tablero[fila][columna] = ' ';
    }
  }

  // Mostrar el tablero vacío
  mostrarTablero();
}

