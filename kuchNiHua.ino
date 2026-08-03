/*
  OV7670 (with AL422B FIFO) + Arduino Uno
  Captures one QQVGA (160x120) RGB565 frame and streams it over Serial.

  WIRING (adjust the #defines below to match how you actually wire it):
    OV7670 module      Arduino Uno
    ----------------    -----------
    3.3V                3.3V
    GND                 GND
    SIOC (SCL)          A5
    SIOD (SDA)          A4
    VSYNC               D2   (must be an interrupt-capable pin)
    XCLK                D11  (Arduino generates this clock, ~8MHz)
    WRST                D3
    RRST                D5
    OE (read enable)    D6
    RCLK                D7
    D0..D7 (data bus)   D8, D9, D10, D12, D13, A0, A1, A2

  Trigger a capture by sending the character 'c' over Serial (115200 baud).
  Pair this with ov7670_viewer.py on the PC side.
*/
//waiting for arduino esp 32
#include <Wire.h>

// ---------- Pin assignments (match these to your wiring) ----------
#define PIN_VSYNC   2
#define PIN_XCLK    11
#define PIN_WRST    3
#define PIN_RRST    5
#define PIN_OE      6
#define PIN_RCLK    7

// Camera data bus D0..D7 -> these 8 Arduino pins, in order
const uint8_t DATA_PINS[8] = {8, 9, 10, 12, 13, A0, A1, A2};

#define OV7670_WRITE_ADDR 0x42  // 7-bit address 0x21, write

#define FRAME_WIDTH  160
#define FRAME_HEIGHT 120
#define BYTES_PER_PIXEL 2
const uint32_t FRAME_BYTES = (uint32_t)FRAME_WIDTH * FRAME_HEIGHT * BYTES_PER_PIXEL;

// ---------- SCCB (I2C-like) register write ----------
bool writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(OV7670_WRITE_ADDR >> 1);
  Wire.write(reg);
  Wire.write(val);
  uint8_t err = Wire.endTransmission();
  delay(1);
  return err == 0;
}

// QQVGA RGB565 register init table (reg, value)
const uint8_t OV7670_QQVGA_RGB565[][2] = {
  {0x12, 0x80}, // COM7: reset
  {0x12, 0x14}, // COM7: QVGA + RGB
  {0x11, 0x01}, // CLKRC: prescaler
  {0x0C, 0x00}, // COM3
  {0x3E, 0x00}, // COM14
  {0x04, 0x00}, // COM1
  {0x40, 0xD0}, // COM15: RGB565, full range
  {0x3A, 0x04}, // TSLB
  {0x14, 0x18}, // COM9: AGC ceiling
  {0x4F, 0xB3}, {0x50, 0xB3}, {0x51, 0x00}, {0x52, 0x3D}, {0x53, 0xA7}, {0x54, 0xE4}, // matrix
  {0x58, 0x9E}, // MTXS
  {0x3D, 0xC0}, // COM13: gamma + UV auto
  {0x17, 0x14}, {0x18, 0x02}, // HSTART/HSTOP
  {0x32, 0x80}, // HREF
  {0x19, 0x03}, {0x1A, 0x7B}, // VSTART/VSTOP
  {0x03, 0x0A}, // VREF
  {0x0E, 0x61}, {0x0F, 0x4B}, {0x16, 0x02},
  {0x1E, 0x37}, // MVFP: mirror/flip if your image looks reversed, toggle bits here
  {0x21, 0x02}, {0x22, 0x91}, {0x29, 0x07}, {0x33, 0x0B}, {0x35, 0x0B},
  {0x37, 0x1D}, {0x38, 0x71}, {0x39, 0x2A},
  {0x3C, 0x78}, {0x4D, 0x40}, {0x4E, 0x20},
  {0x69, 0x00}, {0x6B, 0x4A},
  {0x74, 0x10}, {0x8D, 0x4F}, {0x8E, 0x00}, {0x8F, 0x00}, {0x90, 0x00}, {0x91, 0x00},
  {0x96, 0x00}, {0x9A, 0x00}, {0xB0, 0x84}, {0xB1, 0x0C}, {0xB2, 0x0E}, {0xB3, 0x82},
  {0xB8, 0x0A},
};
const uint16_t NUM_REGS = sizeof(OV7670_QQVGA_RGB565) / 2;

