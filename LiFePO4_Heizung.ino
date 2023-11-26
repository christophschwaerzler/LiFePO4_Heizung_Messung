// Diese Messungen dienen der Beurteilung der Isolierung und Heizung des LiFePO4-Akkumulators
// 
// Mit einem Temperatursensor DS18B20 wird die Aussentemperatur gemessen und mit einem
// Lichtensor die Ein- und Ausschaltzeiten der Heizung. Die Messdaten werden im EEPROM abgespeichert
// und bei jedem Neustart des uC auf dem seriellen Ausgang mit 9600 Baud ausgegeben
//
// uC:               Atmega328P on Arduino Uno R3
// Externe Hardware: Temperatursensor DS18B20, angeschlossen an GND, +5V und Pin 2
//                   Lichtsensor, zwischen GND, +5V und Pin A0
//                   Taster (Ein), zwischen GND und Pin 5
// Stromaufnahme:    50 mA
//
// Christoph Schwaerzler, OE1CGS
// Nov. 2023

#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define TempPin                      2 // Pin D2 fuer DS18B20 Temperatursensor
#define LichtPin                    A0 // Pin A0 fuer Lichtsensor
#define TasterPin                    5 // Pin D5 fuer den Taster, der den Messvorgang startet
#define LEDPin                      13 // Pin D13 fuer eingebaute LED zur Bestaetigung des Beginns der Messung
#define LichtSchwelle               50 // Grenzwert des Lichtsensors, unter der Lichtschwell wird als dunkel, darueber als hell interpretiert

// Variablendeklarationen
bool lflag                    = false; // Zustand des Lichtsensors, Dunkel = false, Hell = true
bool lastlflag                = false; // Letzter Zustand des Lichtsensors
int i                             = 0; // Lokaler Schleifenzaehler
int time                          = 0; // Zeit seit Programmstart in ganzen Sekunden
int licht                         = 0; // 10-bit A/D-Wandler Wert des Lichtsensors (0 = Dunkel, 1023 = Hell)
float tempC                       = 0; // Temperatur in Grad Celsius

OneWire oneWire(TempPin);              // An Pin 'TempPin' (a 4.7K resistor is necessary)
DallasTemperature sensors(&oneWire);   // Pass our oneWire reference to Dallas Temperature sensor

void setup() {
  pinMode(LEDPin, OUTPUT);
  pinMode(TasterPin, INPUT_PULLUP);
  sensors.begin();                    // Initialisiere des DS18B20 Sensor
  Serial.begin(9600);                 // Initialisiere die serielle Schnittstelle
  while(!Serial){;}                   // Wait for serial port to connect
  Serial.print("t[s]");
  Serial.print("\t");
  Serial.print("T[°C]");
  Serial.print("\t");
  Serial.println("Licht[a.u.]");
}

void loop() {
  // EEPROM-Inhalt auf serieller Schnittstelle ausgeben
  i=0;
  while (i < EEPROM.length()){
    EEPROM.get(i,   time);                   // Zeit wird als Integer abgespeichert (2 Byte)
    EEPROM.get(i+2, tempC);                  // Temperatur wird als Floating abgespeichert (4 Byte), die Adresse ist um 2 hoeher
    EEPROM.get(i+6, licht);                  // Lichtstaerke wird als Integer abgespeichert (2 Byte), die Adresse ist um 6 hoeher 
    Serial.print(time);
    Serial.print("\t");
    Serial.print(tempC);
    Serial.print("\t");
    Serial.println(licht);
    i = i + 8;                                // Insgesamt hat ein Datensatz 8 Byte, daher ist die naechste EEPROM-Adresse um 8 hoeher
  }

  // Warten auf Tastendruck fuer Messbeginn und Bestaetigung durch 3-maliges Blinken der LED
  while (digitalRead(TasterPin)){}
  i=0;                                                     // Auf Startbefehl fuer Messung durch Tastendruck warten
  while (i<5){digitalWrite(LEDPin, !digitalRead(LEDPin));delay(500);i++;}   // 3-maliges Blinken als Bestaetigung fuer Messbeginn
  
  // EEPROM loeschen um fuer neue Messung bereit zu sein
  i=0;
  while (i < EEPROM.length()){EEPROM.write(i,0);i++;}      // Schreibt "0" in alle EEPROM-Speicherplaetze (=loeschen)

  // Messschleife, wird solange durchlaufen, bis das EEPROM voll ist oder Strom abgeschaltet wird
  i=0;
  while (i < EEPROM.length() - 8){                         // Die Messung geht so lange, bis der EEPROM-Speicher voll ist
  
  // Test auf Zustandsaenderung des Lichtsensors
  while (lflag == lastlflag){
  // Messung des Lichtsensors
  licht = analogRead(LichtPin);                            // Abfrage des Lichtsensors liefert einen Wert zwischen 0 und 1023
  if (licht <= LichtSchwelle){                             // Lichtsensorwerte unter der LichtSchwelle werden als dunkel interpretiert
    lflag = false;}
  else                                                     // -werte ueber der LichtSchwelle werden als hell interpretiert
    {lflag = true;}
  }                                                        // Weiter erst, wenn sich der Zustand dunkel/hell aendert
  lastlflag = lflag;
  // Messungen der Zeit und Aussentemperatur
  time = (unsigned int)(millis()/1000);                    // Zeit seit Start des uC in ganzen Sekunden, Ueberlauf nach 65536 Sekunden (ca. 18,2 h)
  sensors.requestTemperatures();                           // Request temperature from all DS18B20 sensors on the bus
  tempC = sensors.getTempCByIndex(0);                      // Get temperature in Celsius for the first DS18B20 sensor found on the bus
  // Abspeichern der aktuellen Messdaten
  EEPROM.put(i,   time);                                   // Zeit wird als Integer abgespeichert (2 Byte)
  EEPROM.put(i+2, tempC);                                  // Temperatur wird als Floating abgespeichert (4 Byte), die Adresse ist um 2 hoeher
  EEPROM.put(i+6, licht);                                  // Lichtstaerke wird als Integer abgespeichert (2 Byte), die Adresse ist um 6 hoeher
  delay(500);                                              // Wartet eine halbe Sekunde bevor neuer Messwert abgefragt wird (wegen Stabilitaet Fotosensor)
  i = i + 8;                                               // Erhoehung der EEPROM-Adresse fuer die naechsten Messdaten
  }
  digitalWrite(LEDPin, 0);                                 // LED ausschalten als Zeichen, dass Messung beendet ist
}