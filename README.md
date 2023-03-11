# Viessmann-ESP8266-with-1-Wire-and-MQTT
vcontrold Client with Support for 1-Wire and MQTT for your ESP8266

Project in die ArduinoIDE oder PlatformIO importieren, Daten von deinem MQTT Server und WLAN eintragen. Bitte auch die Statische IP Adressen an dein Netz anpassen.

Das ganz auf einen ESP8266 (z.b. D1-Mini) hochladen.

Den Bauplan für die Hardware erhält man hier https://github.com/openv/openv/wiki/Bauanleitung-ESP8266 

Der Data Pin für 1-Wire hängt bei mir an GPIO2, kann aber auch im Projekt jederzeit über ``#define ONEWIRE_PIN`` geändert werden.

Ich habe in das Projekt noch ein webserial eingebaut, damit man jederzeit über http://IP/webserial die Ereignisse sehen kann. Insbesondere für 1-Wire sicherlich interessant.

OTA ist auch implementiert. Sobald der Sketch auf dem ESP ist, kann über dein Netzwerk das Projekt aktualisiert werden. 

Bei mir läuft das System nun bereits seit über 4 Jahren absolut zuverlässig.
