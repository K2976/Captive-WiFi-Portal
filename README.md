# Captive-WiFi-Portal

Capitive Wi-fi portal is currently available only for "KIET" wifi network.
----------------------------------------------------------------------------------------------
WHAT DO YOU NEED TO RUN THIS?
----------------------------------------------------------------------------------------------

Arduino IDE & ESP32 Wroom module

----------------------------------------------------------------------------------------------
HOW DOES IT WORK?
----------------------------------------------------------------------------------------------

it uses SPIFFS library of arduino by which we can host a HTML site on esp32.
the HTML site is exact replica of a the kiet wifi login page.

----------------------------------------------------------------------------------------------
HOW IT CAPTURES?
----------------------------------------------------------------------------------------------

when u run the code a similar wifi hotspot is created as the same SSID of KIET (i.e. KIET).
so when a person connects to esp hotspot it redirects to fake login page and when the person enterhis/her creds on the page we can see the output on serial mointor.
