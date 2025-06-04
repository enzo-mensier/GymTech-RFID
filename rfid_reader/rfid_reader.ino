#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Configuration WiFi
const char* ssid = "...";
const char* password = "...";
const char* apiUrl = "http://votre-serveur.com/domotique/rfid/api/check_uid.php?uid=";

// Broches du RC522
#define SS_PIN D4   // SDA (GPIO2)
#define RST_PIN D3  // RST (GPIO0)

// Broches LED RGB (cathode commune)
#define RGB_ROUGE D0  // GPIO16 (Rouge)
#define RGB_VERT  D1  // GPIO5  (Vert)
#define RGB_BLEU  D8  // GPIO15 (Bleu)

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Déclaration des fonctions
void eteindreLED();
void afficherErreur();

// Structure pour stocker les informations de l'utilisateur
struct Utilisateur {
  String prenom;
  String genre;
};

// Fonction pour récupérer les informations de l'utilisateur depuis l'API
Utilisateur getUtilisateur(String uid) {
  Utilisateur user;
  
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    // Vérification locale des UID connus avant d'interroger l'API
    if (uid == "7AA3AD15") {
      user.prenom = "Enzo";
      user.genre = "Homme";
      return user;
    } else if (uid == "63789594") {
      user.prenom = "Reine";
      user.genre = "Femme";
      return user;
    } else if (uid == "F99794A3") {
      user.prenom = "Mael";
      user.genre = "Homme";
      return user;
    }
    
    // Si l'UID n'est pas reconnu localement, on essaie l'API
    String url = String(apiUrl) + uid;
    
    // Utilisation de l'API moderne avec WiFiClient
    if (http.begin(client, url)) {
      int httpCode = http.GET();
      
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        
        // Parse la réponse JSON
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        
        if (!error) {
          user.prenom = doc["prenom"].as<String>();
          user.genre = doc["genre"].as<String>();
        } else {
          Serial.println("Erreur d'analyse JSON");
        }
      } else {
        Serial.print("Erreur HTTP: ");
        Serial.println(httpCode);
      }
      
      http.end();
    } else {
      Serial.println("Impossible de se connecter au serveur");
    }
  } else {
    Serial.println("Non connecté au WiFi - Vérification locale uniquement");
  }
  
  return user;
}

void setup() {
  // Initialisation du port série
  Serial.begin(115200);
  while (!Serial) { // Attend que le port série soit prêt
    delay(10);
  }
  Serial.println("\n🔌 Initialisation du système...");
  
  // Initialisation du module RC522
  Serial.println("🔄 Initialisation du module RFID...");
  SPI.begin();
  mfrc522.PCD_Init();
  
  // Configuration des broches LED en sortie
  pinMode(RGB_ROUGE, OUTPUT);
  pinMode(RGB_VERT, OUTPUT);
  pinMode(RGB_BLEU, OUTPUT);
  Serial.println("✅ Broches LED configurées");
  
  // Éteint les LEDs
  eteindreLED();
  
  // Connexion au WiFi
  Serial.print("📶 Connexion au WiFi ");
  Serial.print(ssid);
  Serial.println("...");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Attente de la connexion (max 20 secondes)
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
    delay(500);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("✅ WiFi connecté");
    Serial.print("📡 Adresse IP : ");
    Serial.println(WiFi.localIP());
    
    // Test de la connexion réseau
    Serial.println("🌐 Test de connexion à internet...");
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("✅ Connexion internet active");
    } else {
      Serial.println("❌ Pas de connexion internet");
    }
  } else {
    Serial.println("\n❌ Échec de la connexion WiFi");
  }
  
  Serial.println("\n🔍 Prêt à lire les badges RFID...");
}

void loop() {
  // Vérifie si une carte est détectée
  if (!mfrc522.PICC_IsNewCardPresent()) {
    delay(50);
    return;
  }
  
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  // Récupère l'UID de la carte
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  
  // Convertit en majuscules pour la compatibilité
  uid.toUpperCase();
  
  Serial.print("\n🎫 UID détecté : ");
  Serial.println(uid);
  
  // Vérifie la connexion WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ Reconnexion au WiFi...");
    WiFi.reconnect();
    delay(1000);
    
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("❌ Impossible de se reconnecter au WiFi");
      afficherErreur();
      return;
    }
  }

  // Vérifie l'UID et allume la LED correspondante
  if (uid == "7AA3AD15") {
    // Enzo - Vert
    digitalWrite(RGB_ROUGE, HIGH);  // Rouge éteint
    digitalWrite(RGB_VERT, LOW);    // Vert allumé
    digitalWrite(RGB_BLEU, HIGH);   // Bleu éteint
    Serial.println("🟢 Accès autorisé : Enzo (Admin)");
  } 
  else if (uid == "63789594") {
    // Reine - Bleu
    digitalWrite(RGB_ROUGE, HIGH);  // Rouge éteint
    digitalWrite(RGB_VERT, HIGH);   // Vert éteint
    digitalWrite(RGB_BLEU, LOW);    // Bleu allumé
    Serial.println("🔵 Accès autorisé : Reine (Femmes)");
  } 
  else if (uid == "F99794A3") {
    // Mael - Jaune (Rouge + Vert)
    digitalWrite(RGB_ROUGE, LOW);  // Rouge allumé
    digitalWrite(RGB_VERT, LOW);    // Vert allumé
    digitalWrite(RGB_BLEU, HIGH);   // Bleu éteint
    Serial.println("🟡 Accès autorisé : Mael (Hommes)");
  }
  else {
    // UID non reconnu - Rouge
    digitalWrite(RGB_ROUGE, LOW);   // Rouge allumé
    digitalWrite(RGB_VERT, HIGH);   // Vert éteint
    digitalWrite(RGB_BLEU, HIGH);   // Bleu éteint
    Serial.println("🔴 Accès refusé - Badge non reconnu");
  }
  
  // Si la connexion WiFi est établie, on peut essayer de vérifier l'UID via l'API
  if (WiFi.status() == WL_CONNECTED) {
    Utilisateur user = getUtilisateur(uid);
    if (user.prenom != "") {
      Serial.print("👤 Utilisateur trouvé : ");
      Serial.print(user.prenom);
      Serial.print(" (").print(user.genre).println(")");
    }
  }
  
  // Attend 2 secondes avant de réinitialiser
  delay(2000);
  eteindreLED();
  mfrc522.PICC_HaltA();
}

// Éteint toutes les LEDs
void eteindreLED() {
  digitalWrite(RGB_ROUGE, HIGH);  // Éteint la LED rouge
  digitalWrite(RGB_VERT, HIGH);   // Éteint la LED verte
  digitalWrite(RGB_BLEU, HIGH);   // Éteint la LED bleue
}

// Affiche une erreur (clignotement)
void afficherErreur() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(RGB_ROUGE, HIGH);
    digitalWrite(RGB_BLEU, HIGH);
    delay(200);
    digitalWrite(RGB_ROUGE, LOW);
    digitalWrite(RGB_BLEU, LOW);
    delay(200);
  }
}















