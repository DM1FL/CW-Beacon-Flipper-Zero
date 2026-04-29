# Flipper Zero CW Morse Beacon

Dieses Projekt verwandelt den Flipper Zero in eine leistungsstarke CW-Funkbake (Continuous Wave) für das 70cm-Band. Entwickelt für Funkamateure zu Testzwecken (SDR-Kalibrierung, Signalstärkemessung).

## Features
- **Max Power Mode:** Nutzt die volle Sendeleistung des CC1101-Chips durch PA-Table Injection.
- **Frequenzstabil:** Beinhaltet einen PLL-Refresh-Algorithmus, der Frequenzdrift bei langen Sendezyklen verhindert.
- **Dauerträger:** Sendet nach der Kennung einen 30-sekündigen Träger für Signalmessungen.
- **Sicher:** Kann jederzeit über die "Back"-Taste abgebrochen werden.
