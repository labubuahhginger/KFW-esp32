#ifndef CONFIG_H
#define CONFIG_H

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DISPLAY_ROTATION 0

// ===== Определение чипа =====
#if defined(CONFIG_IDF_TARGET_ESP32S3)
  #define CHIP_ESP32S3
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
  #define CHIP_ESP32C3
#else
  #define CHIP_ESP32
#endif

// ===== Кнопки =====
#ifdef CHIP_ESP32S3
  #define BTN_NEXT_PIN  4
  #define BTN_OK_PIN    5
#elif defined(CHIP_ESP32C3)
  #define BTN_NEXT_PIN  4
  #define BTN_OK_PIN    5
#else
  #define BTN_NEXT_PIN  32
  #define BTN_OK_PIN    33
#endif

// ===== RFID (MFRC522 SPI) =====
#ifdef CHIP_ESP32S3
  #define RFID_SS_PIN    10
  #define RFID_RST_PIN   11
  #define RFID_SCK_PIN   12
  #define RFID_MISO_PIN  13
  #define RFID_MOSI_PIN  14
#elif defined(CHIP_ESP32C3)
  #define RFID_SS_PIN    7
  #define RFID_RST_PIN   6
  #define RFID_SCK_PIN   4
  #define RFID_MISO_PIN  5
  #define RFID_MOSI_PIN  3
#else
  #define RFID_SS_PIN    5
  #define RFID_RST_PIN   27
  #define RFID_SCK_PIN   18
  #define RFID_MISO_PIN  19
  #define RFID_MOSI_PIN  23
#endif

// ===== ИК-порт =====
#ifdef CHIP_ESP32S3
  #define IR_RECV_PIN   16
  #define IR_SEND_PIN   17
#elif defined(CHIP_ESP32C3)
  #define IR_RECV_PIN   6
  #define IR_SEND_PIN   7
#else
  #define IR_RECV_PIN   15
  #define IR_SEND_PIN   13
#endif

// ===== OLED Display (I2C) =====
#ifdef CHIP_ESP32S3
  #define OLED_SDA_PIN  1
  #define OLED_SCL_PIN  2
#elif defined(CHIP_ESP32C3)
  #define OLED_SDA_PIN  8
  #define OLED_SCL_PIN  9
#else
  #define OLED_SDA_PIN  21
  #define OLED_SCL_PIN  22
#endif

// ===== Цвета (для монохромного дисплея) =====
#define COLOR_BG       0
#define COLOR_PRIMARY  1
#define COLOR_SUCCESS  1
#define COLOR_DANGER   1
#define COLOR_TEXT     1
#define COLOR_GRAY     1

#endif
