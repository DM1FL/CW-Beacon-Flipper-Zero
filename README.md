# Flipper Zero CW Morse Beacon

Dieses Projekt verwandelt den Flipper Zero in eine leistungsstarke CW-Funkbake (Continuous Wave) für das 70cm-Band. Entwickelt für Funkamateure zu Testzwecken (SDR-Kalibrierung, Signalstärkemessung).

## Features
- **Max Power Mode:** Nutzt die volle Sendeleistung des CC1101-Chips durch PA-Table Injection.
- **Frequenzstabil:** Beinhaltet einen PLL-Refresh-Algorithmus, der Frequenzdrift bei langen Sendezyklen verhindert.
- **Dauerträger:** Sendet nach der Kennung einen 30-sekündigen Träger für Signalmessungen.
- **Sicher:** Kann jederzeit über die "Back"-Taste abgebrochen werden.

## Installation

1. Installiere das [uFBT SDK](https://github.com/flipperdevices/flipperzero-ufbt).
2. Kopiere die Dateien `app.c` und `application.fam` in einen Ordner.
3. Öffne ein Terminal in diesem Ordner und führe aus:
   ufbt launch

## Konfiguration
Öffne die app.c und passe die folgenden Zeilen an dein Rufzeichen und deine Wunschfrequenz an:

TARGET_FREQ: Deine Sendefrequenz.

CORRECTION: Falls dein Flipper (wie in Tests festgestellt) einen Offset hat, kannst du ihn hier ausgleichen.

DOT_MS: Ändere die Morse-Geschwindigkeit (150 = langsam, 60 = schnell).

## Rechtlicher Hinweis
Achtung: Das Senden auf Amateurfunkfrequenzen erfordert eine entsprechende Lizenz. Der Betreiber ist für die Einhaltung der örtlichen Gesetze verantwortlich. Die Nutzung erfolgt auf eigene Gefahr.

73 de DM1FL
