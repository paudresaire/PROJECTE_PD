#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>

#define DATA_PIN 5         // Pin de conexión de los LEDs
#define NUM_LEDS 9         // Número total de LEDs en la cadena

// SSID & Password
const char* ssid = "MOVISTAR_3174";  // Enter your SSID here
const char* password = "222c28yWH7Z5g58diD2h";  //Enter your Password here

WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)

void realizarJugada(char jugada);
bool hayGanador();
void mostrarTablero();
int mirar_led(int fila, int columna);
void Tablero_terminal(char jugador);
bool tableroCompleto();
void animacionVictoria();
void reiniciarPartida();

void handle_root();
void handle_move();

char obtenerGanador();
bool esMovimientoValido(int fila, int columna);
void realizarMovimiento(int fila, int columna);


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
	
	
	Serial.begin(115200);  // Inicializar la comunicación serial

// Mostrar el tablero vacío al inicio
	mostrarTablero();

	// Connect to your wi-fi modem
	WiFi.begin(ssid, password);

	// Check wi-fi is connected to wi-fi network
	while (WiFi.status() != WL_CONNECTED) {
	delay(1000);
	Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected successfully");
	Serial.print("Got IP: ");
	Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

	server.on("/", handle_root);
	server.on("/move", handle_move);

	server.begin();
	Serial.println("HTTP server started");
	delay(100); 

}


void loop() {
	
	server.handleClient();

	// Verificar si el juego ha terminado y se presionó el botón de escape
	if (juegoTerminado && Serial.available() && Serial.read() == 27) {
			reiniciarPartida();
			return;
	}
	
	if (Serial.available()) {
					char jugada = Serial.read();
					realizarJugada(jugada);
	}
}


