#ifndef __PARAMETER_H_
#define __PARAMETER_H_

//#######################################
// ESP Parameter

#define ESP32
/* Einstellungen Arduiono IDE
 * Board:            "ESP32 Dev Modul"
 * CPU Frequency:    "80 Mhz"
 * Flash Mode:       "DIO"
 * Flash Size:       "4MB (32Mb)"
 * Partition Scheme: "Minimal SPIFFS (1,9MB App with OTA/190KB SPIFFS)"
 */
#define SERIALSPEED                 115200
//#define DEBUG                                             // Aktiv DEBUG
//#######################################
//#######################################
// W-LAN Parameter
#define STASSID1                   "W-LAN SSID 1"            // W-LAN SSID
#define STAPSK1                    "W-LAN Schlüssel 1"       // W-LAN Schlüssel
#define STASSID2                   "W-LAN SSID 2"            // W-LAN SSID
#define STAPSK2                    "W-LAN Schlüssel 2"       // W-LAN Schlüssel
#define STASSID3                   "W-LAN SSID 3"            // W-LAN SSID
#define STAPSK3                    "W-LAN Schlüssel 3"       // W-LAN Schlüssel

#define WIFI_TIMEOUT                 6                       // Sekunden


//#######################################
// NTP & Time Parameter
#define NTP_SERVER_1               "time.google.com"        // alternativ: "fritz.box"
#define NTP_SERVER_2               "pool.ntp.org"
#define TIMEZONE                    1
#define DST                         0

//#######################################
// Modbus Parameter

#define IP_E3DC                    "192.168.178.24"        // IP-Adresse vom E3DC
#define INTERVALL_MODBUS            1                       // Abfrageintervall in Sekunden,
#define MODBUS_TIMEOUT              4                       // Sekunden
#define MB_RESET                    10                      // Modbus-Intervalle bis Reset beim Fehler
#define MB_REBOOT                   5                       // Zähler Reset bis Reboot beim Fehler (nicht auf 0 setzen)
//#define EXT_LM_USE                                          // Aktiv = externer Leistungsmesser ist verbaut
//#define EXT_WB_USE                                          // Aktiv = Wallbox ist verbaut

#define REG_OFFSET                  -1
#define REG_MAGIC                   40001                   // E3DC Modbus Register
#define REG_SOLAR                   40068                   // E3DC Modbus Register
#define REG_BAT                     40070                   // E3DC Modbus Register
#define REG_CON                     40072                   // E3DC Modbus Register
#define REG_GRID                    40074                   // E3DC Modbus Register
#define REG_AUTARKIE                40082                   // E3DC Modbus Register
#define REG_BATSOC                  40083                   // E3DC Modbus Register
#define REG_PV_U1                   40096                   // E3DC Modbus Register
#define REG_PV_U2                   40097                   // E3DC Modbus Register
#define REG_PV_I1                   40099                   // E3DC Modbus Register
#define REG_PV_I2                   40100                   // E3DC Modbus Register
#define REG_PV_P1                   40102                   // E3DC Modbus Register
#define REG_PV_P2                   40103                   // E3DC Modbus Register
#define REG_EXT                     40076                   // E3DC Modbus Register
#define REG_WB_ALL                  40078                   // E3DC Modbus Register
#define REG_WB_SOLAR                40080                   // E3DC Modbus Register
#define REG_WB_CTRL                 40088                   // E3DC Modbus Register
#define REG_SGREADY                 40137                   // E3DC Modbus Register

#define WB_USE                      0x0001                  // Bit  0 Wallbox vorhanden und verfügbar (1)
#define WB_MODE                     0x0002                  // Bit  1 Solarbetrieb aktiv (1) / Mischbetrieb aktiv (0)
#define WB_ERR                      0x0004                  // Bit  2 Laden abgebrochen (1) / Laden freigegeben (0)
#define WB_CHARGE                   0x0008                  // Bit  3 Auto lädt (1) / Auto lädt nicht (0)
#define WB_LOCKED                   0x0010                  // Bit  4 Typ-2-Stecker verriegelt (1)
#define WB_CONNECT                  0x0020                  // Bit  5 Typ-2-Stecker gesteckt (1)
#define WB_SCHUKO_ON                0x0040                  // Bit  6 Schukosteckdose an (1)
#define WB_SCHUKO_CONNECT           0x0080                  // Bit  7 Schukostecker gesteckt (1)
#define WB_SCHUKO_LOCKED            0x0100                  // Bit  8 Schukostecker verriegelt (1)
#define WB_FREE_1                   0x0200                  // Bit  9 Frei
#define WB_FREE_2                   0x0400                  // Bit 10 Frei
#define WB_FREE_3                   0x0800                  // Bit 11 Frei
#define WB_PHASES                   0x1000                  // Bit 12 Eine Phase aktiv (1) /  drei Phasen aktiv (0)
#define WB_FREE_4                   0x2000                  // Bit 13 Frei

//#######################################
// Homematic Parameter
//#define HM_USE                                              // Aktiv = Daten werden an die ISE_ID der Homematic-Systemvariablen gesendet
#define IP_HM                      "192.168.178.yyy"        // IP-Adresse von der Homematic
#define INTERVALL_HM                30                      // Abfrageintervall, keinste Empfehlung 15 Sekunden

#define ISE_SEND_AKTIV              0                       // ISE_ID für Status, ein Program anlegen welches nach Auslösen diser Sytemvariable z.B. nach 3 Minuten die Variable zurücksetzt
#define ISE_SEND_INFO               0                       // ISE_ID Startzeitpunkt
#define ISE_SEND_WIFI               0                       // ISE_ID WiFi Empfangsstärke

#define ISE_S10_SOLAR               0                       // ISE_ID aus der HM
#define ISE_S10_BAT                 0                       // ISE_ID aus der HM
#define ISE_S10_CON                 0                       // ISE_ID aus der HM
#define ISE_S10_GRID                0                       // ISE_ID aus der HM
#define ISE_S10_AUTARKIE            0                       // ISE_ID aus der HM
#define ISE_S10_EIGEN               0                       // ISE_ID aus der HM
#define ISE_S10_BATSOC              0                       // ISE_ID aus der HM

//#######################################


#endif // __PARAMETER_H_
