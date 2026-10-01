# Embedded Systems Labor – Konzept

*Zielplattform: MSP430F5335 · 15 Laboreinheiten · Einheiten 1–11 geführt, 12–15 frei*

---
## TODOs

- [ ] Einheit 5 (Nachholeinheit): Text/Refinement ergänzen, aktuell nur Überschrift
- [ ] Finale Übersichtstabelle mit allen Einheiten gegen die Fließtexte gegenprüfen
- [ ] Implementierung aller Einheiten, bzw. refactoring der bestehenden Einheiten
- [ ] (LV) auch für die neuen Inhalte des LABs nachziehen

## Grundsätzliche Fragen die sich während der Aufbereitung klären oder eben festgelegt werden müssen
- [ ] Wann führen wir explizit FreeRTOS ein? Eher noch etwas früher? Besten Zeitpunkt während der Aufbereitung erfassen. 
- [ ] Möglicher Passender FreeRTOS-Einstieg nach Einheit 5? (also vor SPI I2C) Scheduler könnte dann später direkt für Display-Messages bzw. "Fetching" der Sensorwerte verwendet werden. 
- [ ] Layer-Mocking aufbereiten 
- [ ] Mocking Idee für Regelparameter entwerfen, Simulator? (Python Portierung des Matlabsimulators)
- [ ] Hardware in the Loop, Fahrzeugsimulator kommuniziert mit Fahrzeug
- [ ] Einheit 10 und 11 könnte man auch noch zusätzlich zusammenfassen, damit könnte man noch eine "Nachholeinheit" im LAB-Verlauf einbauen um individuelle Probleme nachziehen zu können

## Übersicht

| # | Einheit | Implementierung | Testverantwortung (Unity) | Messung / Verifikation | FreeRTOS |
|---|---|---|---|---|---|
| 1 | Grundlagen, Werkzeuge, Registereinstieg (GPIOs) | Register | HAL-Tests bereitgestellt | – (Fokus: Setup, Datenblätter, Registerverständnis) | – |
| 2 | Rechendauer, Interrupts, ISRs | Register | Tests weiterhin bereitgestellt | Rechendauer int vs. float; Polling vs. Delay (zeitliche Zuverlässigkeit) | – |
| 3 | SystemClock, TimerB | Register oder Bibliothek (datenblattbasiert) | Vorgegebenes Gerüst, Grenzfälle selbst ergänzen | Soll- vs. Ist-Taktfrequenz (Oszilloskop) | – |
| 4 | TimerA, PWM | Bibliothek | Vorgegebenes Gerüst; MockHAL für DL-Tests eingeführt | Signalform, Duty-Cycle (Oszilloskop); Drehzahl, Geschwindigkeit, Weg über TimerA | – |
| 5 | Nachholeinheit | – | Konsolidierung offener Tests aus 1–4 | – | – |
| 6 | SPI/I2C-Schnittstelle (Messungen) | Bibliothek | überwiegend eigenständig, nur Anforderungen vorgegeben | Timing/Protokollverlauf SPI & I2C messen und dekodieren (Logic-Analyzer); korrekter Aufbau der Serviceroutinen | – |
| 7 | SPI (Display), I2C (Abstandssensoren) | Bibliothek | eigenständig, mit Mock-HAL | Sensoren korrekt eingelesen (laut Anleitung); Gerätetreiber funktional | – |
| 8 | ADC-Konfiguration, Batteriemessung, Direct Memory Access | ADC: Bibliothek / DMA: Bibliothek | eigenständig; Regressionsnachweis der DL-Tests nach DMA-Umstellung | Batteriespannung (Ladestatus); Rechenzeitvergleich, Timings | – |
| 9 | FreeRTOS-Aufsetzen, Scheduling | Bibliothek (FreeRTOS) | Konsolidierung 1–8 zu Beginn; danach neue Kategorie (Ablauf-/Scheduling-Tests) | Task-Timing: Latenz | ab hier aktiv |
| 10 | Fahralgorithmus (Zustandsautomat) | reine Logik | Host-Tests eigenständig; Mocking wird bereitgestellt und mit Studierenden besprochen | Host-Test, CrazyCar-Simulator, Nachweis am realen Fahrzeug (Zustandswechsel provoziert) | aktiv |
| 11 | Regelungsprozess (PID) | reine Logik | AL-Unity-Tests, umfangreichste Suite; Mocking aller anderen Layer eingebunden | Reglerparameter im CrazyCar-Simulator variieren (bzw. gegebene Mocking-Szenarien); Feinabstimmung am realen Fahrzeug | aktiv |
| 12–15 | Freies Üben | frei gewählt | vollständige Suite bleibt verpflichtend | Systemintegration, eigene Erweiterung, Optimierung, Abschlussdemonstration | aktiv |

