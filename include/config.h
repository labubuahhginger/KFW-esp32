#ifndef CONFIG_H
#define CONFIG_H

// ===== PLATFORM DETECTION =====
#if defined(ARDUINO_M5STICK_C_PLUS2)
  #define PLATFORM_M5STICKC_PLUS2
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
  #define PLATFORM_ESP32_S3
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
  #define PLATFORM_ESP32_C3
#elif defined(ARDUINO_ESP32_WROVER)
  #define PLATFORM_ESP32_WROVER
#else
  #define PLATFORM_ESP32_DEV
#endif

// ===== DISPLAY SETTINGS =====
#ifdef PLATFORM_M5STICKC_PLUS2
  #define SCREEN_WIDTH 240
  #define SCREEN_HEIGHT 135
  #define DISPLAY_ROTATION 1
#else
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64
  #define DISPLAY_ROTATION 0
#endif

// ===== BUTTON PINS =====
#ifdef PLATFORM_M5STICKC_PLUS2
  #define BTN_NEXT_PIN  39
  #define BTN_OK_PIN    37
  #define BTN_PWR_PIN   35
#elif defined(PLATFORM_ESP32_C3)
  #define BTN_NEXT_PIN  8
  #define BTN_OK_PIN    9
#else
  #define BTN_NEXT_PIN  32
  #define BTN_OK_PIN    33
#endif

// ===== RFID PINS =====
#ifdef PLATFORM_M5STICKC_PLUS2
  #define RFID_SS_PIN    5
  #define RFID_RST_PIN   26
  #define RFID_SCK_PIN   18
  #define RFID_MISO_PIN  19
  #define RFID_MOSI_PIN  23
#elif defined(PLATFORM_ESP32_C3)
  #define RFID_SS_PIN    7
  #define RFID_RST_PIN   10
  #define RFID_SCK_PIN   4
  #define RFID_MISO_PIN  5
  #define RFID_MOSI_PIN  6
#else
  #define RFID_SS_PIN    5
  #define RFID_RST_PIN   27
  #define RFID_SCK_PIN   18
  #define RFID_MISO_PIN  19
  #define RFID_MOSI_PIN  23
#endif

// ===== IR PINS =====
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

// ===== COLORS =====
#ifdef PLATFORM_M5STICKC_PLUS2
  #define COLOR_BG       0x0000
  #define COLOR_PRIMARY  0xFD20
  #define COLOR_SUCCESS  0x07E0
  #define COLOR_DANGER   0xF800
  #define COLOR_TEXT     0xFFFF
  #define COLOR_GRAY     0x8410
#else
  #define COLOR_BG       0
  #define COLOR_PRIMARY  1
  #define COLOR_SUCCESS  1
  #define COLOR_DANGER   1
  #define COLOR_TEXT     1
  #define COLOR_GRAY     1
#endif

#endif
