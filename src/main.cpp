#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN D4   // SDA du RC522 branché à D4 (GPIO2)
#define RST_PIN D3  // RST du RC522 branché à D3 (GPIO0)

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Crée une instance MFRC522

void setup() {
  Serial.begin(9600);    // Initialisation du port série à 9600 bauds
  SPI.begin();           // Initialise le bus SPI
  mfrc522.PCD_Init();    // Initialise le lecteur RFID
  Serial.println("Approchez une carte RFID...");
}

void loop() {
  // Vérifie si une nouvelle carte est présente
  if ( !mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Lit la carte si présente
  if ( !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Affiche l'UID de la carte
  Serial.print("UID de la carte : ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  delay(1000); // Pause pour éviter les lectures répétées rapides
}
