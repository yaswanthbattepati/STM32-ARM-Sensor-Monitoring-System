#include <Wire.h>

/*
===========================================================
 STM32L031K6 ARM SENSOR MONITORING & CONTROL SYSTEM
===========================================================

WOKWI CONNECTIONS
-----------------------------------------------------------
Potentiometer:
    VCC  -> 3.3V
    GND  -> GND
    SIG  -> A0

LCD 16x2 I2C:
    VCC  -> 3.3V
    GND  -> GND
    SDA  -> D4
    SCL  -> D5

GREEN LED:
    D3 -> 220 ohm -> LED long leg (+)
    LED short leg (-) -> GND

YELLOW LED:
    D6 -> 220 ohm -> LED long leg (+)
    LED short leg (-) -> GND

RED LED:
    D12 -> 220 ohm -> LED long leg (+)
    LED short leg (-) -> GND

PUSH BUTTON:
    A1 -> one side of button
    other side -> GND

===========================================================
*/


/* ========================================================
   PIN DEFINITIONS
   ======================================================== */

// Status LEDs
const int GREEN_LED  = D3;
const int YELLOW_LED = D6;
const int RED_LED    = D12;

// Analog sensor
const int SENSOR_PIN = A0;

// Push button
const int BUTTON_PIN = A1;


/* ========================================================
   LCD SETTINGS
   ======================================================== */

#define LCD_ADDRESS 0x27

// LCD pins
// D4 = SDA
// D5 = SCL


/* ========================================================
   BUTTON INTERRUPT FLAG
   ======================================================== */

volatile bool buttonPressed = false;


/* ========================================================
   INTERRUPT SERVICE ROUTINE
   ======================================================== */

void buttonInterrupt()
{
  buttonPressed = true;
}


/* ========================================================
   LCD LOW LEVEL FUNCTIONS
   ======================================================== */

// Send 4-bit data to LCD through I2C

void lcdWrite4Bits(byte data)
{
  Wire.beginTransmission(LCD_ADDRESS);

  Wire.write(data | 0x0C);   // EN = 1
  Wire.write(data | 0x08);   // EN = 0

  Wire.endTransmission();

  delayMicroseconds(100);
}


// Send command to LCD

void lcdCommand(byte command)
{
  byte high = command & 0xF0;
  byte low  = (command << 4) & 0xF0;

  lcdWrite4Bits(high);
  lcdWrite4Bits(low);
}


// Send character to LCD

void lcdData(byte data)
{
  byte high = data & 0xF0;
  byte low  = (data << 4) & 0xF0;

  Wire.beginTransmission(LCD_ADDRESS);

  Wire.write(high | 0x0D);
  Wire.write(high | 0x09);

  Wire.write(low | 0x0D);
  Wire.write(low | 0x09);

  Wire.endTransmission();

  delayMicroseconds(100);
}


// Initialize LCD

void lcdInit()
{
  delay(50);

  lcdCommand(0x33);
  lcdCommand(0x32);

  // 4-bit, 2-line, 5x8 font
  lcdCommand(0x28);

  // Display ON, cursor OFF
  lcdCommand(0x0C);

  // Entry mode
  lcdCommand(0x06);

  // Clear display
  lcdCommand(0x01);

  delay(5);
}


// Clear LCD

void lcdClear()
{
  lcdCommand(0x01);
  delay(2);
}


// Set cursor

void lcdSetCursor(byte column, byte row)
{
  byte address;

  if (row == 0)
  {
    address = 0x80 + column;
  }
  else
  {
    address = 0xC0 + column;
  }

  lcdCommand(address);
}


// Print string

void lcdPrint(const char *text)
{
  while (*text)
  {
    lcdData(*text);
    text++;
  }
}


// Print integer

void lcdPrintNumber(int number)
{
  char buffer[10];

  sprintf(buffer, "%d", number);

  lcdPrint(buffer);
}


/* ========================================================
   LED CONTROL FUNCTIONS
   ======================================================== */

void setNormal()
{
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
}


void setWarning()
{
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(RED_LED, LOW);
}


void setAlert()
{
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, HIGH);
}


void turnAllLEDsOff()
{
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
}


/* ========================================================
   SETUP
   ======================================================== */