---

## Einheit 1 – Grundlagen, Werkzeuge, Registereinstieg (GPIOs)

Die erste Einheit dient dem Einstieg, in die einzelnen Tools und in die Zielarchitektur (HAL, DL, AL). Die relevanten Datenblätter, Controller sowie eingesetzte Fahrzeugkomponenten, werden präsentiert. Das Fahrzeug wird als Gesamtsystem vorgestellt: Aufbau, verbaute Sensorik und Aktorik, Zielarchitektur des gesamten Kurses.

Code Composer Studio wird aufgesetzt: Projektanlage, Toolchain-Konfiguration, Verbindung zum Debugger. Für sämtliche beschalteten Pins des Controllers legen die Studierenden eine Header-Datei mit sprechenden Bezeichnungen an. Diese Pin-Dokumentation wird in den Folgeeinheiten weiterverwendet.

Anschließend erfolgt die klassische Einführung in die Registerebene: Adressraum, Memory-Mapped I/O, anhand des Datenblatts. Der Registerzugriff wird auch im Code eingeführt, anhand konkreter Zuweisungen und Bitmasken, nicht nur theoretisch (Setzen von Aus- und Eingängen, Pull-Ups bzw. einer Funktion, die die Logikschaltung der jeweiligen Pins am HAL setzt, je nachdem was man mit den Pins machen möchte). Eine "GPIO-Config"-Funktion wird erstellt und über die ersten Einheiten hinweg erweitert.

Die Unity-Tests für den HAL zu dieser Einheit werden als vollständige Vorlage bereitgestellt. So liegt ein Referenzstandard vor, und Studierende können sich auf Einführung, Datenblätter, GPIO-Beschaltungen und den Umgang mit Code Composer konzentrieren.

**Wesentliche Arbeitsschritte:**
- Projekt- und Zielarchitektur-Überblick (HAL, DL, AL)
- Datenblätter (Controller + Fahrzeugkomponenten) präsentieren
- Fahrzeug als Gesamtsystem vorstellen (Sensorik, Aktorik)
- Code Composer Studio aufsetzen (Projekt, Toolchain, Debugger)
- Header-Datei mit Pin-Bezeichnungen für alle beschalteten Pins anlegen
- Einführung Registerebene: Adressraum, Memory-Mapped I/O
- Registerzugriff im Code: Zuweisungen, Bitmasken
- Erste "GPIO-Config"-Funktion im HAL erstellen (Ein-/Ausgang, Pull-Ups) – Basis für spätere Erweiterung
- HAL-Unity-Tests als bereitgestellte Vorlage übernehmen

## Einheit 2 – Rechendauer, Interrupts, ISRs

Gemessen wird die Rechendauer einzelner Rechenoperationen, abhängig von der Architektur des Controllers: Ganzzahlarithmetik (int) gegenüber Fließkommaarithmetik (float). Auf einem Controller ohne Fließkommaeinheit ergibt sich hier ein deutlich messbarer Unterschied.

Die bestehenden HAL-Funktionen werden erweitert: Interruptfähigkeit, Pull-Up, Pull-Down usw. – weiterhin auf Registerebene, ausschließlich für die GPIO-Abstraktion. Ein Taster löst über einen Portinterrupt eine ISR aus; mithilfe der ISR wird das Backlight des CrazyCars getoggelt.

