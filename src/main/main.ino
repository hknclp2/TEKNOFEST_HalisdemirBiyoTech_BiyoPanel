/*
 * Halisdemir BiyoTech - Akıllı Çift Fotobiyoreaktör Otomasyon Sistemi
 * Bu yazılım Deneyap Kart / ESP32 mimarisi için Nesne Yönelimli (OOP) olarak geliştirilmiştir.
 * Çift Panel (Aktif/Pasif) Kontrol Grubu ve Bilimsel Optik Yoğunluk (OD) Dönüşüm Kodu
 */

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// --- Pin Tanımlamaları ---
#define PIN_PH_AKTIF    32  // Analog pH - Aktif Panel (ADC1)
#define PIN_PH_PASIF    33  // Analog pH - Pasif Panel (ADC1)
#define PIN_LDR_AKTIF   34  // Analog LDR - Aktif Panel (ADC1)
#define PIN_LDR_PASIF   35  // Analog LDR - Pasif Panel (ADC1)

#define PIN_LED_AKTIF   12  // Aydınlatma LED - Aktif Panel
#define PIN_LED_PASIF   13  // Aydınlatma LED - Pasif Panel
#define PIN_POMPA       25  // 12V Hava Pompası Sürücü Çıkışı
#define PIN_VALF        26  // 12V CO2 Solenoid Valf Sürücü Çıkışı

#define PIN_ONE_WIRE    4   // DS18B20 Tek Hat Pin (3 Sensör Bağlı)
#define PIN_BUTTON      14  // Ekran Değiştirme Butonu (Internal Pull-Up)

// --- Küresel Nesneler ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Özel Gülücük Matrisi (Hafıza Yuvası: 1)
byte smiley[8] = {
  B00000, B01010, B01010, B00000, B10001, B01110, B00000, B00000
};

// --- Nesne Yönelimli Sınıf Yapıları (OOP) ---

// 1. Çoklu Sıcaklık Yönetim Sınıfı
class SicaklikSistemi {
private:
    OneWire oneWire;
    DallasTemperature sensors;
public:
    SicaklikSistemi(int pin) : oneWire(pin), sensors(&sensors) {}
    
    void begin() {
        sensors.begin();
    }
    
    // Tek hattan indeks sırasına göre veri okuma
    float oku(int index) {
        sensors.requestTemperatures();
        float temp = sensors.getTempCByIndex(index);
        if (temp == DEVICE_DISCONNECTED_C) return 24.0; // Hata durumunda güvenli sınır
        return temp;
    }
};

// 2. pH Sensör Sınıfı
class PHSensoru {
private:
    int pin;
public:
    PHSensoru(int _pin) : pin(_pin) {}
    
    float oku() {
        int analogVeri = analogRead(pin);
        float voltaj = analogVeri * (3.3 / 4095.0); // 12-bit ADC dönüşümü
        return 3.5 * voltaj; // Örnek kalibrasyon sabiti
    }
};

// 3. Bilimsel Optik Yoğunluk (OD) ve LDR Sınıfı
class OptikYogunlukSensoru {
private:
    int ldrPin;
    int ledPin;
public:
    OptikYogunlukSensoru(int _ldrPin, int _ledPin) : ldrPin(_ldrPin), ledPin(_ledPin) {}
    
    void begin() {
        pinMode(ledPin, OUTPUT);
        digitalWrite(ledPin, LOW);
    }
    
    // LDR verisini gerçek OD (0.2 - 4.25) değerlerine dönüştürür
    float okuOD() {
        digitalWrite(ledPin, HIGH); // Ölçüm için ilgili paneli aydınlat
        delay(40);
        int hamLdr = analogRead(ldrPin);
        digitalWrite(ledPin, LOW);  // Enerji tasarrufu için kapat
        
        // Alg yoğunlaştıkça ışık geçirgenliği azalır, LDR direnci/değeri değişir.
        float odDegeri = 4.25 - ((hamLdr / 4095.0) * (4.25 - 0.2));
        if (odDegeri < 0.2) odDegeri = 0.2;
        if (odDegeri > 4.25) odDegeri = 4.25;
        
        return odDegeri;
    }
};

// --- Modül Sınıf Tanımlamaları ---
SicaklikSistemi termometreler(PIN_ONE_WIRE);
PHSensoru phAktif(PIN_PH_AKTIF);
PHSensoru phPasif(PIN_PH_PASIF);
OptikYogunlukSensoru odAktif(PIN_LDR_AKTIF, PIN_LED_AKTIF);
OptikYogunlukSensoru odPasif(PIN_LDR_PASIF, PIN_LED_PASIF);

