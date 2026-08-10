"""
============================================================================
 strom.py — Akku messen, IR-Licht schalten, Notabschaltung
============================================================================
 Drei Aufgaben:
   1. Akkuspannung UND Ladestrom messen (INA219 über I2C)
   2. Die IR-LEDs an-/ausschalten und dimmen (PWM auf GPIO18)
   3. Den Pi sauber herunterfahren, wenn der Akku leer wird

 Zu 3: Ein Pi, der sich herunterfährt, startet nicht von allein wieder.
 Das übernimmt der Laderegler: Er trennt die Last bei ~10,5 V und schaltet
 sie wieder ein, wenn die Sonne den Akku auf ~12,5 V gebracht hat.
 Dann bootet der Pi von selbst. Siehe Schaltplan 3.4.
============================================================================
"""

import logging
import subprocess
import threading
import time

from . import einstellungen as cfg

log = logging.getLogger(__name__)

# ---------------------------------------------------------------------------
#  INA219 — Spannungs- und Strommessung über I2C
# ---------------------------------------------------------------------------
#  Wir sprechen den Chip direkt an, statt eine große Bibliothek zu
#  installieren. Das sind nur vier Register — gut zum Nachlesen, wie so
#  ein Sensor eigentlich funktioniert.
# ---------------------------------------------------------------------------

_INA219_ADRESSE = 0x40
_REG_CONFIG = 0x00
_REG_BUS_SPANNUNG = 0x02
_REG_STROM = 0x04
_REG_KALIBRIERUNG = 0x05

# Werkseinstellung des Chips: 32 V Messbereich, Verstärkung /8, 12 Bit,
# fortlaufende Messung von Spannung und Strom.
_CONFIG_WERT = 0x399F


class INA219:
    def __init__(self, bus_nummer: int = 1, adresse: int = _INA219_ADRESSE):
        from smbus2 import SMBus                      # erst hier importieren
        self._bus = SMBus(bus_nummer)
        self._adr = adresse

        # Kalibrierung: bei 0,1 Ohm Messwiderstand und 3,2 A Maximalstrom
        # ist ein Zählschritt 100 Mikroampere gross.
        self._strom_lsb = 0.0001
        kalibrierung = int(0.04096 / (self._strom_lsb * cfg.INA219_SHUNT_OHM))

        self._schreiben(_REG_KALIBRIERUNG, kalibrierung)
        self._schreiben(_REG_CONFIG, _CONFIG_WERT)

    def _schreiben(self, register: int, wert: int) -> None:
        # Der Chip erwartet die zwei Bytes "verkehrt herum" (MSB zuerst)
        self._bus.write_i2c_block_data(
            self._adr, register, [(wert >> 8) & 0xFF, wert & 0xFF])

    def _lesen(self, register: int) -> int:
        hoch, niedrig = self._bus.read_i2c_block_data(self._adr, register, 2)
        return (hoch << 8) | niedrig

    def volt(self) -> float:
        """Akkuspannung in Volt."""
        roh = self._lesen(_REG_BUS_SPANNUNG)
        # Die unteren 3 Bit sind Statusbits, deshalb erst schieben.
        # Ein Zählschritt entspricht 4 Millivolt.
        return (roh >> 3) * 0.004

    def ampere(self) -> float:
        """Strom in Ampere. POSITIV = Akku wird geladen,
        NEGATIV = Akku wird entladen."""
        roh = self._lesen(_REG_STROM)
        if roh > 32767:                # negative Zahlen im Zweierkomplement
            roh -= 65536
        return roh * self._strom_lsb


# ---------------------------------------------------------------------------
#  Zustand
# ---------------------------------------------------------------------------
_ina: INA219 | None = None
_ir_pwm = None
_ir_stand = 0.0

_letzte_volt = 0.0
_letzte_ampere = 0.0
_zu_niedrig_zaehler = 0
_notaus_aktiv = False


def start() -> None:
    """Einmal beim Programmstart aufrufen."""
    global _ina, _ir_pwm

    # --- IR-LEDs ---
    try:
        from gpiozero import PWMOutputDevice
        # 1000 Hz: schnell genug, dass die Kamera kein Flackern sieht.
        # Falls im Nachtbild Streifen auftauchen: Frequenz erhöhen oder
        # IR_HELLIGKEIT auf 1.0 stellen (dann ist PWM ganz aus).
        _ir_pwm = PWMOutputDevice(cfg.PIN_IR_LED, frequency=1000, initial_value=0)
        log.info("IR-LEDs bereit an GPIO%d", cfg.PIN_IR_LED)
    except Exception as e:
        log.error("IR-LEDs nicht verfuegbar: %s", e)

    # --- Akkumessung ---
    if cfg.AKKU_MESSEN:
        try:
            _ina = INA219()
            log.info("INA219 bereit: %.2f V", _ina.volt())
        except Exception as e:
            log.error("INA219 nicht gefunden: %s", e)
            log.error("Pruefe: I2C aktiviert? `sudo raspi-config` -> Interfaces")
            log.error("Pruefe: `i2cdetect -y 1` zeigt 40?")
            _ina = None


