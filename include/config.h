#ifndef CONFIG_H
#define CONFIG_H

// Platform detection
#if defined(ARDUINO_M5STICK_C_PLUS2)
  #define PLATFORM_M5STICKC_PLUS2
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
  #define PLATFORM_ESP32_S3
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
  #define PLATFORM_ESP32_C3
#else
  #define PLATFORM_ESP32_DEV
#endif

// Display settings
#ifdef PLATFORM_M5STICKC_PLUS2
  #define SCREEN_WIDTH 240
  #define SCREEN_HEIGHT 135
#else
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64
#endif

// Button pins
#ifdef PLATFORM_M5STICKC_PLUS2
  #define BTN_NEXT_PIN  39
  #define BTN_OK_PIN    37
#elif defined(PLATFORM_ESP32_C3)
  #define BTN_NEXT_PIN  8
  #define BTN_OK_PIN    9
#else
  #define BTN_NEXT_PIN  32
  #define BTN_OK_PIN    33
#endif

// RFID pins
#ifdef PLATFORM_M5STICKC_PLUS2
  #define RFID_SS_PIN    5
  #define RFID_RST_PIN   26
#elif defined(PLATFORM_ESP32_C3)
  #define RFID_SS_PIN    7
  #define RFID_RST_PIN   10
#else
  #define RFID_SS_PIN    5
  #define RFID_RST_PIN   27
#endif

#define RFID_SCK_PIN   18
#define RFID_MISO_PIN  19
#define RFID_MOSI_PIN  23

// IR pins
#ifdef PLATFORM_M5STICKC_PLUS2
  #define IR_SEND_PIN   19
  #define IR_RECV_PIN   36
#elif defined(PLATFORM_ESP32_C3)
  #define IR_RECV_PIN   2
  #define IR_SEND_PIN   3
#else
  #define IR_RECV_PIN   15
  #define IR_SEND_PIN   13
#endif

// Colors for TFT
#ifdef PLATFORM_M5STICKC_PLUS2
  #define COLOR_BG       0x0000
  #define COLOR_PRIMARY  0xFD20
  #define COLOR_TEXT     0xFFFF
#endif

#endif