// --- Değişkenler ve Durum Yönetimi ---
bool aktifPanelGosterilsin = true; 
unsigned long oncekiZaman = 0;
const unsigned long LOG_PERIYOT = 3000; // 3 saniyede bir kontrol ve genel log

// --- Hava Pompası Zamanlama Parametreleri ---
const unsigned long POMPA_ACIK_SURE = 300000;  // 5 Dakika Çalışma (Milisaniye)
const unsigned long POMPA_KAPALI_SURE = 300000; // 5 Dakika Dinlenme (Milisaniye)
unsigned long sonPompaGecisZamani = 0;
bool pompaZamanlayiciDurumu = true;             // Zaman döngüsüne göre açık mı kapalı mı?

// --- Solenoid Valf Hysteresis (Histerezis) Parametreleri ---
const float PH_UST_ESIK = 7.60;  // pH bu değeri aşarsa valf açılır (CO2 verilir)
const float PH_ALT_ESIK = 7.20;  // pH bu değerin altına düşerse valf kapatılır
bool valfDurumu = false;         // Anlık valf açık/kapalı durumu

int butonDurumu;
int sonButonDurumu = HIGH;
unsigned long sonButonZamani = 0;
unsigned long debounceGecikmesi = 50;

void setup() {
    Serial.begin(115200);
    
    // LCD Başlatma
    lcd.init();
    lcd.backlight();
    lcd.createChar(1, smiley);
    
    // Aktüatör ve Buton Girişleri
    pinMode(PIN_POMPA, OUTPUT);
    pinMode(PIN_VALF, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    
    // İlk Güvenlik Durumu (Sistem kapalı)
    digitalWrite(PIN_POMPA, LOW);
    digitalWrite(PIN_VALF, LOW);
    
    // Sensörleri Başlat
    termometreler.begin();
    odAktif.begin();
    odPasif.begin();
    
    // Açılış Ekranı
    lcd.setCursor(2, 0);
    lcd.print("SOHAL BIYOPANEL");
    lcd.setCursor(7, 1);
    lcd.write(1);
    delay(3000);
    lcd.clear();
    
    sonPompaGecisZamani = millis();
}

void loop() {
    unsigned long simdikiZaman = millis();
    
    // --- 1. Buton Kontrolü ve Debounce (Ekran Değiştirme) ---
    int okuma = digitalRead(PIN_BUTTON);
    if (okuma != sonButonDurumu) {
        sonButonZamani = simdikiZaman;
    }
    
    if ((simdikiZaman - sonButonZamani) > debounceGecikmesi) {
        if (okuma != butonDurumu) {
            butonDurumu = okuma;
            if (butonDurumu == LOW) { // Butona basıldıysa durumu tersine çevir
                aktifPanelGosterilsin = !aktifPanelGosterilsin;
                lcd.clear(); // Ekran geçişinde kalıntıları temizle
            }
        }
    }
    sonButonDurumu = okuma;
    
    // --- 2. Bağımsız Hava Pompası Zamanlayıcı Mekanizması (Arka Plan Kontrolü) ---
    if (pompaZamanlayiciDurumu && (simdikiZaman - sonPompaGecisZamani >= POMPA_ACIK_SURE)) {
        pompaZamanlayiciDurumu = false;
        sonPompaGecisZamani = simdikiZaman;
        Serial.println("\n[ANLIK LOG] >> Pompa çalışma süresi doldu. Dinlenme moduna geçiliyor...");
    } 
    else if (!pompaZamanlayiciDurumu && (simdikiZaman - sonPompaGecisZamani >= POMPA_KAPALI_SURE)) {
        pompaZamanlayiciDurumu = true;
        sonPompaGecisZamani = simdikiZaman;
        Serial.println("\n[ANLIK LOG] >> Dinlenme süresi doldu. Pompa tekrar aktif ediliyor...");
    }
    
    // --- 3. Periyodik Sensör Okuma, Kontrol ve Loglama Döngüsü ---
    if (simdikiZaman - oncekiZaman >= LOG_PERIYOT) {
        oncekiZaman = simdikiZaman;
        
        // Tüm Sensör Verilerini Çekelim
        float tOrtam = termometreler.oku(0);
        float tAktif = termometreler.oku(1);
        float tPasif = termometreler.oku(2);
        
        float pH_A = phAktif.oku();
        float pH_P = phPasif.oku();
        
        float od_A = odAktif.okuOD();
        float od_P = odPasif.okuOD();
        
        // --- Solenoid Valf Algoritması (Hysteresis Modeli) ---
        if (pH_A >= PH_UST_ESIK && !valfDurumu) {
            valfDurumu = true;
            Serial.println("\n[ANLIK LOG] >> [UYARI] pH Üst Sınırı Aşıldı! CO2 Valfi AÇILDI.");
        } 
        else if (pH_A <= PH_ALT_ESIK && valfDurumu) {
            valfDurumu = false;
            Serial.println("\n[ANLIK LOG] >> [BİLGİ] pH İdeal Seviyeye Düştü. CO2 Valfi KAPATILDI.");
        }
        
        // --- Hava Pompası Nihai Karar Mekanizması ---
        // Pompa, zamanlayıcı fazı AÇIK olduğunda VE Optik Yoğunluk hasat sınırına (3.8) gelmediğinde çalışır.
        bool pompaNihaiGereksinim = (od_A < 3.8) && pompaZamanlayiciDurumu;
        
        // Aktüatörleri Fiziksel Olarak Tetikleme
        digitalWrite(PIN_VALF, valfDurumu ? HIGH : LOW);
        digitalWrite(PIN_POMPA, pompaNihaiGereksinim ? HIGH : LOW);
        
        // --- 4. LCD Ekran Güncellemesi ---
        if (aktifPanelGosterilsin) {
            lcd.setCursor(0, 0);
            lcd.print("A:"); lcd.print(tAktif, 1); lcd.print((char)223); lcd.print("C ");
            lcd.print("pH:"); lcd.print(pH_A, 2);
            
            lcd.setCursor(0, 1);
            lcd.print("OD600: "); lcd.print(od_A, 2);
            lcd.setCursor(15, 1);
            lcd.write(1);
        } else {
            lcd.setCursor(0, 0);
            lcd.print("P:"); lcd.print(tPasif, 1); lcd.print((char)223); lcd.print("C ");
            lcd.print("pH:"); lcd.print(pH_P, 2);
            
            lcd.setCursor(0, 1);
            lcd.print("OD600: "); lcd.print(od_P, 2);
            lcd.setCursor(15, 1);
            lcd.print("P");
        }
        
        // --- 5. Gelişmiş Seri Port Terminal Log Çıktısı ---
        Serial.println("==========================================");
        Serial.print("TIME        : "); Serial.print(simdikiZaman / 1000); Serial.println(" s");
        Serial.print("MODE        : "); Serial.println("AUTO (Dual Panel Tracker)");
        Serial.print("AMBIENT TEMP: "); Serial.print(tOrtam, 1); Serial.println(" C");
        Serial.println("------------------------------------------");
        
        // Aktif Panel Bilgileri
        Serial.print("[AKTIF] Temp: "); Serial.print(tAktif, 1);
        Serial.print(" C | pH: "); Serial.print(pH_A, 2);
        Serial.print(" | Optic Density (OD): "); Serial.println(od_A, 2);
        
        // Pasif Panel Bilgileri
        Serial.print("[PASIF] Temp: "); Serial.print(tPasif, 1);
        Serial.print(" C | pH: "); Serial.print(pH_P, 2);
        Serial.print(" | Optic Density (OD): "); Serial.println(od_P, 2);
        Serial.println("------------------------------------------");
        
        // Aktüatör ve Algoritma Takip Logları
        Serial.print("CO2 VALV (V)  : "); 
        Serial.print(valfDurumu ? "ON (Injecting CO2)" : "OFF (Monitoring)");
        Serial.print(" [Sınırlar: "); Serial.print(PH_ALT_ESIK); Serial.print(" - "); Serial.print(PH_UST_ESIK); Serial.println("]");
        
        Serial.print("AIR PUMP (P)  : "); 
        Serial.print(pompaNihaiGereksinim ? "ON (Mixing)" : "OFF");
        Serial.print(" [Faz: "); Serial.print(pompaZamanlayiciDurumu ? "ACIK (Çalışma)" : "KAPALI (Dinlenme)");
        Serial.println("]");
        Serial.println("==========================================");
        Serial.println();
    }
}