#!/usr/bin/env python3
"""
ccMonitor.py  -  Gegenstelle zum Crazy Car mit Oberflaeche

Links laeuft mit, was das Fahrzeug sendet. Rechts schicken Sie Kommandos
hin. Beide Haelften zeigen die Rahmen zusaetzlich in Hex an - damit sehen
Sie Ihr eigenes Protokoll, nicht nur dessen Ergebnis.

    Rahmen vom Auto:   'C' 'S'  v0_hi v0_lo  v1_hi v1_lo ...  'E' 'C'
    Rahmen zum Auto:   'H' 'S'  typ  val_hi val_lo  'E' 'H'

Aufruf:
    python ccMonitor.py                 Einstellungen aus dem Block unten
    python ccMonitor.py COM7            anderer Port
    python ccMonitor.py --list          verfuegbare Ports anzeigen
    python ccMonitor.py --demo          ohne Fahrzeug, erzeugt Testwerte
    python ccMonitor.py --selftest      nur Packen und Auspacken pruefen

Benoetigt:  pip install pyserial matplotlib
"""

# ############################################################################
# Einstellungen - das ist der Teil, den Sie anpassen
# ############################################################################

PORT = "COM7"  # Geraetemanager -> MSP Application UART
BAUD = 57600  # 57600 ueber USB, 9600 ueber Bluetooth

# So viele Werte schickt Ihr driverComSend(). Muss zum count im Fahrzeug passen.
NAMEN = ["Wert 0", "Wert 1", "Wert 2", "Wert 3"]

# 1 = diesen Wert als vorzeichenbehaftet lesen (passend zum Testsatz)
VORZEICHEN = [0, 0, 0, 1]

# Ihre Kommandotabelle: Nummer -> (Name, kleinster Wert, groesster Wert).
# Dieselbe Tabelle steht in Ihrer Hauptschleife im Fahrzeug.
TYPEN = {
    1: ("Lenkwinkel", -100, 100),
    2: ("Gas", -100, 100),
}

SEKUNDEN = 20  # Breite des Zeitfensters im Plot

# ############################################################################
# Ab hier muessen Sie nichts mehr aendern
# ############################################################################

import argparse
import math
import sys
import threading
import time
from collections import deque

CAR_START = b"CS"
CAR_END = b"EC"
HOST_START = b"HS"
HOST_END = b"EH"

ANZAHL = len(NAMEN)


# ##### Protokoll ############################################################

def packeKommando(typ, wert):
    """Baut den Rahmen zum Auto: 'H' 'S' typ hi lo 'E' 'H' (7 Byte)."""
    if not 0 <= typ <= 255:
        raise ValueError("Typ muss zwischen 0 und 255 liegen.")
    muster = wert & 0xFFFF  # negative Werte als Zweierkomplement
    return bytes([HOST_START[0], HOST_START[1], typ,
                  (muster >> 8) & 0xFF, muster & 0xFF,
                  HOST_END[0], HOST_END[1]])


