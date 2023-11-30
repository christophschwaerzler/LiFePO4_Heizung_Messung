# LiFePO4_Heizung_Messung
Messung des Heizungsverhaltens einer Isolierung und Heizung für einen LiFePO4 Solarakku im Freien.
Das Projekt ist ausführlich auf meiner Homepage beschrieben: http://www.oe1cgs.at/isolierung-und-heizung-fuer-lifepo4-akkumulator/

Der Solarakku befindet sich zusammen mit einem Heizkabel (Nominalleistung: 30 W) in einer Box aus 3cm dicken XPS-Platten:

![Box](https://github.com/christophschwaerzler/LiFePO4_Heizung_Messung/assets/151140591/de77048b-01fb-4393-b99c-e6d769636ab2)

Dieses Projekt dient der Quantifizierung des Heizverhaltens (insbesondere die erforderliche Heizleistung) und der Beurteilung der Effektivitaet der Isolierung.

Dazu wurde auf Basis eines Arduino UNO R3 boards, einem Temperatursensor DS18B20, einem Fotowiderstand sowie einem externen Taster eine einfache Messanordnung realisiert.
Der Fotowiderstand wird direkt vor der Zustandsanzeige (LED) des Heizungscontrollers angebracht und erfasst damit, ob die Heizung ein- oder ausgeschaltet ist.
Bei jedem Zustandswechsel wird die Zeit (in Sekunden seit dem Messbeginn) und die Außentemperatur gemessen und im EEPROM abgespeichert, bis entweder
der EEPROM-Speicher voll ist, oder die Messung durch Trennung von der Stromversorgung beendet wird.

Sobald der uC erneut gestartet wird, gibt er als erstes sämtliche abgespeicherte Messwerte am seriellen Ausgang mit 9600 Baud aus.
Diese Daten können mit einem Terminalprogramm (z.B. PuTTY, Termite...) oder auch mit der Arduino IDE erfasst und in einer Tabellenkalkulation ausgewertet werden.
Danach wartet er auf eine Betätigung des externen Tasters, um den Speicher zu löschen und eine erneute Messung zu beginnen.

Die Auswertung der Messdaten hat die Tauglichkeit der Isolierung und Heizung des Solarakkus bestätigt. Die damit berechnet Mindestaußentempertur
beträgt -44°C, was für einen Einsatz in Mitteleuropa ausreichend sein sollte. Rund um den Gefrierpunkt ist die mittlere Leistungsaufnahme der
Heizung @@ W. Der aus den Messdaten ermittelte Wärmewiderstand der Isolierung beträgt @@ W/K.
