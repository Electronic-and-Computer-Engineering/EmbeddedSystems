# Embedded Systems Labor – Konzept

*Zielplattform: MSP430F5335 · 15 Laboreinheiten · Einheiten 1–11 geführt, 12–15 frei*

---

## Übersicht

| # | Einheit | Implementierung | Testverantwortung | FreeRTOS |
|---|---|---|---|---|
| 1 | Grundlagen, Werkzeuge, Registereinstieg (GPIOs) | Register | bereitgestellt | – |
| 2 | Rechendauer, Interrupts, ISRs | Register | bereitgestellt | – |
| 3 | SystemClock, TimerB | Register oder Bibliothek (datenblattbasiert) | Gerüst | – |
| 4 | TimerA, PWM | Register oder Bibliothek | Gerüst | – |
| 5 | SPI/I2C-Schnittstelle (Messungen) | Register oder Bibliothek | überwiegend eigenständig | – |
| 6 | SPI (Display), I2C (Abstandssensoren) | Register oder Bibliothek | eigenständig | – |
| 7 | ADC-Konfiguration, Batteriemessung | Register oder Bibliothek | eigenständig | – |
| 8 | Direct Memory Access | Bibliothek | eigenständig, Regressionsnachweis Einheit 7 | – |
| 9 | FreeRTOS-Aufsetzen, Scheduling | Bibliothek (FreeRTOS) | Konsolidierung 1–8, danach neue Kategorie | ab hier aktiv |
| 10 | Fahralgorithmus (Zustandsautomat) | reine Logik | eigenständig | aktiv |
| 11 | Regelungsprozess (PID) | reine Logik | eigenständig | aktiv |
| 12–15 | Freies Üben | frei gewählt | vollständige Suite bleibt verpflichtend | aktiv |

---

## Einheit 1 – Grundlagen, Werkzeuge, Registereinstieg (GPIOs)

Die erste Einheit dient dem Einstieg in Projekt, Werkzeug und Zielarchitektur. Die relevanten Datenblätter – Controller sowie die eingesetzten Fahrzeugkomponenten – werden gemeinsam durchgearbeitet. Das Fahrzeug wird als Gesamtsystem vorgestellt: Aufbau, verbaute Sensorik und Aktorik, Zielarchitektur des gesamten Kurses.

Code Composer Studio wird aufgesetzt: Projektanlage, Toolchain-Konfiguration, Verbindung zum Debugger. Für sämtliche beschalteten Pins des Controllers wird eine Header-Datei mit sprechenden Bezeichnungen angelegt – die Pin-Dokumentation ist das erste Artefakt des Projekts und wird in den Folgeeinheiten weiterverwendet.

Anschließend erfolgt die klassische Einführung in die Registerebene: Adressraum, Memory-Mapped I/O, Aufbau eines Konfigurationsregisters anhand des Datenblatts. Der Registerzugriff wird auch im Code eingeführt – anhand konkreter Zuweisungen und Bitmasken, nicht nur theoretisch.

Als praktische Anwendung wird ein Pin per Software geschaltet; Frequenz und Pulsbreite der resultierenden Toggle-Schleife werden am Logic-Analyzer erfasst. Flankensteilheit (Schaltzeiten im engeren Sinn) ist mit einem Logic-Analyzer nicht sinnvoll erfassbar und bleibt daher ausgeklammert. Die HAL-Tests zu dieser Einheit werden als vollständige Vorlage bereitgestellt, damit ein Referenzstandard für gute Tests vorliegt.

## Einheit 2 – Rechendauer, Interrupts, ISRs

Weiterhin Registerebene. Ein Taster löst über einen Portinterrupt eine ISR aus; die Latenz zwischen Ereignis und Reaktion wird gemessen. Da diese Einheit explizit die Rechendauer thematisiert, wird zusätzlich ein direkter Vergleich durchgeführt: dieselbe Berechnung wird einmal in Fließkomma- und einmal in Ganzzahlarithmetik ausgeführt, die jeweilige Rechendauer gemessen. Auf einem Controller ohne Fließkommaeinheit ergibt sich hier ein deutlich messbarer Unterschied. Die zugehörigen Tests werden weiterhin als vollständige Vorlage bereitgestellt.

## Einheit 3 – SystemClock, TimerB

Ab dieser Einheit ist die Verwendung von Bibliotheken zulässig – selbst erstellte ebenso wie bereitgestellte. Die Konfiguration erfolgt in jedem Fall datenblattbasiert: unabhängig davon, ob über Register oder über eine Bibliotheksfunktion konfiguriert wird, müssen die verwendeten Werte aus dem Datenblatt begründet werden können. Gemessen wird die tatsächliche gegenüber der konfigurierten Taktfrequenz am Oszilloskop. Die Testverantwortung verschiebt sich auf ein vorgegebenes Gerüst, das um Grenzfälle zu ergänzen ist.