// ---------- XCLK generation (Timer2, ~8MHz square wave on pin 11 / OC2A) ----------
void startXCLK() {
  pinMode(PIN_XCLK, OUTPUT);
  TCCR2A = _BV(COM2A0) | _BV(WGM21); // toggle OC2A on compare match, CTC mode
  TCCR2B = _BV(CS20);                // no prescaling
  OCR2A = 0;                         // toggle every cycle -> 16MHz/2 = 8MHz
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(PIN_VSYNC, INPUT);
  pinMode(PIN_WRST, OUTPUT);
  pinMode(PIN_RRST, OUTPUT);
  pinMode(PIN_OE, OUTPUT);
  pinMode(PIN_RCLK, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) pinMode(DATA_PINS[i], INPUT);

  digitalWrite(PIN_WRST, HIGH); // not resetting
  digitalWrite(PIN_RRST, HIGH);
  digitalWrite(PIN_OE, HIGH);   // FIFO output disabled (active low)
  digitalWrite(PIN_RCLK, LOW);

  startXCLK();
  delay(100); // let XCLK stabilize before talking to the sensor

  bool ok = true;
  for (uint16_t i = 0; i < NUM_REGS; i++) {
    ok &= writeReg(OV7670_QQVGA_RGB565[i][0], OV7670_QQVGA_RGB565[i][1]);
    if (OV7670_QQVGA_RGB565[i][0] == 0x12 && OV7670_QQVGA_RGB565[i][1] == 0x80) {
      delay(100); // reset needs settling time
    }
  }

  Serial.println(ok ? "OV7670 init OK" : "OV7670 init FAILED (check wiring/address)");
  Serial.println("Send 'c' to capture a frame.");
}

uint8_t readDataBus() {
  uint8_t b = 0;
  for (uint8_t i = 0; i < 8; i++) {
    if (digitalRead(DATA_PINS[i])) b |= (1 << i);
  }
  return b;
}

void captureFrame() {
  // 1) Reset FIFO write pointer synced to a VSYNC pulse so the next frame
  //    written starts cleanly.
  while (digitalRead(PIN_VSYNC) == LOW) {}   // wait for VSYNC high
  while (digitalRead(PIN_VSYNC) == HIGH) {}  // wait for it to go low
  digitalWrite(PIN_WRST, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_WRST, HIGH);

  // Let one full frame be written into the FIFO (VSYNC low->high->low cycle)
  while (digitalRead(PIN_VSYNC) == LOW) {}
  while (digitalRead(PIN_VSYNC) == HIGH) {}

  // 2) Reset the FIFO read pointer
  digitalWrite(PIN_RRST, LOW);
  digitalWrite(PIN_RCLK, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_RCLK, HIGH);
  delayMicroseconds(2);
  digitalWrite(PIN_RRST, HIGH);

  // 3) Enable output and clock out every byte
  digitalWrite(PIN_OE, LOW);

  Serial.println("FRAME_START");
  for (uint32_t i = 0; i < FRAME_BYTES; i++) {
    digitalWrite(PIN_RCLK, LOW);
    delayMicroseconds(1);
    uint8_t b = readDataBus();
    Serial.write(b);
    digitalWrite(PIN_RCLK, HIGH);
    delayMicroseconds(1);
  }
  Serial.println();
  Serial.println("FRAME_END");

  digitalWrite(PIN_OE, HIGH);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'c' || c == 'C') {
      captureFrame();
      // end hu gya 
    }
  }
}
