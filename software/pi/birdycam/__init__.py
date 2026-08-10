"""BirdyCam — Nistkasten-Kamera für den Raspberry Pi Zero 2 W.

Module:
    einstellungen   Alle Einstellungen. Die einzige Datei, die man anfasst.
    kamera          Bild, Bewegungserkennung, Clips, Livestream, Tag/Nacht
    lichtschranke   Vögel zählen (IR-Strahl im Einflugloch)
    strom           Akku messen (INA219), IR-LEDs, Notabschaltung
    ringspeicher    Round-Robin-Dateien, damit die Platte nie volläuft
    statistik       SQLite-Datenbank mit Besuchen, Clips, Telemetrie
    web             Flask-Website und Livestream
    main            Startet alles in der richtigen Reihenfolge
"""

__version__ = "1.0.0"