void realizarJugada(char jugada) {
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
  for (int i = 0; i < 6; i++) {  // Repetir la animación 5 veces
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


/*String HTML = "<!DOCTYPE html>\
<html>\
<head>\
<style>\
table {\
  border-collapse: collapse;\
}\
td {\
  width: 50px;\
  height: 50px;\
  text-align: center;\
  border: 1px solid black;\
}\
</style>\
</head>\
<body>\
<h1>Tres en raya</h1>\
<table>\
<tr>\
<td onclick=\"playMove(0, 0)\">" + String(tablero[0][0]) + "</td>\
<td onclick=\"playMove(0, 1)\">" + String(tablero[0][1]) + "</td>\
<td onclick=\"playMove(0, 2)\">" + String(tablero[0][2]) + "</td>\
</tr>\
<tr>\
<td onclick=\"playMove(1, 0)\">" + String(tablero[1][0]) + "</td>\
<td onclick=\"playMove(1, 1)\">" + String(tablero[1][1]) + "</td>\
<td onclick=\"playMove(1, 2)\">" + String(tablero[1][2]) + "</td>\
</tr>\
<tr>\
<td onclick=\"playMove(2, 0)\">" + String(tablero[2][0]) + "</td>\
<td onclick=\"playMove(2, 1)\">" + String(tablero[2][1]) + "</td>\
<td onclick=\"playMove(2, 2)\">" + String(tablero[2][2]) + "</td>\
</tr>\
</table>\
<script>\
function playMove(row, col) {\
  var xhttp = new XMLHttpRequest();\
  xhttp.open('GET', '/move?row=' + row + '&col=' + col, true);\
  xhttp.send();\
}\
</script>\
</body>\
</html>";*/

String HTML = "<!DOCTYPE html>\
<html>\
<head>\
<style>\
table {\
  border-collapse: collapse;\
}\
td {\
  width: 50px;\
  height: 50px;\
  text-align: center;\
  border: 1px solid black;\
}\
</style>\
</head>\
<body>\
<h1>Tres en raya</h1>\
<table>\
<tr>\
<td onclick=\"playMove(0, 0)\">" + String(tablero[0][0]) + "</td>\
<td onclick=\"playMove(0, 1)\">" + String(tablero[0][1]) + "</td>\
<td onclick=\"playMove(0, 2)\">" + String(tablero[0][2]) + "</td>\
</tr>\
<tr>\
<td onclick=\"playMove(1, 0)\">" + String(tablero[1][0]) + "</td>\
<td onclick=\"playMove(1, 1)\">" + String(tablero[1][1]) + "</td>\
<td onclick=\"playMove(1, 2)\">" + String(tablero[1][2]) + "</td>\
</tr>\
<tr>\
<td onclick=\"playMove(2, 0)\">" + String(tablero[2][0]) + "</td>\
<td onclick=\"playMove(2, 1)\">" + String(tablero[2][1]) + "</td>\
<td onclick=\"playMove(2, 2)\">" + String(tablero[2][2]) + "</td>\
</tr>\
</table>\
<script>\
function playMove(row, col) {\
  var xhttp = new XMLHttpRequest();\
  xhttp.open('GET', '/move?row=' + row + '&col=' + col, true);\
  xhttp.send();\
}\
</script>\
</body>\
</html>";

/*
String generarTablaHTML() {
  String tablaHTML = "<table>";
  for (int fila = 0; fila < 3; fila++) {
    tablaHTML += "<tr>";
    for (int columna = 0; columna < 3; columna++) {
      tablaHTML += "<td>";
      tablaHTML += tablero[fila][columna];
      tablaHTML += "</td>";
    }
    tablaHTML += "</tr>";
  }
  tablaHTML += "</table>";
  return tablaHTML;
}


void handle_move() {
  // Leer el número de fila y columna enviados por la solicitud HTTP
  int fila = server.arg("fila").toInt();
  int columna = server.arg("columna").toInt();

  // Verificar si el movimiento es válido y actualizar el tablero si es así
  if (esMovimientoValido(fila, columna)) {
    realizarMovimiento(fila, columna);

    // Construir la página HTML con el tablero actualizado
    String html = HTML;  // Utilizar la cadena HTML proporcionada
    // Reemplazar los valores del tablero en la cadena HTML
    html.replace("{0}", String(tablero[0][0]));
    html.replace("{1}", String(tablero[0][1]));
    html.replace("{2}", String(tablero[0][2]));
    html.replace("{3}", String(tablero[1][0]));
    html.replace("{4}", String(tablero[1][1]));
    html.replace("{5}", String(tablero[1][2]));
    html.replace("{6}", String(tablero[2][0]));
    html.replace("{7}", String(tablero[2][1]));
    html.replace("{8}", String(tablero[2][2]));

    // Enviar la página HTML como respuesta HTTP
    server.send(200, "text/html", html);
  } else {
    // Enviar una respuesta HTTP de error
    server.send(400, "text/plain", "Movimiento invalido");
  }
}

void handle_root() {
  // Construir la página HTML con el tablero inicial
  String html = HTML;  // Utilizar la cadena HTML proporcionada
  // Reemplazar los valores del tablero en la cadena HTML
  html.replace("{0}", String(tablero[0][0]));
  html.replace("{1}", String(tablero[0][1]));
  html.replace("{2}", String(tablero[0][2]));
  html.replace("{3}", String(tablero[1][0]));
  html.replace("{4}", String(tablero[1][1]));
  html.replace("{5}", String(tablero[1][2]));
  html.replace("{6}", String(tablero[2][0]));
  html.replace("{7}", String(tablero[2][1]));
  html.replace("{8}", String(tablero[2][2]));

  // Enviar la página HTML como respuesta HTTP
  server.send(200, "text/html", html);
}

*/

String generarTablaHTML() {
  String tablaHTML = "<table>";
  for (int fila = 0; fila < 3; fila++) {
    tablaHTML += "<tr>";
    for (int columna = 0; columna < 3; columna++) {
      tablaHTML += "<td onclick=\"playMove(" + String(fila) + ", " + String(columna) + ")\">";
      tablaHTML += String(tablero[fila][columna]);
      tablaHTML += "</td>";
    }
    tablaHTML += "</tr>";
  }
  tablaHTML += "</table>";
  return tablaHTML;
}


void handle_move() {
  // Leer el número de fila y columna enviados por la solicitud HTTP
  int fila = server.arg("fila").toInt();
  int columna = server.arg("columna").toInt();

  // Verificar si el movimiento es válido y actualizar el tablero si es así
  if (esMovimientoValido(fila, columna)) {
    realizarMovimiento(fila, columna);

    // Verificar si hay un ganador
    char ganador = obtenerGanador();
    if (ganador != ' ') {
      String mensaje = "GANADOR: Jugador " + String(ganador);
      server.send(200, "text/plain", mensaje);
    } else {
      // Construir la página HTML con el tablero actualizado
      String tablaHTML = generarTablaHTML();
      String html = HTML;  // Utilizar la cadena HTML proporcionada
      // Reemplazar la etiqueta <table> en la cadena HTML
      html.replace("<table>", tablaHTML);

      // Enviar la página HTML como respuesta HTTP
      server.send(200, "text/html", html);
    }
  } else {
    // Enviar una respuesta HTTP de error
    server.send(400, "text/plain", "Movimiento inválido");
  }
}

void handle_root() {
  // Construir la página HTML con el tablero inicial
  String tablaHTML = generarTablaHTML();
  String html = HTML;  // Utilizar la cadena HTML proporcionada
  // Reemplazar la etiqueta <table> en la cadena HTML
  html.replace("<table>", tablaHTML);

  // Enviar la página HTML como respuesta HTTP
  server.send(200, "text/html", html);
}

char obtenerGanador() {
  // Verificar filas
  for (int fila = 0; fila < 3; fila++) {
    if (tablero[fila][0] == tablero[fila][1] && tablero[fila][0] == tablero[fila][2] && tablero[fila][0] != ' ') {
      return tablero[fila][0];
    }
  }

  // Verificar columnas
  for (int columna = 0; columna < 3; columna++) {
    if (tablero[0][columna] == tablero[1][columna] && tablero[0][columna] == tablero[2][columna] && tablero[0][columna] != ' ') {
      return tablero[0][columna];
    }
  }

  // Verificar diagonales
  if (tablero[0][0] == tablero[1][1] && tablero[0][0] == tablero[2][2] && tablero[0][0] != ' ') {
    return tablero[0][0];
  }
  if (tablero[0][2] == tablero[1][1] && tablero[0][2] == tablero[2][0] && tablero[0][2] != ' ') {
    return tablero[0][2];
  }

  return ' ';  // No hay ganador
}


bool esMovimientoValido(int fila, int columna) {
  // Verificar si la posición está dentro de los límites del tablero
  if (fila >= 0 && fila < 3 && columna >= 0 && columna < 3) {
    // Verificar si la posición está vacía (no tiene ninguna ficha)
    if (tablero[fila][columna] == ' ') {
      return true;  // El movimiento es válido
    }
  }
  return false;  // El movimiento es inválido
}

void realizarMovimiento(int fila, int columna) {
  // Obtener el símbolo del jugador actual (X o O)
  char simbolo = jugadorActual == 'X' ? 'X' : 'O';

  // Actualizar el tablero con el símbolo del jugador actual en la posición dada
  tablero[fila][columna] = simbolo;

  // Cambiar al siguiente jugador
  jugadorActual = (jugadorActual == 'X') ? 'O' : 'X';
}



