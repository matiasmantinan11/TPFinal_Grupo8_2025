#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Configuración de la pantalla OLED 128x32
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// Definiciones de tiempos de Morse
const unsigned long DOT_DURACION = 300;  // Duración de un punto 
const unsigned long DASH_DURACION = 3 * DOT_DURACION; // Duración de una raya 
const unsigned long TOLERANCIA = 200;     // Tolerancia en ms para reconocer puntos/rayas/espacios

// Gaps (Tiempo que el botón está liberado)
const unsigned long SHORT_GAP = DOT_DURACION;         // Espacio entre Punto y Raya (1t)
const unsigned long MEDIUM_GAP = 3 * DOT_DURACION;    // Espacio entre caracteres (3t)
const unsigned long LONG_GAP = 7 * DOT_DURACION;      // Espacio entre palabras (7t)

// Pines de conexión
const int BOTON_PIN = 6; // Pin para el pulsador (conectado a GND con INPUT_PULLUP)
const int BOTON_LIMPIAR = 5;  // Para limpiar la pantalla cuando haya errores je :)
const int BUZZER_PIN = 9;  // Pin para el buzzer


// Estructuras de Datos y Variables de Estado 
const char CLEAR = 0;
const char DOT = 1;
const char DASH = 2;

// Tabla de Alfabeto Morse (Carácter y hasta 5 elementos Morse)
const char Alfabeto[26][6] = {
    {'A', DOT, DASH, CLEAR, CLEAR, CLEAR},
    {'B', DASH, DOT, DOT, DOT, CLEAR},
    {'C', DASH, DOT, DASH, DOT, CLEAR},
    {'D', DASH, DOT, DOT, CLEAR, CLEAR},
    {'E', DOT, CLEAR, CLEAR, CLEAR, CLEAR},
    {'F', DOT, DOT, DASH, DOT, CLEAR},
    {'G', DASH, DASH, DOT, CLEAR, CLEAR},
    {'H', DOT, DOT, DOT, DOT, CLEAR},
    {'I', DOT, DOT, CLEAR, CLEAR, CLEAR},
    {'J', DOT, DASH, DASH, DASH, CLEAR},
    {'K', DASH, DOT, DASH, CLEAR, CLEAR},
    {'L', DOT, DASH, DOT, DOT, CLEAR},
    {'M', DASH, DASH, CLEAR, CLEAR, CLEAR},
    {'N', DASH, DOT, CLEAR, CLEAR, CLEAR},
    {'O', DASH, DASH, DASH, CLEAR, CLEAR},
    {'P', DOT, DASH, DASH, DOT, CLEAR},
    {'Q', DASH, DASH, DOT, DASH, CLEAR},
    {'R', DOT, DASH, DOT, CLEAR, CLEAR},
    {'S', DOT, DOT, DOT, CLEAR, CLEAR},
    {'T', DASH, CLEAR, CLEAR, CLEAR, CLEAR},
    {'U', DOT, DOT, DASH, CLEAR, CLEAR},
    {'V', DOT, DOT, DOT, DASH, CLEAR},
    {'W', DOT, DASH, DASH, CLEAR, CLEAR},
    {'X', DASH, DOT, DOT, DASH, CLEAR},
    {'Y', DASH, DOT, DASH, DASH, CLEAR},
    {'Z', DASH, DASH, DOT, DOT, CLEAR}
};


// Variables de control de tiempo y estado
enum Estado { UP, DOWN };
Estado estado_actual;
unsigned long ultimo_cambio_t = 0;
unsigned long down_duracion = 0;

char morse_actual[5]; 
int morse_i = 0;


// Funciones de Utilidad 
void LimpiaMorse() {
    morse_i = 0;
    for (int i = 0; i < 5; ++i) {
        morse_actual[i] = CLEAR;
    }
}

void LeerMorse() {
    if (down_duracion >= DASH_DURACION - TOLERANCIA && down_duracion <= DASH_DURACION + TOLERANCIA) {
        morse_actual[morse_i] = DASH;
        Serial.println("RAYA");
        morse_i++;
    } else if (down_duracion >= DOT_DURACION - TOLERANCIA && down_duracion <= DOT_DURACION + TOLERANCIA) {
        morse_actual[morse_i] = DOT;
        Serial.println("PUNTO");
        morse_i++;
    } else {
        Serial.print("Pulsacion no reconocida: ");
        Serial.println(down_duracion);
    }
}

char LeerCaracter() {
    for (int i = 0; i < 26; ++i) {
        bool comparar = true;
        for (int j = 0; comparar && j < 5; ++j) {
            if (morse_actual[j] != Alfabeto[i][j + 1]) {
                comparar = false;
            }
        }
        if (comparar) {
            return Alfabeto[i][0];
        }
    }
    return 0;
}
/*
void LimpiarDisplay()
  { if (BOTON_LIMPIAR == 1);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.display();
  }*/

// Setup y Loop
void setup() {
    Serial.begin(9600);
    
    // Inicialización OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("Error: no se pudo iniciar la pantalla OLED"));
        while (true);
    }
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(1, 1);
    display.println("Traductor Morse");
    display.display();
    delay(2000);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.display();

    pinMode(BOTON_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(BOTON_LIMPIAR, INPUT_PULLUP);
    estado_actual = UP;
    LimpiaMorse();
}

void loop() {
    Estado estado_nuevo = (digitalRead(BOTON_PIN) == HIGH) ? UP : DOWN;

    if (estado_nuevo == DOWN) {
        tone(BUZZER_PIN, 100);
    } else {
        noTone(BUZZER_PIN);
    }

    if (estado_nuevo != estado_actual) {
        unsigned long duracion = millis() - ultimo_cambio_t;

        if (estado_actual == DOWN) {
            down_duracion = duracion;
            LeerMorse();
        } else {
            unsigned long up_duracion = duracion;
            
            if (up_duracion >= LONG_GAP - TOLERANCIA) {
                Serial.println("GAP LARGO: ESPACIO");
                display.print(' ');
                display.display();
            } else if (up_duracion >= MEDIUM_GAP - TOLERANCIA) {
                Serial.println("GAP MEDIO: FIN CARACTER");
                char c = LeerCaracter();
                if (c != 0) {
                    display.print(c);
                } else {
                    display.print('?');
                }
                display.display();
                LimpiaMorse();
            }
        }

        ultimo_cambio_t = millis();
        estado_actual = estado_nuevo;

    } else {
        if (estado_actual == UP) {
            unsigned long up_duracion = millis() - ultimo_cambio_t;

            if (morse_i > 0 && up_duracion >= LONG_GAP) {
                Serial.println("Auto-Gap Largo (Esperando nueva palabra)");
                char c = LeerCaracter();
                if (c != 0) { display.print(c); } else { display.print('?'); }
                display.display();
                LimpiaMorse();

                display.print(' ');
                display.display();

                ultimo_cambio_t = millis(); 
            } else if (morse_i > 0 && up_duracion >= MEDIUM_GAP) {
                Serial.println("Auto-Gap Medio (Fin de caracter por inactividad)");
                char c = LeerCaracter();
                if (c != 0) { display.print(c); } else { display.print('?'); }
                display.display();
                LimpiaMorse();
                
                ultimo_cambio_t = millis();
            }
        }
    }
    if (BOTON_LIMPIAR == 0) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.display();
    }
}