Studierende realisieren den Unterschied zwischen Polling und Delay hinsichtlich ihrer zeitlichen Zuverlässigkeit (Timings). Individuelle Diskussion mit den Studierenden. 

Die zugehörigen Unity-Tests werden weiterhin als vollständige Vorlage bereitgestellt.

**Wesentliche Arbeitsschritte:**
- Rechendauer einzelner Operationen messen: int vs. float (architekturabhängig)
- Bestehende HAL-Funktionen erweitern: Interruptfähigkeit, Pull-Up, Pull-Down (weiterhin Registerebene, reine GPIO-Abstraktion)
- Taster löst über Portinterrupt eine ISR aus (Fokus auf Entprellen richten)
- ISR toggelt das Backlight des CrazyCars
- Polling vs. Delay messen – zeitliche Zuverlässigkeit vergleichen
- Diskussion: Polling vs. ISR
- Unity-Tests weiterhin als vollständige Vorlage bereitgestellt

## Einheit 3 – SystemClock, TimerB

Ab dieser Einheit werden zur Verfügung gestellte Libraries verwendet und die System Clock sowie der Timer werden anhand dieser Libraries und den Datenblättern konfiguriert.

Die Konfiguration erfolgt in jedem Fall datenblattbasiert: unabhängig davon, ob über Register oder über eine Bibliotheksfunktion konfiguriert wird, müssen die verwendeten Werte aus dem Datenblatt begründet werden können. Die Taktrequenz wird konfiguriert und durch eine Messung am PIN überprüft. (Übung Oszilloskop). Die Testverantwortung verschiebt sich auf ein vorgegebenes Gerüst, das um Grenzfälle ergänzt wird.

**Wesentliche Arbeitsschritte:**
- Einführung bereitgestellter Libraries ab dieser Einheit
- SystemClock über Library konfigurieren
- TimerB über Library konfigurieren
- Konfigurationswerte datenblattbasiert begründen (unabhängig von Register oder Bibliothek)
- Tatsächliche vs. konfigurierte Taktfrequenz am Oszilloskop messen
- Unity Tests: vorgegebenes Gerüst, Grenzfälle selbst ergänzen

## Einheit 4 – TimerA, PWM

Gleiches Prinzip wie in Einheit 3. Timer A wird für die spätere Verwendung der Aktorik aufgesetzt. Signalform und Duty-Cycle werden am Oszilloskop verifiziert, unabhängig vom gewählten Implementierungsweg. Die Umrechnung von Lenkwinkel zu Duty-Cycle bleibt in jedem Fall eine eigenständige, host-testbare Funktion.

Hier wird der Driver Layer für die Umrechnung des gegebenen Lenkwinkels "eingeführt". Die Studierenden sollen einen sauberen Übergang zwischen HAL und DL schaffen. (Wenn bisher noch nicht gemacht). An dieser Stelle wird zum Test des Driver Layers ein MockHAL eingeführt, welches für die Unity Tests verwendet werden soll.

**Wesentliche Arbeitsschritte:**
- TimerA für spätere Aktorik-Nutzung aufsetzen (gleiches Prinzip wie Einheit 3)
- Signalform und Duty-Cycle am Oszilloskop verifizieren, unabhängig vom Implementierungsweg
- Umrechnung Lenkwinkel → Duty-Cycle als eigenständige, testbare Funktion umsetzen
- Messung der Drehzahl mittels Timer A, Ermittlung der Geschwindigkeit (mm/s) sowie des zurückgelegten Weges
- Driver Layer (DL) für diese Umrechnung einführen
- Sauberen Übergang zwischen HAL und DL schaffen
- MockHAL zum Testen des Driver Layer einführen
- Unity Tests: vorgegebenes Gerüst, Grenzfälle selbst ergänzen

## Einheit 5 - Nachholeinheit

## Einheit 6 – SPI/I2C-Schnittstelle (Messungen)

 Der Schwerpunkt der Einheit liegt auf der Verifikation. Beide Schnittstellen werden konfiguriert, Timing und Protokollverlauf werden am Logic-Analyzer geprüft. Die Tests werden ab dieser Einheit überwiegend eigenständig geschrieben, vorgegeben sind nur noch die Anforderungen an die Tests. 

