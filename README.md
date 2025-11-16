/*
Proyecto: Traductor de Codigo Morse con Arduino

Descripcion del sistema:
Este proyecto implementa un traductor de codigo Morse utilizando una placa Arduino UNO,
un pulsador, un buzzer piezoelectrico y una pantalla OLED 128x32 por I2C. El sistema permite ingresar
letras mediante pulsaciones (puntos y rayas) y traduce el codigo Morse en texto alfanumerico
mostrado en la pantalla OLED, con salida sonora que acompaña cada senal. Ademas se agrego
un segundo boton dedicado a limpiar la pantalla cuando se presiona.

------------------------------------------------------
Integrantes del grupo:
------------------------------------------------------
- Santino Pelle — spelle@unsam-edu.com.ar
- Matias Mantinan — matiasmantinan11@gmail.com
- Juan Ezequiel Fornes — jefornes@estudiantes.unsam.edu.ar
- Diego Alexander Mammana — mammanad@gmail.com

------------------------------------------------------
Docentes y materia:
------------------------------------------------------
Materia: Laboratorio de Computacion 1

Docentes: Matias Jose Gagliardo y Pedro Facundo Iriso

Institucion: Universidad Nacional de San Martin (UNSAM)

------------------------------------------------------
Descripcion general y funcionalidades:
------------------------------------------------------
- Cada pulsacion corta representa un punto (·)
- Cada pulsacion larga representa una raya (–)
- El buzzer emite un sonido durante la pulsacion
- La pantalla OLED (128x32 por I2C) muestra el texto traducido letra por letra
- Los espacios entre pulsaciones determinan la separacion entre letras y palabras
- Boton extra para limpiar la pantalla en cualquier momento

------------------------------------------------------
Requisitos funcionales cumplidos:
------------------------------------------------------
- Control de entradas y salidas digitales y analogicas
- Contador de flancos para detectar cambios de estado del boton principal
- Control logico basado en tiempos (puntos, rayas y pausas)
- Implementacion de una maquina de estados (UP / DOWN)
- Visualizacion del texto traducido en pantalla OLED
- Emision de sonido con buzzer durante las entradas
- Boton adicional para limpiar la pantalla

------------------------------------------------------
Componentes utilizados:
------------------------------------------------------
| Componente | Descripcion | Conexion principal |
|-------------|-------------|--------------------|
| Arduino UNO | Microcontrolador principal | USB |
| Pulsador principal | Entrada de usuario (puntos/rayas) | Pin 6 (INPUT_PULLUP) |
| Pulsador secundario | Limpieza de pantalla | Pin 7 |
| Buzzer piezoelectrico | Salida sonora | Pin 9 |
| Pantalla OLED 128x32 (I2C) | Salida visual | Pines SDA/SCL |
| Protoboard y cables | Conexiones de montaje |  |

------------------------------------------------------
Esquema electrico / Diagrama de conexiones:
------------------------------------------------------
![Captura](img/diagramaconexiones.png)
------------------------------------------------------
Maquina de estados:
------------------------------------------------------
Estados principales:
- UP: el boton no esta presionado. Se mide la pausa para determinar fin de caracter o palabra.
- DOWN: el boton esta presionado. Se mide la duracion de la pulsacion para decidir si es punto o raya.

Transiciones:
- UP -> DOWN: inicio de una pulsacion
- DOWN -> UP: fin de pulsacion, se interpreta punto o raya
- Intervalos entre pulsaciones determinan cierre de letra o palabra

------------------------------------------------------
Instrucciones de uso y ejecucion:
------------------------------------------------------
1. Conectar el circuito segun el diagrama.
2. Cargar el codigo al Arduino desde el IDE.
3. Encender el sistema.
4. Mantener presionado el boton principal:
   - Pulsacion corta -> punto (·)
   - Pulsacion larga -> raya (–)
5. Soltar el boton y esperar:
   - Pausa corta -> misma letra
   - Pausa media -> fin de letra
   - Pausa larga -> fin de palabra
6. Presionar el segundo boton para limpiar la pantalla cuando se desee.
7. Ver la traduccion en la pantalla OLED.

------------------------------------------------------
Imagenes o video demostrativo:
------------------------------------------------------
- Diagrama de conexion: incluido arriba
- Video demostrativo (opcional): puede agregarse un enlace a YouTube o Google Drive

------------------------------------------------------
Licencia:
------------------------------------------------------
Este proyecto esta bajo la Licencia MIT, lo que permite su uso, copia, modificacion y
distribucion con fines educativos o personales, siempre que se mencionen los autores originales.

------------------------------------------------------
Creditos:
------------------------------------------------------
Proyecto desarrollado por estudiantes de Laboratorio de Computacion 1 de la UNSAM, año 2025.

Basado e inspirado en el trabajo original:
"The Morse Translator With Arduino"
By Fred_RK en Instructables (Circuits)
Diagrama realizado con Tinkercad.

El desarrollo se realizo con trabajo grupal, referencias del proyecto original, una pequeña asistencia de herramientas de IA y Diagrama realizado con Tinkercad.

Fin del README
*/
