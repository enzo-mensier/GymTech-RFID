#include <SPI.h>
#include <MFRC522.h>

// === Broches du RC522 ===
#define SS_PIN D4   // SDA (GPIO2)
#define RST_PIN D3  // RST (GPIO0)

// === LED RGB (cathode commune) ===
#define RGB_ROUGE D8  // GPIO15
#define RGB_BLEU  D0  // GPIO16

MFRC522 mfrc522(SS_PIN, RST_PIN);

// === UIDs autorisés ===
byte uid_bleu[] = {0xF9, 0x97, 0x94, 0xA3};   // Bleu
byte uid_rose[] = {0x7A, 0xA3, 0xAD, 0x15};   // Rose (Rouge + Bleu)

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(RGB_ROUGE, OUTPUT);
  pinMode(RGB_BLEU, OUTPUT);
  eteindreLED();

  Serial.println("📶 Lecteur RFID prêt. Placez une carte...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  // Affiche UID détecté
  Serial.print("🔍 UID détecté :");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Comparaison des UIDs
  if (compareUID(mfrc522.uid.uidByte, uid_bleu, 4)) {
    Serial.println("✅ Badge HOMME reconnu");
    afficherCouleur(false, true);  // Bleu
  }
  else if (compareUID(mfrc522.uid.uidByte, uid_rose, 4)) {
    Serial.println("✅ Badge FEMME reconnu");
    afficherCouleur(true, true);   // Rose = Rouge + Bleu
  }
  else {
    Serial.println("⛔ Badge inconnu");
    afficherCouleur(true, false);  // Rouge seul
  }

  delay(2000);
  eteindreLED();
  mfrc522.PICC_HaltA();
}
    
// Compare deux UIDs
bool compareUID(byte *uid, byte *ref, byte size) {
  for (byte i = 0; i < size; i++) {
    if (uid[i] != ref[i]) return false;
  }
  return true;
}

// Allume la bonne couleur
void afficherCouleur(bool rouge, bool bleu) {
  digitalWrite(RGB_ROUGE, rouge ? HIGH : LOW);
  digitalWrite(RGB_BLEU,  bleu ? HIGH : LOW);
}

// Éteint les LEDs
void eteindreLED() {
  digitalWrite(RGB_ROUGE, LOW);
  digitalWrite(RGB_BLEU,  LOW);
}