Wichtig ist dabei, die einzelnen Schnittstellen – SPI und I2C – mit dem zur Verfügung gestellten Analyzer zu messen und zu dekodieren. Die Verifizierung, dass die Schnittstellen sowie die einzelnen Serviceroutinen korrekt aufgesetzt wurden, ist notwendig.

**Wesentliche Arbeitsschritte:**
- Konfiguration über Register oder Bibliothek – freie Wahl
- SPI- und I2C-Schnittstelle in Betrieb nehmen
- Beide Schnittstellen mit dem gegebenen Analyzer messen und dekodieren
- Timing und Protokollverlauf gegen Spezifikation prüfen
- Korrekten Aufbau der Schnittstellen und der zugehörigen Serviceroutinen verifizieren
- HAL-Tests überwiegend eigenständig schreiben, nur Anforderungen vorgegeben

## Einheit 7 – SPI (Display), I2C (Abstandssensoren)

Die einzelnen Sensoren werden eingelesen und laut Anleitung / Datenblatt konfiguriert. Driver-Layer-Funktionen sollen erstellt werden, um über I2C einzelne Sensorwerte auszulesen. Analog sollen über SPI Daten an das Display geschickt werden, auch hier über Funktionen aus dem Driver Layer.

Aufbauend auf der Schnittstellenkonfiguration aus Einheit 7 entstehen die konkreten Gerätetreiber. Die DL-Tests werden mit einer Mock-HAL eigenständig erstellt.

**Wesentliche Arbeitsschritte:**
- Sensoren gemäß Anleitung einlesen und korrekt konfigurieren
- DL-Funktionen zum Auslesen einzelner Sensorwerte über I2C erstellen
- DL-Funktionen zum Senden von Daten an das Display über SPI erstellen
- Gerätetreiber auf der Schnittstellenkonfiguration aus Einheit 7 aufbauen
- DL-Tests mit Mock-HAL eigenständig erstellen

## Einheit 8 – ADC-Konfiguration, Batteriemessung, Direct Memory Access

Diese Einheit verbindet ADC-Konfiguration und Direct Memory Access zu einem gemeinsamen Themenblock, da die eingelesenen ADC Daten für das (post-processing) automatisch weiterverarbeitet werden sollen. (Kann dann auch gleich für I2C Interrupts weiter adaptiert werden) Zunächst wird der ADC in Betrieb genommen. Die Batteriespannung wird gemessen; die Umrechnung des Rohwerts in einen aussagekräftigen Spannungswert (Oder Ladestatus) erfolgt über eine eigenständige DL-Funktion, die eigenständig getestet wird.

Die Distanzmessung der IR-Sensoren wird den Studierenden mitsamt Dokumentation zur Verfügung gestellt – die Umrechnung von Rohwert zu Distanz muss nicht selbst hergeleitet werden, sondern wird als fertiger, dokumentierter Baustein übernommen und in die DL-Schicht eingebunden. Ob Studierende Teile oder alle Elemente der Linearisierung selbst erledigen, wird in der Vorbereitung noch geklärt.
In der Praxis wird dabei überwiegend die Bibliotheksvariante gewählt. Die im ersten Teil dieser Einheit entstandenen DL-Tests müssen nach der Umstellung unverändert bestehen – das dient als Nachweis, dass sich an der nach außen sichtbaren Schnittstelle nichts geändert hat.

**Wesentliche Arbeitsschritte:**
- ADC in Betrieb nehmen
- Batterie messen, Umrechnung als eigenständige DL-Funktion erstellen und eigenständig testen 
- Bereitgestellte, dokumentierte Distanzmessung der IR-Sensoren übernehmen und in DL einbinden
- Datenerfassung (Batterie + Abstands-Sensoren) mit DMA-Zugriff umstellen (überwiegend Bibliotheksvariante)
- Bestehende DL-Tests müssen nach der Umstellung unverändert bestehen

## Einheit 9 – FreeRTOS-Aufsetzen, Scheduling

