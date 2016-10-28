/*
 * BIRDBROODER Arduino input/output module
 * josh@finlay.id.au
 *
 ----------------------------------------------------------------------------- Nicola Coppola
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               51                MOSI
 * SPI MISO   12               50                MISO
 * SPI SCK    13               52                SCK
 *
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <SPI.h>
#include <MFRC522.h>
#include "pitches.h"

#define SS_PIN 10
#define RST_PIN 9
#define speakerOut 3
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
/*int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};*/
int melody[] = {
  NOTE_B4, NOTE_C6, NOTE_G6, NOTE_CS7
};

int errormel[] = {
  NOTE_D4, NOTE_D4, NOTE_D4
};

int noteDurations[] = {
  8, 8, 8, 8
};

int errDurations[] {
  3, 3, 3, 3
};
bool dataReady = false;
String dataRcvd = "";
bool gotReply = true;

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  pinMode(speakerOut, OUTPUT);
  Serial.println("BOOT rfidTimesheet/1.0 -- ready");
}

void loop() {
  serialEvent();

  if (gotReply) {
    if (!mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    if (!mfrc522.PICC_ReadCardSerial()) {
      return;
    }
  
  
    gotReply = false;
  
    Serial.print("SCAN ");
    
    MFRC522::Uid cuid;
  
    bool firstDash = true;
    cuid = mfrc522.uid;
    for (byte i = 0; i < cuid.size; i++) {
      if(cuid.uidByte[i] < 0x10)
        Serial.print(F("0"));
      else {
        if (firstDash) {
          firstDash = false;
        }
        else Serial.print(F("-"));
      }
      Serial.print(cuid.uidByte[i], HEX);
    }
    Serial.println();
    delay(1000);
  }

  if (dataReady) {
    gotReply = true;
    if (dataRcvd == "TONEUP\n") { toneUp(); }
    if (dataRcvd == "TONEDOWN\n") { toneDown(); }
    if (dataRcvd == "TONEERR\n") { toneErr(); }
    Serial.print("DEBUG ");
    Serial.print(dataRcvd);
    dataRcvd = "";
    dataReady = false;
  }
    
    serialEvent();
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    dataRcvd += inChar;
    if (inChar == '\n') {
      dataReady = true;
    }
  }
}

void toneUp() {
    for (int thisNote = 0; thisNote < 4; thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(speakerOut, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1;
      delay(pauseBetweenNotes);
      noTone(8);
    }
}

void toneDown() {
    for (int thisNote = 3; thisNote > -1; thisNote--) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(speakerOut, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1;
      delay(pauseBetweenNotes);
      noTone(8);
    }
}

void toneErr() {
    for (int thisNote = 0; thisNote < 3; thisNote++) {
      int noteDuration = 1000 / errDurations[thisNote];
      tone(speakerOut, errormel[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.3;
      delay(pauseBetweenNotes);
      noTone(8);
    }
}