void setup()
{
  /* ------------------------------------------
     SERIAL COMMUNICATION
     ------------------------------------------ */

  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("=================================");
  Serial.println("STM32L031K6 SENSOR MONITOR");
  Serial.println("System Starting...");
  Serial.println("=================================");


  /* ------------------------------------------
     LED CONFIGURATION
     ------------------------------------------ */

  pinMode(GREEN_LED, OUTPUT);

  pinMode(YELLOW_LED, OUTPUT);

  pinMode(RED_LED, OUTPUT);


  /* Turn LEDs OFF initially */

  turnAllLEDsOff();


  /* ------------------------------------------
     ANALOG SENSOR
     ------------------------------------------ */

  pinMode(SENSOR_PIN, INPUT);

  /*
     STM32L031 ADC:
     12-bit resolution

     Range:
     0 - 4095
  */

  analogReadResolution(12);


  /* ------------------------------------------
     PUSH BUTTON
     ------------------------------------------ */

  /*
     Button is connected between A1 and GND.

     INPUT_PULLUP means:

     Button NOT pressed = HIGH
     Button pressed     = LOW
  */

  pinMode(BUTTON_PIN, INPUT_PULLUP);


  /*
     Configure external interrupt.

     When button goes from HIGH -> LOW,
     buttonInterrupt() is triggered.
  */

  attachInterrupt(
    digitalPinToInterrupt(BUTTON_PIN),
    buttonInterrupt,
    FALLING
  );


  /* ------------------------------------------
     I2C
     ------------------------------------------ */

  /*
     Nucleo-L031K6:

     D4 = PB7 = SDA
     D5 = PB6 = SCL
  */

  Wire.setSDA(D4);
  Wire.setSCL(D5);

  Wire.begin();

  delay(100);


  /* ------------------------------------------
     LCD
     ------------------------------------------ */

  lcdInit();

  lcdClear();

  lcdSetCursor(0, 0);
  lcdPrint("STM32 MONITOR");

  lcdSetCursor(0, 1);
  lcdPrint("System Ready");

  delay(2000);

  lcdClear();


  /* ------------------------------------------
     STARTUP COMPLETE
     ------------------------------------------ */

  Serial.println("System Ready");
  Serial.println("ADC Resolution: 12-bit");
  Serial.println("Sensor Pin: A0");
  Serial.println("LCD: I2C");
  Serial.println("Button: A1");
  Serial.println("---------------------------------");
}


/* ========================================================
   MAIN LOOP
   ======================================================== */

void loop()
{
  /* ======================================================
     READ SENSOR
     ====================================================== */

  int adcValue = analogRead(SENSOR_PIN);


  /* ======================================================
     CONVERT ADC VALUE TO PERCENTAGE
     ====================================================== */

  int percentage = map(
    adcValue,
    0,
    4095,
    0,
    100
  );


  /* Make sure percentage stays inside 0-100 */

  percentage = constrain(
    percentage,
    0,
    100
  );


  /* ======================================================
     SYSTEM STATE
     ====================================================== */

  const char *status;


  /* ------------------------------------------
     NORMAL
     0 - 30%
     ------------------------------------------ */

  if (percentage <= 30)
  {
    status = "NORMAL";

    setNormal();
  }


  /* ------------------------------------------
     WARNING
     31 - 70%
     ------------------------------------------ */

  else if (percentage <= 70)
  {
    status = "WARNING";

    setWarning();
  }


  /* ------------------------------------------
     ALERT
     71 - 100%
     ------------------------------------------ */

  else
  {
    status = "ALERT";

    setAlert();
  }


  /* ======================================================
     LCD DISPLAY
     ====================================================== */

  lcdSetCursor(0, 0);

  if (percentage <= 30)
  {
    lcdPrint("Status: NORMAL  ");
  }
  else if (percentage <= 70)
  {
    lcdPrint("Status: WARNING ");
  }
  else
  {
    lcdPrint("Status: ALERT   ");
  }


  /* Second LCD line */

  lcdSetCursor(0, 1);

  lcdPrint("Level: ");

  if (percentage < 100)
  {
    lcdPrint(" ");
  }

  if (percentage < 10)
  {
    lcdPrint(" ");
  }

  lcdPrintNumber(percentage);

  lcdPrint("%    ");


  /* ======================================================
     SERIAL / UART OUTPUT
     ====================================================== */

  Serial.print("ADC = ");

  Serial.print(adcValue);

  Serial.print(" | Level = ");

  Serial.print(percentage);

  Serial.print("% | Status = ");

  Serial.println(status);


  /* ======================================================
     BUTTON INTERRUPT EVENT
     ====================================================== */

  if (buttonPressed)
  {
    /*
       Temporarily disable interrupts while
       accessing the shared flag.
    */

    noInterrupts();

    buttonPressed = false;

    interrupts();


    /* ------------------------------------------
       BUTTON ACTION
       ------------------------------------------ */

    Serial.println("---------------------------------");
    Serial.println("BUTTON PRESSED");
    Serial.println("System status checked");
    Serial.println("---------------------------------");


    /*
       Show button message on LCD
    */

    lcdClear();

    lcdSetCursor(0, 0);
    lcdPrint("Button Pressed");

    lcdSetCursor(0, 1);
    lcdPrint("System Active");

    delay(1000);
  }


  /* ======================================================
     UPDATE RATE
     ====================================================== */

  delay(500);
}