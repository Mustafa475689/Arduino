#include <Wire.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Camera I2C Address (OV7670 SCCB address is 0x21 in 7-bit form)
#define OV7670_I2C_ADDR 0x21

// --- Function Declarations ---
void setupXCLK();
void sccbInit();
void sccbWrite(uint8_t reg, uint8_t val);
void ov7670Init();
void captureFrame();

void setup() {
  // Initialize Serial port for pixel data transfer (Fast 115200 baud)
  Serial.begin(115200);

  // Set control pin directions
  pinMode(2, INPUT);   // VS (VSYNC)
  pinMode(12, INPUT);  // PLK (PCLK)
  pinMode(A0, INPUT);  // HS (HREF)

  // Data Bus inputs (PORT D & PORT C)
  DDRD &= ~0b11111100; // Digital Pins 2..7 as Inputs
  DDRB &= ~0b00000001; // Pin 8 (D2) as Input
  DDRC &= ~0b00001100; // Analog Pins A2 (D0) & A3 (D1) as Inputs

  // 1. Generate PWM Master Clock (XCLK) on Pin 11
  setupXCLK();

  // 2. Initialize SCCB / I2C
  sccbInit();

  // 3. Configure Camera Registers
  ov7670Init();
  
  delay(100);
}

void loop() {
  // Wait for user or trigger over Serial to take a photo frame
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == 'c' || c == 'C') { // Press 'c' in Serial Monitor to capture
      captureFrame();
    }
  }
}

// Generates an ~8MHz clock signal on Arduino Pin 11 using Timer 2
void setupXCLK() {
  pinMode(11, OUTPUT);
  TCCR2A = _BV(COM2A0) | _BV(WGM21); // Toggle OC2A on compare match, CTC mode
  TCCR2B = _BV(CS20);                 // No prescaling
  OCR2A = 0;                          // Toggle every 1 cycle -> 16MHz / (2 * 1) = 8MHz
}

// Initialize I2C interface
void sccbInit() {
  Wire.begin();
  TWBR = 72; // Set I2C speed to ~100kHz for stability
}

// Write a register value to OV7670 via SCCB/I2C
void sccbWrite(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(OV7670_I2C_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
  delay(1);
}

// Load basic QQVGA (160x120) & RGB565 configurations into OV7670
void ov7670Init() {
  sccbWrite(0x12, 0x80); // Reset all registers to default
  delay(100);

  // Resolution & Color setup
  sccbWrite(0x12, 0x14); // Output format: QQVGA (160x120), RGB mode
  sccbWrite(0x40, 0xD0); // Set RGB565 format
  sccbWrite(0x8C, 0x00); // Disable RGB444
  
  // Clock prescaling & scaling settings
  sccbWrite(0x11, 0x01); // Internal Clock Pre-scaler
  sccbWrite(0x0C, 0x04); // Enable QCIF / Downscaling
  sccbWrite(0x3E, 0x19); // Scaling PCLK divider
}

// Captures pixel bytes directly from parallel bus and streams over Serial
void captureFrame() {
  // 1. Wait for VSYNC pulse to go HIGH then LOW (Start of Frame)
  while (!(PIND & (1 << PIND2))); 
  while ((PIND & (1 << PIND2)));

  // QQVGA frame size: 160 x 120 pixels, 2 bytes per pixel (RGB565)
  for (int y = 0; y < 120; y++) {
    for (int x = 0; x < 160 * 2; x++) {
      
      // Wait for PCLK (Pin 12 / PB4) to go HIGH
      while (!(PINB & (1 << PINB4)));

      // Read 8-bit parallel data from split PORT registers
      // D7-D3 on PORTD (pins 7..3), D2 on PORTB (pin 8), D1-D0 on PORTC (A3..A2)
      uint8_t pixelByte = (PIND & 0xF8) | (PINB & 0x01) | ((PINC & 0x0C) >> 2);

      // Stream raw byte out via Serial
      Serial.write(pixelByte);

      // Wait for PCLK to go LOW
      while ((PINB & (1 << PINB4)));
    }
  }
}
