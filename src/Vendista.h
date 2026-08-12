#pragma once
#include <Arduino.h>

// COMMANDS TO TERMINAL
#define VENDISTA_READ_CARD 0x01
#define VENDISTA_FILL_SCREEN 0x09
#define VENDISTA_WRITE_LINE 0x0A
#define VENDISTA_ASK_REBOOT 0x05
#define VENDISTA_CANCEL_READ_CARD 0x08

// COMMANDS FROM TERMINAL
#define VENDISTA_TOUCH 0x11
#define VENDISTA_REBOOT 0x16
#define VENDISTA_CARD_AUTH 0x14
#define VENDISTA_CARD_READ 0x13
#define VENDISTA_CUSTOM_PACKET 0x1B
#define VENDISTA_SUCCESS 0x01
#define VENDISTA_FAILED 0x00

// COLORS IN RGB565 (2 BYTES)
#define VENDISTA_RED_COLOR 0xF800
#define VENDISTA_GREEN_COLOR 0x07E0
#define VENDISTA_BLUE_COLOR 0x001F
#define VENDISTA_YELLOW_COLOR 0xFFE0
#define VENDISTA_PURPLE_COLOR 0xF81F
#define VENDISTA_CYAN_COLOR 0x07FF
#define VENDISTA_BLACK_COLOR 0x0000
#define VENDISTA_WHITE_COLOR 0xFFFF

// CURRENCY CODES (ISO-4217)
#define VENDISTA_RUB 0x06, 0x43
#define VENDISTA_EUR 0x09, 0x78
#define VENDISTA_USD 0x08, 0x40
#define VENDISTA_BYN 0x09, 0x33
#define VENDISTA_KZT 0x03, 0x98

class Vendista {
public:
  Vendista(Stream& stream, int delay_before_packet = 500) : _stream(stream), _delay_before_packet(delay_before_packet) {}

  bool ReadPacket(uint8_t* buffer, size_t maxSize, size_t& outLen);

  bool ReadCardRequestSync(int32_t amount);

  void FillScreen(int16_t color);

  void WriteLine(int16_t background_color, int16_t text_color, byte Font, int16_t X, int16_t Y, String text);

  void CancelReadCard();

  void Reboot();

private:
  Stream& _stream;
  int _delay_before_packet;

  void AddHeader(byte* buffer, size_t length_of_buffer, byte* packet_body, size_t length_of_packet_body);

  size_t utf8ToCp1251(const String& input, uint8_t* output, size_t maxSize);

  uint16_t MakeCRC16(const uint8_t* data, size_t len);
};