## Einheit 4 – TimerA, PWM

Gleiches Prinzip wie in Einheit 3. Signalform und Duty-Cycle werden am Oszilloskop verifiziert, unabhängig vom gewählten Implementierungsweg. Die Umrechnung von Lenkwinkel zu Duty-Cycle bleibt in jedem Fall eine eigenständige, host-testbare Funktion.

## Einheit 5 – SPI/I2C-Schnittstelle (Messungen)

Auch hier ist die Konfiguration über Register oder Bibliothek möglich. Der Schwerpunkt der Einheit liegt auf der messtechnischen Verifikation: Timing und Protokollverlauf werden am Logic-Analyzer gegen die Spezifikation geprüft, unabhängig von der gewählten Implementierung. Die Tests werden ab dieser Einheit überwiegend eigenständig geschrieben, vorgegeben sind nur noch die Anforderungen.

## Einheit 6 – SPI (Display), I2C (Abstandssensoren)

Aufbauend auf der Schnittstellenkonfiguration aus Einheit 5 entstehen die konkreten Gerätetreiber. Die DL-Tests werden mit einer Fake-HAL vollständig eigenständig erstellt; erstmals kommt eine kleine Anwendung der AL-Schicht hinzu (Statusanzeige).

## Einheit 7 – ADC-Konfiguration, Batteriemessung

Der gemessene Rohwert wird gegen eine Referenzmessung mit dem Multimeter gehalten. Die Umrechnungslogik ist eine eigenständige DL-Funktion und wird vollständig eigenständig getestet.

## Einheit 8 – Direct Memory Access

In dieser Einheit wird in der Praxis überwiegend die Bibliotheksvariante gewählt, da eine vollständige Registerkonfiguration des DMA-Controllers hohen Aufwand bei vergleichsweise geringem zusätzlichem Lernertrag bedeutet. Die dadurch gewonnene Zeit fließt vollständig in die Messung: Der Rechenzeitvergleich zwischen Polling und DMA wird mit derselben Sorgfalt durchgeführt wie in den vorangegangenen Einheiten. Die DL-Tests aus Einheit 7 müssen unverändert bestehen – dies dient als Nachweis, dass sich an der nach außen sichtbaren Schnittstelle nichts geändert hat.

## Einheit 9 – FreeRTOS-Aufsetzen, Scheduling

Zu Beginn der Einheit werden sämtliche offenen Tests aus den Einheiten 1–8 fertiggestellt; die gesamte bisherige Testsuite muss vollständig bestehen, bevor mit neuem Inhalt begonnen wird. Anschließend wird FreeRTOS als Bibliothek eingebunden. Aufgebaut wird die Architektur *Fetch Sensor Data → Process Sensor Data → Make Decisions* als Task-Pipeline: Eine Task ruft die bestehenden DL-Funktionen zur Datenerfassung auf, eine zweite verarbeitet die Werte, eine dritte trifft Entscheidungen – letztere zunächst als Platzhalter, inhaltlich gefüllt in den Einheiten 10 und 11. Gemessen wird das Timing zwischen den Tasks (Latenz, Jitter). Es entsteht eine neue Testkategorie; die bestehenden HAL- und DL-Tests bleiben davon unberührt.

## Einheit 10 – Fahralgorithmus (Zustandsautomat)

Der Zustandsautomat füllt die Decision-Stufe aus Einheit 9 inhaltlich. Die Verifikation erfolgt auf drei Ebenen: Host-Tests wie bisher, eine Überprüfung des Fahrverhaltens im CrazyCar-Simulator unter kontrollierten Bedingungen, sowie abschließend ein Nachweis am realen Fahrzeug, bei dem ein Zustandswechsel gezielt provoziert und dokumentiert wird.

## Einheit 11 – Regelungsprozess (PID)

Eine systematische Variation einzelner Reglerparameter ist am realen Fahrzeug nur eingeschränkt reproduzierbar, da Randbedingungen wie Batteriestand oder Bodenhaftung nicht konstant gehalten werden können. Für diesen Zweck eignet sich der CrazyCar-Simulator besser: Einzelne Parameter lassen sich dort unter gleichbleibenden Bedingungen gezielt variieren. Am realen Fahrzeug erfolgt anschließend die Feinabstimmung sowie der Nachweis, dass die Regelung unter realen Bedingungen funktioniert. Die AL-Tests erreichen in dieser Einheit den größten Umfang im gesamten Kurs.

## Einheit 12–15 – Freies Üben

Systemintegration, individuelle Erweiterung, Optimierung und Vorbereitung der Abschlussdemonstration. Der Inhalt ist frei wählbar, der Nachweis über die weiterhin vollständige Testsuite bleibt verpflichtend.

---

*Konzeptentwurf, Stand Juli 2026*
