#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = D3;
constexpr uint8_t SS_PIN = D4;

MFRC522 mfrc522(SS_PIN, RST_PIN);

String code="";
String UID="";
int No;

void write_UID()     //To read the data written on RFID
{
 //*********************************Set UID*******************************************
 byte buffer[34];
  byte block;
  MFRC522::StatusCode status;
  byte len;

  Serial.setTimeout(20000L) ;    
  Serial.println(F("Unique Number, ending with #"));
  len = Serial.readBytesUntil('#', (char *) buffer, 30) ; 
  for (byte i = len; i < 30; i++) buffer[i] = ' ';     

  block = 1;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));
  // Write block
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));
  
  //****************************************Set Code************************************************************8
  
  block = 4;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write block
  buffer[0]=byte('t');
  buffer[1]=byte('e');
  buffer[2]=byte('c');
  buffer[3]=byte('h');
  status = mfrc522.MIFARE_Write(block, buffer , 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));
  
  
  }

void read_UID()    //To write data on RFID
{
  code="";
  UID="";
   MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) 
    key.keyByte[i] = 0xFF;
  byte block;
  byte len;
  Serial.println("Reading Card");
  MFRC522::StatusCode status;
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card
  byte buffer1[18];

  block = 4;
  len = 18;
  //*********************Get code*******************
  
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    ESP.reset();
    return;
  }
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
      Serial.write(buffer1[i]);
      code=code+char(buffer1[i]);
    }
  }
  Serial.print(" ");

  //****************Get Unique Number*****************************
  
  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    ESP.reset();
    return;
  }

  for (uint8_t i = 0; i < 16; i++) {
    Serial.write(buffer2[i] );
    UID=UID+char(buffer2[i]);
  }
  //Serial.println(UID);

}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  
}

void loop() {

  //Detect card
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  read_UID();
  No=0;
  Serial.println(UID.toInt());

  //Interrupt From RPI
//  if(Interrupt==true)
//  {
//  write_UID();
//  }

  
  if(code=="tech")
  {
    No=UID.toInt();
    Serial.println("Yes");
    if(No>=01 && No<=2147483647)
    {
      Serial.println("Valid card");
      //Send signal to relay and Rpi  
  
      }
      else
        {
          Serial.println("Invalid Card");
        }
    }
    else
    {
      Serial.println("Invalid Card"); 
    }
  delay(1000);   
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1(); 

}
