
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
 
#define PN532_IRQ   (2)
#define PN532_RESET (3)

// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
// also change #define in Adafruit_PN532.cpp library file
   #define Serial SerialUSB
#endif

int led1=11;
int led2=12;
 
void setup() 
{
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif
  Serial.begin(115200);
    nfc.begin();

  Serial.println();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A Card ...");

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

}
void loop() 
{
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
  }

  Serial.println();
  String content= "";
  byte letter;
  for (byte i = 0; i < uidLength; i++) 
  {
     Serial.print(nfc.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(nfc.uidByte[i], HEX);
     content.concat(String(nfc.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(nfc.uidByte[i], HEX));
  }

  content.toUpperCase();
  
  if (content.substring(1) == "0x49 0xEB 0x83 0xDE") // the UID of the card/cards that you want to give access
  {
    Serial.println("RFID authenticated(rectangle)"); //the red light(led1) lights up when the first RFID tag is scanned
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    delay(1000);     
    Serial.println();
    delay(3000);
  }

  else if(content.substring(1) == "0x60 0x4D 0x70 0xA5"){//the blue light(led2) lights up when the other RFID tag is scanned
    Serial.println("RFID authenticated(circle");
    digitalWrite(led2, HIGH);
    digitalWrite(led1, LOW);
    delay(1000);     
    Serial.println();
    Serial.println();
    delay(3000);
  }
 
 else   {
    Serial.println(" Access denied");// if an invalid/unrecognizable RFID tag is detectd, an error messgage is displayed
    delay(3000);
  }
} 