# ---------------------------------------------------------------------------
#  IR-Licht
# ---------------------------------------------------------------------------
def ir_licht(helligkeit: float) -> None:
    """helligkeit von 0.0 (aus) bis 1.0 (voll)."""
    global _ir_stand
    helligkeit = max(0.0, min(1.0, helligkeit))
    _ir_stand = helligkeit
    if _ir_pwm is not None:
        _ir_pwm.value = helligkeit


def ir_an() -> None:
    ir_licht(cfg.IR_HELLIGKEIT)


def ir_aus() -> None:
    ir_licht(0.0)


def ir_ist_an() -> bool:
    return _ir_stand > 0.0


# ---------------------------------------------------------------------------
#  Messwerte
# ---------------------------------------------------------------------------
def messen() -> tuple[float, float]:
    """(Volt, Ampere) frisch messen. (0, 0) wenn kein Sensor da ist."""
    global _letzte_volt, _letzte_ampere
    if _ina is None:
        return 0.0, 0.0
    try:
        _letzte_volt = _ina.volt()
        _letzte_ampere = _ina.ampere()
    except OSError as e:
        log.warning("INA219 antwortet nicht: %s", e)
    return _letzte_volt, _letzte_ampere


def letzte_werte() -> tuple[float, float]:
    return _letzte_volt, _letzte_ampere


def prozent() -> int:
    """Ladezustand grob geschätzt. Für LiFePO4 mit 4 Zellen (12,8 V nominal).

    Achtung: Die Spannungskurve von LiFePO4 ist sehr flach — zwischen
    20 % und 80 % Ladung liegen nur ~0,4 V. Die Prozentzahl ist deshalb
    eine ungenaue Schätzung. Der LADESTROM ist die aussagekräftigere Zahl.
    """
    v = _letzte_volt
    if v < 1.0:
        return 0
    kurve = [(10.0, 0), (12.0, 10), (12.8, 20), (13.0, 50),
             (13.2, 75), (13.4, 90), (13.6, 100)]
    if v <= kurve[0][0]:
        return 0
    if v >= kurve[-1][0]:
        return 100
    for (v1, p1), (v2, p2) in zip(kurve, kurve[1:]):
        if v < v2:
            return int(p1 + (v - v1) / (v2 - v1) * (p2 - p1))
    return 100


def laedt() -> bool:
    """Kommt gerade Strom vom Panel?"""
    return _letzte_ampere > 0.02


# ---------------------------------------------------------------------------
#  Notabschaltung
# ---------------------------------------------------------------------------
def notaus_pruefen() -> bool:
    """True, wenn der Pi heruntergefahren wird."""
    global _zu_niedrig_zaehler, _notaus_aktiv

    if _ina is None or _notaus_aktiv:
        return False

    volt, _ = messen()
    if volt < 1.0:                     # kein plausibler Messwert
        return False

    if volt < cfg.AKKU_NOTAUS_VOLT:
        _zu_niedrig_zaehler += 1
        log.warning("Akku niedrig: %.2f V (%d/%d)",
                    volt, _zu_niedrig_zaehler, cfg.NOTAUS_MESSUNGEN)
    else:
        _zu_niedrig_zaehler = 0
        return False

    # Erst nach mehreren Messungen hintereinander — ein kurzer
    # Spannungseinbruch beim SSD-Schreiben soll nicht ausreichen.
    if _zu_niedrig_zaehler < cfg.NOTAUS_MESSUNGEN:
        return False

    _notaus_aktiv = True
    log.critical("NOTAUS: Akku bei %.2f V. Fahre herunter.", volt)
    log.critical("Der Laderegler schaltet die Last wieder ein, sobald")
    log.critical("die Sonne den Akku geladen hat. Dann bootet der Pi selbst.")

    ir_aus()
    try:
        subprocess.run(["/sbin/shutdown", "-h", "now"], check=False, timeout=10)
    except Exception as e:
        log.error("Herunterfahren fehlgeschlagen: %s", e)
    return True


# ---------------------------------------------------------------------------
#  Hintergrund-Aufgabe: messen, speichern, Notaus prüfen
# ---------------------------------------------------------------------------
def telemetrie_schleife(ist_nacht_funktion) -> None:
    """Läuft in einem eigenen Thread, für immer."""
    from . import statistik

    letztes_speichern = 0.0
    while True:
        try:
            volt, ampere = messen()

            if notaus_pruefen():
                return                      # Pi fährt herunter

            jetzt = time.time()
            if jetzt - letztes_speichern >= cfg.TELEMETRIE_SPEICHERN_S:
                letztes_speichern = jetzt
                if volt > 1.0:
                    statistik.telemetrie_eintragen(volt, ampere, ist_nacht_funktion())
        except Exception as e:
            log.error("Telemetrie-Fehler: %s", e)

        time.sleep(cfg.TELEMETRIE_INTERVALL_S)


def telemetrie_starten(ist_nacht_funktion) -> None:
    t = threading.Thread(target=telemetrie_schleife, args=(ist_nacht_funktion,),
                         name="telemetrie", daemon=True)
    t.start()


def aufraeumen() -> None:
    ir_aus()
    if _ir_pwm is not None:
        try:
            _ir_pwm.close()
        except Exception:
            pass