Zu Beginn der Einheit werden sämtliche offenen Tests aus den vorangegangenen Einheiten fertiggestellt; die gesamte bisherige Testsuite muss vollständig bestehen, bevor mit neuem Inhalt begonnen wird. Anschließend wird FreeRTOS als Bibliothek eingebunden. Aufgebaut wird die Architektur *Fetch Sensor Data → Process Sensor Data → Make Decisions* als Task-Pipeline. 
Eine Task ruft die bestehenden DL-Funktionen zur Datenerfassung auf, eine zweite verarbeitet die Werte, eine dritte trifft Entscheidungen – letztere zunächst als Platzhalter, inhaltlich gefüllt ab Einheit 10. Es entsteht eine neue Testkategorie die bestehenden HAL- und DL-Tests bleiben davon unberührt. Mocking der Fahrzeugumgebung. 

**Wesentliche Arbeitsschritte:**
- Offene Tests aus den vorangegangenen Einheiten fertigstellen, gesamte Suite muss vollständig bestehen
- FreeRTOS als Bibliothek einbinden
- Task-Pipeline Fetch Sensor Data → Process Sensor Data → Make Decisions aufsetzen
- Decision-Stufe zunächst als Platzhalter anlegen (inhaltlich gefüllt in Einheit 10)
- Timing zwischen Tasks messen bzw. verifizieren
- Neue Testkategorie (Ablauf-/Scheduling-Tests); HAL- und DL-Tests bleiben unberührt

## Einheit 10 – Fahralgorithmus (Zustandsautomat)

Der Zustandsautomat füllt die Decision-Stufe aus Einheit 9 inhaltlich. Die Verifikation erfolgt auf drei Ebenen: Host-Tests wie bisher, eine Überprüfung des Fahrverhaltens im CrazyCar-Simulator unter kontrollierten Bedingungen, sowie abschließend ein Nachweis am realen Fahrzeug, bei dem ein Zustandswechsel gezielt provoziert und dokumentiert wird.

**Wesentliche Arbeitsschritte:**
- State-Machine für Fahralgorithmus besprechen und entwickeln
- Tasks für das Scheduling erstellen
- Zustandsautomat host-testen
- Fahrverhalten im CrazyCar-Simulator unter kontrollierten Bedingungen prüfen
- Nachweis am realen Fahrzeug: Zustandswechsel gezielt provozieren und dokumentieren
- Mocking für Unity-Tests bereitsstellen (wird bereitgestellt und mit Studierenden besprochen)

## Einheit 11 – Regelungsprozess (PID)

Eine systematische Variation einzelner Reglerparameter ist am realen Fahrzeug nur eingeschränkt reproduzierbar, da Randbedingungen wie Batteriestand oder Bodenhaftung nicht konstant gehalten werden können. Für diesen Zweck eignet sich der CrazyCar-Simulator besser: Einzelne Parameter lassen sich dort unter gleichbleibenden Bedingungen gezielt variieren. Am realen Fahrzeug erfolgt anschließend die Feinabstimmung sowie der Nachweis, dass die Regelung unter realen Bedingungen funktioniert. Die AL-Tests erreichen in dieser Einheit den größten Umfang im gesamten Kurs.

**Wesentliche Arbeitsschritte:**
- PID-Regler entwickeln
- Einzelne Reglerparameter zusätzlich im CrazyCar-Simulator unter reproduzierbaren Bedingungen variieren (Oder durch gegebene Mocking-Szenarios)
- Feinabstimmung am realen Fahrzeug durchführen
- Nachweis, dass die Regelung unter realen Bedingungen funktioniert
- AL-Unity Test, Mocking aller anderen Layer einbinden

## Einheit 12–15 – Freies Üben

Systemintegration, individuelle Erweiterung, Optimierung und Vorbereitung für das Crazy Car Event. Freies Training in einem dafür vorgesehenen Raum, innerhalb eingetragener Trainingsslots, für individuelle Betreuung.

---

*Neustrukturierung der ES-LAB Inhalte, Stand Juli 2026*