def entpackeWerte(nutzlast, vorzeichen):
    """Setzt je zwei Bytes zu einem Wert zusammen, hoeherwertiges zuerst."""
    werte = []
    for i in range(len(nutzlast) // 2):
        roh = (nutzlast[2 * i] << 8) | nutzlast[2 * i + 1]
        if i < len(vorzeichen) and vorzeichen[i] and roh > 32767:
            roh -= 65536
        werte.append(roh)
    return werte


def alsHex(rohbytes):
    """Rahmen lesbar machen: Kennung | Werte paarweise | Kennung."""
    h = ["%02X" % b for b in rohbytes]
    if len(h) < 4:
        return " ".join(h)
    mitte = h[2:-2]
    paare = [" ".join(mitte[i:i + 2]) for i in range(0, len(mitte), 2)]
    return "%s | %s | %s" % (" ".join(h[:2]), "  ".join(paare), " ".join(h[-2:]))


# ##### Empfang ##############################################################

class Empfaenger(threading.Thread):
    """Liest im Hintergrund Rahmen und legt Werte in die Ringpuffer."""

    def __init__(self, quelle, zeiten, reihen, t0):
        super().__init__(daemon=True)
        self.quelle = quelle
        self.zeiten = zeiten
        self.reihen = reihen
        self.t0 = t0
        self.sperre = threading.Lock()
        self.laeuft = True
        self.rahmen = 0
        self.letzter = b""
        self.letzteZeit = 0.0
        self.fehler = ""

    def lies(self, n):
        return self.quelle.read(n)

    def leseRahmen(self):
        """Synchronisiert auf 'C','S' und liest den Rest des Rahmens."""
        b = self.lies(1)
        if not b:
            return None
        if b[0] != CAR_START[0]:
            return None
        b = self.lies(1)
        if not b:
            return None
        if b[0] != CAR_START[1]:
            return None  # naechster Durchlauf setzt neu auf

        nutzlast = self.lies(2 * ANZAHL)
        ende = self.lies(2)
        if len(nutzlast) < 2 * ANZAHL or len(ende) < 2:
            return None
        if ende != CAR_END:
            return None

        self.letzter = CAR_START + nutzlast + ende
        return entpackeWerte(nutzlast, VORZEICHEN)

    def run(self):
        try:
            while self.laeuft:
                werte = self.leseRahmen()
                if werte is None:
                    continue
                t = time.time() - self.t0
                with self.sperre:
                    self.zeiten.append(t)
                    for i, v in enumerate(werte):
                        self.reihen[i].append(v)
                    self.rahmen += 1
                    self.letzteZeit = t
        except Exception as e:  # Port gezogen, Fahrzeug aus
            self.fehler = str(e)
            self.laeuft = False


class DemoQuelle:
    """Erzeugt Rahmen ohne Fahrzeug, damit die Oberflaeche probiert werden kann."""

    def __init__(self):
        self.puffer = b""
        self.t0 = time.time()

    def _neuerRahmen(self):
        t = time.time() - self.t0
        werte = []
        for i in range(ANZAHL):
            w = int(80 * math.sin(t * (0.6 + 0.3 * i)))
            werte.append(w & 0xFFFF)
        nutzlast = b"".join(bytes([w >> 8, w & 0xFF]) for w in werte)
        return CAR_START + nutzlast + CAR_END

    def read(self, n):
        while len(self.puffer) < n:
            time.sleep(0.1)
            self.puffer += self._neuerRahmen()
        teil, self.puffer = self.puffer[:n], self.puffer[n:]
        return teil

    def write(self, daten):
        return len(daten)


# ##### Oberflaeche ##########################################################

def starteOberflaeche(quelle, portname):
    import tkinter as tk
    from tkinter import ttk
    import matplotlib
    matplotlib.use("TkAgg")
    from matplotlib.figure import Figure
    from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

    maxlen = SEKUNDEN * 50
    zeiten = deque(maxlen=maxlen)
    reihen = [deque(maxlen=maxlen) for _ in range(ANZAHL)]
    t0 = time.time()

    leser = Empfaenger(quelle, zeiten, reihen, t0)
    leser.start()

    wurzel = tk.Tk()
    wurzel.title("Crazy Car  -  %s" % portname)

    links = ttk.Frame(wurzel, padding=8)
    links.grid(row=0, column=0, sticky="nsew")
    rechts = ttk.Frame(wurzel, padding=8)
    rechts.grid(row=0, column=1, sticky="nsew")
    wurzel.columnconfigure(0, weight=3)
    wurzel.columnconfigure(1, weight=1)
    wurzel.rowconfigure(0, weight=1)

    # ----- links: Empfang --------------------------------------------------
    ttk.Label(links, text="Empfangen", font=("TkDefaultFont", 11, "bold")).pack(anchor="w")

    bild = Figure(figsize=(7, 4.2), dpi=100)
    achse = bild.add_subplot(111)
    achse.set_xlabel("Zeit (s)")
    achse.set_ylabel("Wert")
    achse.grid(True, alpha=0.3)
    linien = [achse.plot([], [], ".-", label=NAMEN[i])[0] for i in range(ANZAHL)]
    achse.legend(loc="upper left", fontsize=8)
    bild.tight_layout()

    leinwand = FigureCanvasTkAgg(bild, master=links)
    leinwand.get_tk_widget().pack(fill="both", expand=True)

    ttk.Label(links, text="Letzter Rahmen").pack(anchor="w", pady=(8, 0))
    empfangHex = tk.StringVar(value="-")
    ttk.Label(links, textvariable=empfangHex, font=("TkFixedFont", 10)).pack(anchor="w")

    # ----- rechts: Senden --------------------------------------------------
    ttk.Label(rechts, text="Senden", font=("TkDefaultFont", 11, "bold")).grid(
        row=0, column=0, columnspan=3, sticky="w")

    protokoll = tk.Text(rechts, width=34, height=10, font=("TkFixedFont", 9))

    def sende(typ, feld):
        """feld ist die IntVar des Eingabefeldes - erst hier auslesen,
           damit eine leere oder unsinnige Eingabe nur eine Meldung erzeugt."""
        try:
            nummer = typ.get() if hasattr(typ, "get") else int(typ)
            wert = feld.get() if hasattr(feld, "get") else int(feld)
            rahmen = packeKommando(int(nummer), int(wert))
        except Exception:
            protokoll.insert("end", "ungueltige Eingabe\n")
            protokoll.see("end")
            return
        try:
            quelle.write(rahmen)
        except Exception as e:
            protokoll.insert("end", "Fehler: %s\n" % e)
            protokoll.see("end")
            return
        protokoll.insert("end", "%s   Typ %d = %d\n" % (alsHex(rahmen), int(nummer), int(wert)))
        protokoll.see("end")

    zeile = 1
    for typ in sorted(TYPEN):
        name, kleinster, groesster = TYPEN[typ]
        ttk.Label(rechts, text="%d  %s" % (typ, name)).grid(
            row=zeile, column=0, columnspan=3, sticky="w", pady=(8, 0))
        zeile += 1

        wert = tk.IntVar(value=0)
        regler = ttk.Scale(rechts, from_=kleinster, to=groesster, orient="horizontal",
                           command=lambda v, w=wert: w.set(int(float(v))))
        regler.grid(row=zeile, column=0, sticky="ew")
        feld = ttk.Entry(rechts, textvariable=wert, width=7)
        feld.grid(row=zeile, column=1, padx=4)
        ttk.Button(rechts, text="Setzen",
                   command=lambda t=typ, w=wert: sende(t, w)).grid(row=zeile, column=2)
        zeile += 1

    ttk.Separator(rechts, orient="horizontal").grid(
        row=zeile, column=0, columnspan=3, sticky="ew", pady=10)
    zeile += 1

    ttk.Label(rechts, text="frei: Typ und Wert").grid(
        row=zeile, column=0, columnspan=3, sticky="w")
    zeile += 1

    freiTyp = tk.IntVar(value=0)
    freiWert = tk.IntVar(value=0)
    ttk.Spinbox(rechts, from_=0, to=255, textvariable=freiTyp, width=5).grid(
        row=zeile, column=0, sticky="w")
    ttk.Entry(rechts, textvariable=freiWert, width=7).grid(row=zeile, column=1, padx=4)
    ttk.Button(rechts, text="Senden",
               command=lambda: sende(freiTyp, freiWert)).grid(row=zeile, column=2)
    zeile += 1

    ttk.Label(rechts, text="Gesendete Rahmen").grid(
        row=zeile, column=0, columnspan=3, sticky="w", pady=(10, 0))
    zeile += 1
    protokoll.grid(row=zeile, column=0, columnspan=3, sticky="nsew")
    rechts.rowconfigure(zeile, weight=1)
    rechts.columnconfigure(0, weight=1)
    zeile += 1

    zustand = tk.StringVar(value="warte auf Rahmen")
    ttk.Label(rechts, textvariable=zustand).grid(
        row=zeile, column=0, columnspan=3, sticky="w", pady=(8, 0))

    # ----- Auffrischen -----------------------------------------------------
    def auffrischen():
        with leser.sperre:
            t = list(zeiten)
            daten = [list(r) for r in reihen]
            anzahl = leser.rahmen
            letzter = leser.letzter
            letzteZeit = leser.letzteZeit
            fehler = leser.fehler

        if t:
            for i, linie in enumerate(linien):
                linie.set_data(t, daten[i])
            achse.relim()
            achse.autoscale_view(scalex=False)
            ende = t[-1]
            achse.set_xlim(max(0.0, ende - SEKUNDEN), max(ende, SEKUNDEN))
            leinwand.draw_idle()
            empfangHex.set(alsHex(letzter))

        if fehler:
            zustand.set("Port: %s" % fehler)
        elif anzahl == 0:
            zustand.set("warte auf Rahmen")
        else:
            still = time.time() - t0 - letzteZeit
            if still > 1.0:
                zustand.set("%d Rahmen, seit %.1f s nichts mehr" % (anzahl, still))
            else:
                zustand.set("%d Rahmen empfangen" % anzahl)

        wurzel.after(100, auffrischen)

    wurzel.after(100, auffrischen)

    def schliessen():
        leser.laeuft = False
        wurzel.destroy()

    wurzel.protocol("WM_DELETE_WINDOW", schliessen)
    wurzel.mainloop()


# ##### Start ################################################################

def zeigePorts():
    try:
        from serial.tools import list_ports
    except ImportError:
        sys.exit("pyserial fehlt.  Installieren mit:  pip install pyserial")
    ports = list(list_ports.comports())
    if not ports:
        print("Keine seriellen Ports gefunden.")
        return
    for p in ports:
        print("  %-10s %s" % (p.device, p.description))
    print("\nGesucht ist ueblicherweise 'MSP Application UART'.")


def selbsttest():
    fehler = 0

    erwartet = bytes([0x48, 0x53, 0x01, 0x00, 0x32, 0x45, 0x48])
    if packeKommando(1, 50) != erwartet:
        print("FEHLER: packeKommando(1, 50)"); fehler += 1

    if packeKommando(1, -40) != bytes([0x48, 0x53, 0x01, 0xFF, 0xD8, 0x45, 0x48]):
        print("FEHLER: negativer Wert"); fehler += 1

    satz = bytes([0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xD8])
    if entpackeWerte(satz, [0, 0, 0, 1]) != [0, 4095, 65535, -40]:
        print("FEHLER: Testsatz"); fehler += 1

    if entpackeWerte(satz, [0, 0, 0, 0])[3] != 65496:
        print("FEHLER: ohne Vorzeichen"); fehler += 1

    if entpackeWerte(bytes([0xFF, 0x0F]), [0]) != [65295]:
        print("FEHLER: vertauschte Bytes ergeben nicht 65295"); fehler += 1

    print("Selbsttest: %s" % ("alles in Ordnung" if fehler == 0 else "%d Fehler" % fehler))
    return fehler


def main():
    p = argparse.ArgumentParser(description="Gegenstelle zum Crazy Car")
    p.add_argument("port", nargs="?", default=PORT)
    p.add_argument("--baud", type=int, default=BAUD)
    p.add_argument("--list", action="store_true", help="Ports auflisten")
    p.add_argument("--demo", action="store_true", help="ohne Fahrzeug")
    p.add_argument("--selftest", action="store_true", help="nur Protokoll pruefen")
    args = p.parse_args()

    if args.selftest:
        sys.exit(selbsttest())

    if len(VORZEICHEN) < ANZAHL:
        sys.exit("VORZEICHEN braucht genauso viele Eintraege wie NAMEN.")

    if args.list:
        zeigePorts()
        return

    if args.demo:
        starteOberflaeche(DemoQuelle(), "Demo")
        return

    try:
        import serial
    except ImportError:
        sys.exit("pyserial fehlt.  Installieren mit:  pip install pyserial")

    try:
        with serial.Serial(args.port, args.baud, timeout=0.5) as ser:
            starteOberflaeche(ser, "%s @ %d Baud" % (args.port, args.baud))
    except serial.SerialException as e:
        sys.exit("Port laesst sich nicht oeffnen: %s\nLaeuft noch HTerm oder MATLAB?" % e)


if __name__ == "__main__":
    main()
