# Viessmann-ESP8266-with-1-Wire-and-MQTT
vcontrold Client with Support for 1-Wire and MQTT for your ESP8266

Project in die ArduinoIDE oder PlatformIO importieren, Daten von deinem MQTT Server und WLAN eintragen. Bitte auch die Statische IP Adressen an dein Netz anpassen.

Das ganz auf einen ESP8266 (z.b. D1-Mini) hochladen.

Den Bauplan für die Hardware erhält man hier https://github.com/openv/openv/wiki/Bauanleitung-ESP8266 Habe bei mir den ESP und die Platine für den "Lesekopf" getrennt gehalten und per Kabel verbunden. Bei mir geht das eher in diese Richtung:

![71757728-5558a180-2e98-11ea-9e9d-23817dd05f42](https://user-images.githubusercontent.com/13675579/224473982-29e7c7ef-4ba3-4fdb-9cca-2f0bed8322e4.jpg)
(Quelle: https://github.com/openv/openv/wiki/Weiterentwicklung-3.3V-TTL-mit-3D-Geh%C3%A4use)


Der Data Pin für 1-Wire hängt bei mir an GPIO2, kann aber auch im Projekt jederzeit über ``#define ONEWIRE_PIN`` geändert werden.

Ich habe in das Projekt noch ein webserial eingebaut, damit man jederzeit über http://IP/webserial die Ereignisse sehen kann. Insbesondere für 1-Wire sicherlich interessant.

OTA ist auch implementiert. Sobald der Sketch auf dem ESP ist, kann über dein Netzwerk das Projekt aktualisiert werden. 

Bei mir läuft das System nun bereits seit über 4 Jahren absolut zuverlässig.

*Home-Assistant*

Zum Auslesen der Heizung nutze ich hier das vcontrold Add-On: https://github.com/Alexandre-io/homeassistant-vcontrol

Bei TTY einfach die IP:8888 eintragen (<IP>:<PORT>). Port ist im Sketch mit 8888 hinterlegt, kann dort aber auch jederzeit über die Zeile ``int wifi_port`` angepasst werden.
  
Alle weiteren Einstellungen im vcontrold müsst Ihr passend zu eurer Heizung einstellen.
  
Hier noch eine Beispiel yaml von mir: https://github.com/Alexandre-io/homeassistant-vcontrol/issues/7 Die muss nicht klappen, kann aber (je nachdem welches Heizungsmodell Ihr habt).
  
Bitte beachten, dass Ihr sowohl für das vcontrold Add-On, als auch für 1-Wire einen MQTT Server aufsetzen müsst. Ich nutze hier https://github.com/home-assistant/addons/tree/master/mosquitto
