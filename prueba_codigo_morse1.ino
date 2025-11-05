#include <LiquidCrystal_I2C.h>


// Definiciones de tiempos de Morse
const unsigned long DOT_DURACION = 300;  // Duración de un punto 
const unsigned long DASH_DURACION = 3 * DOT_DURACION; // Duración de una raya 
const unsigned long TOLERANCIA = 150;       // Tolerancia en ms para reconocer puntos/rayas/espacios

// Gaps (Tiempo que el botón está liberado)
const unsigned long SHORT_GAP = DOT_DURACION;         // Espacio entre Punto y Raya (1t)
const unsigned long MEDIUM_GAP = 3 * DOT_DURACION;    // Espacio entre caracteres (3t)
const unsigned long LONG_GAP = 7 * DOT_DURACION;      // Espacio entre palabras (7t)

// Pines de conexión
const int BOTON_PIN = 6;  // Pin para el pulsador (conectado a GND con INPUT_PULLUP)
const int BUZZER_PIN = 9;  // Pin para el buzzer

// Configuración del LCD I2C
// (Dirección I2C, Columnas, Filas)
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Estructuras de Datos y Variables de Estado 

// Constantes para los elementos Morse
const char CLEAR = 0;
const char DOT = 1;
const char DASH = 2;

// Tabla de Alfabeto Morse (Carácter y hasta 5 elementos Morse)
// {'Letra', Elem1, Elem2, Elem3, Elem4, Elem5}
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
enum Estado { UP, DOWN } estado_actual; 
unsigned long ultimo_cambio_t = 0; // Tiempo del último cambio de estado (UP a DOWN o viceversa)
unsigned long down_duracion = 0;    // Duración de la pulsación (DOWN)

// Almacenamiento del código Morse actual (máximo 5 elementos)
char morse_actual[5]; 
int morse_i = 0;


// Funciones de Utilidad 

// Limpia el buffer para el siguiente caracter
void LimpiaMorse() {
    morse_i = 0;
    for (int i = 0; i < 5; ++i) {
        morse_actual[i] = CLEAR;
    }
}

// Determina si una duración coincide con PUNTO o RAYA y lo agrega al arreglo
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

// Interpreta el arreglo morse_actual y devuelve el carácter.
char LeerCaracter() {
    for (int i = 0; i < 26; ++i) { //repasa el alfabeto
        bool comparar = true;
        for (int j = 0; comparar && j < 5; ++j) {
     // Compara el elemento del código con la tabla (+1 porque la posición 0 es la letra ojoooo)
            if (morse_actual[j] != Alfabeto[i][j + 1]) {
                comparar = false;
            }
        }
        if (comparar) {
            return Alfabeto[i][0]; // Devuelve la letra (posición 0)
        }
    }
    return 0; // Devuelve 0 si no se encuentra el carácter
}

// Setup y Loop

void setup() {
    Serial.begin(9600); //para que nos devuelva que carancho pasa
    
    // Configuración de la LCD
    lcd.init(); //inicia
    lcd.backlight(); //para que se ilumine y de letra
    lcd.setCursor(0, 0); //donde arranca
    lcd.print("Traductor Morse");
  	lcd.setCursor(0, 1); 
    

    pinMode(BOTON_PIN, INPUT_PULLUP); // Usar resistencia interna PULLUP
    pinMode(BUZZER_PIN, OUTPUT);

    estado_actual = UP;
    LimpiaMorse();
}

void loop() {
    // Leer el estado del botón (INPUT_PULLUP: HIGH=UP, LOW=DOWN)
    Estado estado_nuevo = (digitalRead(BOTON_PIN) == HIGH) ? UP : DOWN;

    // 1. Manejo del Sonido 
    if (estado_nuevo == DOWN) {
        tone(BUZZER_PIN, 1000); // Tono de 1000 Hz mientras se pulsa
    } else {
        noTone(BUZZER_PIN);
    }

    // 2. Transición de Estado (Cambio de UP a DOWN o viceversa)
    if (estado_nuevo != estado_actual) {
        // La duración del estado anterior es (millis() - ultimo_cambio_t
        unsigned long duracion = millis() - ultimo_cambio_t;

        if (estado_actual == DOWN) {
            // TRANSICIÓN DOWN -> UP (Fin de la pulsación)
            down_duracion = duracion;
            LeerMorse(); // Reconoce y guarda DIT o DAH
        } else {
            // TRANSICIÓN UP -> DOWN (Inicio de la pulsación/Análisis de Gap)
            unsigned long up_duracion = duracion;
            
            // Analiza el GAP (tiempo que el botón estuvo liberado)
            if (up_duracion >= LONG_GAP - TOLERANCIA) {
                // Gap largo (>= 7t): Nueva palabra. 
                Serial.println("GAP LARGO: ESPACIO");
                lcd.print(' ');
            } else if (up_duracion >= MEDIUM_GAP - TOLERANCIA) {
                // Gap medio (>= 3t): Fin de carácter. 
                Serial.println("GAP MEDIO: FIN CARACTER");
                char c = LeerCaracter();
                if (c != 0) {
                    lcd.print(c); // Muestra el carácter en LCD
                } else {
                    lcd.print('?'); // Carácter no reconocido
                }
                LimpiaMorse();
            } 
            // Si el Gap es corto (1t), no pasa nada, se espera el siguiente PUNTO o RAYA	.
        }

        ultimo_cambio_t = millis();
        estado_actual = estado_nuevo;

    } else {
        // 3. Estado Estacionario (Comprobar Gaps largos solo en estado UP)
        if (estado_actual == UP) {
            unsigned long up_duracion = millis() - ultimo_cambio_t;
            
            // Si se supera el LONG_GAP (7t), significa que hay un espacio entre palabras.
            // Para evitar escribir múltiples espacios, solo lo hacemos una vez.
            if (morse_i > 0 && up_duracion >= LONG_GAP) {
                // Si hay código Morse previo y el gap es muy largo, es un espacio entre palabras
                Serial.println("Auto-Gap Largo (Esperando nueva palabra)");
                char c = LeerCaracter();
                if (c != 0) { lcd.print(c); } else { lcd.print('?'); }
                LimpiaMorse();

                lcd.print(' '); // Imprime el espacio de palabra

                // Reinicia ultimo_cambio_t para que el siguiente Gap se mida desde ahora.
                ultimo_cambio_t = millis(); 
            } else if (morse_i > 0 && up_duracion >= MEDIUM_GAP) {
                // Si se detecta el gap de 3t mientras está en UP (sin haber pulsado)
                Serial.println("Auto-Gap Medio (Fin de caracter por inactividad)");
                char c = LeerCaracter();
                if (c != 0) { lcd.print(c); } else { lcd.print('?'); }
                LimpiaMorse();
                
                // Reinicia ultimo_cambio_t para no repetir la acción
                ultimo_cambio_t = millis();
            }
        }
    }
}