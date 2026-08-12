#include "Vendista.h"

bool Vendista::ReadPacket(uint8_t* buffer, size_t maxSize, size_t& outLen) {
  uint8_t lenLow, lenHigh;

  if (_stream.readBytes(&lenLow, 1) != 1) return false;
  if (_stream.readBytes(&lenHigh, 1) != 1) return false;
  uint16_t bodyLen = (uint16_t)lenLow | ((uint16_t)lenHigh << 8);

  if (bodyLen > maxSize) {
    return false;
  }

  uint8_t dummy[2];
  if (_stream.readBytes(dummy, 2) != 2) return false;
  if (_stream.readBytes(buffer, bodyLen) != bodyLen) return false;

  outLen = bodyLen;
  return true;
}

bool Vendista::ReadCardRequestSync(int32_t amount) {
  delay(_delay_before_packet);
  byte packet_body[12] = { VENDISTA_READ_CARD, 0x00, 0x00, 0x00, 0x00, VENDISTA_RUB, 0x00, 0x00, 0x00, 0x00, 0x01 };
  packet_body[4] = (amount >> 24) & 0xFF;
  packet_body[3] = (amount >> 16) & 0xFF;
  packet_body[2] = (amount >> 8) & 0xFF;
  packet_body[1] = amount & 0xFF;
  byte packet[16];
  AddHeader(packet, sizeof(packet), packet_body, sizeof(packet_body));
  _stream.write(packet, sizeof(packet));

  uint8_t packetBuffer[256];
  size_t length = 0;
  while (true) {
    if (ReadPacket(packetBuffer, sizeof(packetBuffer), length)) {
      //_stream.println("Пакет прочитан.");
      if (packetBuffer[0] == VENDISTA_CARD_AUTH) {
        if (packetBuffer[1] == VENDISTA_SUCCESS) {
          return true;
        }
        if (packetBuffer[1] == VENDISTA_FAILED) {
          return false;
        }
      }
      if (packetBuffer[0] == VENDISTA_CARD_READ) {
        if (packetBuffer[1] == VENDISTA_FAILED) {
          return false;
        }
      }
    }
  }
}

void Vendista::FillScreen(int16_t color) {
  delay(_delay_before_packet);
  byte packet_body[3] = { VENDISTA_FILL_SCREEN, 0x00, 0x00 };
  packet_body[2] = (color >> 8) & 0xFF;
  packet_body[1] = color & 0xFF;
  byte packet[7];
  AddHeader(packet, sizeof(packet), packet_body, sizeof(packet_body));
  _stream.write(packet, sizeof(packet));
}

void Vendista::WriteLine(int16_t background_color, int16_t text_color, byte Font, int16_t X, int16_t Y, String text) {
  delay(_delay_before_packet);
  byte buffer[100];
  size_t len = utf8ToCp1251(text, buffer, sizeof(buffer));

  byte packet_body[11 + len];
  packet_body[0] = VENDISTA_WRITE_LINE;
  packet_body[1] = background_color & 0xFF;
  packet_body[2] = (background_color >> 8) & 0xFF;
  packet_body[3] = text_color & 0xFF;
  packet_body[4] = (text_color >> 8) & 0xFF;
  packet_body[5] = Font & 0xFF;
  packet_body[6] = X & 0xFF;
  packet_body[7] = (X >> 8) & 0xFF;
  packet_body[8] = Y & 0xFF;
  packet_body[9] = (Y >> 8) & 0xFF;
  packet_body[10] = len & 0xFF;
  memcpy(packet_body + 11, buffer, len);

  byte packet[sizeof(packet_body) + 4];
  AddHeader(packet, sizeof(packet), packet_body, sizeof(packet_body));
  _stream.write(packet, sizeof(packet));
}

void Vendista::CancelReadCard() {
  byte packet_body[1] = { VENDISTA_CANCEL_READ_CARD };
  byte packet[sizeof(packet_body) + 4];
  AddHeader(packet, sizeof(packet), packet_body, sizeof(packet_body));
  _stream.write(packet, sizeof(packet));
}

void Vendista::Reboot() {
  byte packet_body[1] = { VENDISTA_ASK_REBOOT };
  byte packet[sizeof(packet_body) + 4];
  AddHeader(packet, sizeof(packet), packet_body, sizeof(packet_body));
  _stream.write(packet, sizeof(packet));
}

void Vendista::AddHeader(byte* buffer, size_t length_of_buffer, byte* packet_body, size_t length_of_packet_body) {
  uint16_t crc = MakeCRC16(packet_body, length_of_packet_body);
  buffer[0] = length_of_packet_body & 0xFF;  // Самый старший байт
  buffer[1] = (length_of_packet_body >> 8) & 0xFF;
  buffer[2] = crc & 0xFF;
  buffer[3] = (crc >> 8) & 0xFF;
  memcpy(buffer + 4, packet_body, length_of_packet_body);
}

size_t Vendista::utf8ToCp1251(const String& input, uint8_t* output, size_t maxSize) {
  size_t outIdx = 0;
  size_t len = input.length();

  for (size_t i = 0; i < len && outIdx < maxSize; i++) {
    uint8_t c = (uint8_t)input[i];

    if (c < 0x80) {
      // ASCII – копируем как есть
      output[outIdx++] = c;
    } else if ((c & 0xE0) == 0xC0) {
      // Двухбайтовый UTF-8 (все русские буквы)
      if (i + 1 >= len) break;  // неполная последовательность
      uint8_t c2 = (uint8_t)input[i + 1];
      if ((c2 & 0xC0) != 0x80) break;  // неверный формат

      uint16_t code = ((c & 0x1F) << 6) | (c2 & 0x3F);
      uint8_t cp = '?';  // замена для неподдерживаемых символов

      if (code == 0x0401) {  // Ё
        cp = 0xA8;
      } else if (code == 0x0451) {  // ё
        cp = 0xB8;
      } else if (code >= 0x0410 && code <= 0x042F) {  // А‑Я
        cp = 0xC0 + (code - 0x0410);
      } else if (code >= 0x0430 && code <= 0x044F) {  // а‑я
        cp = 0xE0 + (code - 0x0430);
      }
      // иначе оставляем '?'

      output[outIdx++] = cp;
      i++;  // пропускаем второй байт
    } else if ((c & 0xF0) == 0xE0) {
      // Трёхбайтовый UTF-8 (редко, но обрабатываем)
      if (i + 2 >= len) break;
      uint8_t c2 = (uint8_t)input[i + 1];
      uint8_t c3 = (uint8_t)input[i + 2];
      if ((c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80) break;

      uint32_t code = ((c & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
      // Здесь можно добавить обработку других символов, но обычно не нужно
      output[outIdx++] = '?';
      i += 2;
    } else {
      // Прочие случаи (4‑байтовые, неверные) – заменяем на '?'
      output[outIdx++] = '?';
    }
  }

  return outIdx;
}

uint16_t Vendista::MakeCRC16(const uint8_t* data, size_t len) {
  uint16_t crc = 0xFFFF;
  const uint16_t poly = 0x8005;
  for (size_t i = 0; i < len; i++) {
    crc ^= (uint16_t)data[i] << 8;  // байт в старший разряд
    for (int bit = 0; bit < 8; bit++) {
      if (crc & 0x8000) crc = (crc << 1) ^ poly;
      else crc <<= 1;
    }
  }
  return crc ^ 0xFFFF;
}