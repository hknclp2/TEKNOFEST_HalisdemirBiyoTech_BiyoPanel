# Halisdemir BiyoTech - Akıllı Çift Fotobiyoreaktör Otomasyon Sistemi 🌿🤖

Bu depo, **TEKNOFEST 2026** Çevre ve Enerji Teknolojileri Yarışması kapsamında geliştirilen **"Halisdemir BiyoTech"** projesinin nesne yönelimli (OOP) gömülü yazılım, otomasyon kontrol ve veri analizi kodlarını içermektedir. Proje, dikey modüler mikroalg panelleri ve IoT altyapısı kullanarak kentsel alanlardaki karbondioksit ($CO_2$) emisyonlarını biyolojik olarak yakalamayı hedeflemektedir.

## 🚀 Proje Mimari Yaklaşımı
Sistem, bilimsel kontrol grubu mantığına dayanarak **Aktif (Müdahale Edilen)** ve **Pasif (Doğal Akışına Bırakılan)** olmak üzere çift panel yapısıyla tasarlanmıştır. Sistemdeki tüm döngüler, milisaniye tabanlı donma yapmayan (*non-blocking*) mimariyle kodlanmış olup, gücünü tamamen entegre fotovoltaik (güneş) panellerinden alarak şebekeden bağımsız (off-grid) çalışır.

## 🛠️ Donanım Mimarisi ve Pin Şeması
Prototip üretiminde kullanılan bileşenlerin ESP32 tabanlı **Deneyap Kart** üzerindeki bağlantı şeması aşağıdaki gibidir:

| Bileşen / Sensör | Pin | Görevi |
| :--- | :---: | :--- |
| **Analog pH Sensörü (Aktif)** | `32` | Aktif panel asitlik ve dolaylı $CO_2$ doymuşluk takibi |
| **Analog pH Sensörü (Pasif)** | `33` | Pasif panel (kontrol grubu) anlık durum takibi |
| **Analog LDR Sensör (Aktif)** | `34` | Aktif panel Hücre Yoğunluğu ($OD_{600}$) ölçümü |
| **Analog LDR Sensör (Pasif)** | `35` | Pasif panel Hücre Yoğunluğu ($OD_{600}$) ölçümü |
| **Aydınlatma LED'i (Aktif)** | `12` | Aktif panel OD ölçüm ışık kaynağı |
| **Aydınlatma LED'i (Pasif)** | `13` | Pasif panel OD ölçüm ışık kaynağı |
| **12V Hava Pompası Sürücüsü**| `25` | Kültür sirkülasyonu ve periyodik havalandırma |
| **12V $CO_2$ Solenoid Valf** | `26` | pH'a bağlı otomatik karbondioksit gaz enjeksiyonu |
| **DS18B20 Sıcaklık Sensörleri**| `4`  | Tek hat (*One-Wire*) üzerinden 3 farklı bölgenin sıcaklık takibi |
| **Ekran Değiştirme Butonu**  | `14` | Aktif/Pasif panel verileri arasında LCD geçiş butonu |

## 📊 Gelişmiş Kontrol Algoritmaları ve Takip Logları

*   **Histerezis (Hysteresis) Tabanlı pH Regülasyonu:** Solenoid valfin tek bir değerde sürekli açılıp kapanarak salınım yapmasını ve mekanik olarak yıpranmasını önlemek amacıyla akıllı üst limit (7.60) ve alt limit (7.20) modeli uygulanmıştır. pH 7.60'ı aşınca açılan valf, sistem nötrleşip 7.20'ye inene kadar açık kalır.
*   **Non-Blocking Hava Pompası Zamanlayıcısı (Duty Cycle):** Hava pompasının devamlı açık kalarak aşırı hava çekmesini engellemek amacıyla arka planda bağımsız çalışan bir zamanlayıcı kurulmuştur. Pompa `delay()` kullanılmadan 5 dakika çalışıp 5 dakika dinlenecek şekilde optimize edilmiş ve her geçiş anlık olarak seri port terminaline loglanmaktadır.
*   **Bilimsel Optik Yoğunluk ($OD_{600}$) Dönüşümü:** Ham analog LDR sinyalleri, alglerin klorofil yapısının ışık absorbe etme katsayılarına göre kalibre edilerek doğrudan bilimsel $OD_{600}$ (0.2 - 4.25) değerlerine dönüştürülür.

## 📂 Depo İçeriği
*   📁 **`src/`**
    *   `main.ino`: Deneyap Kart üzerinde koşan, OOP yapısında yazılmış ana otomasyon ve histerezis kontrol kodu.
*   📁 **`analysis/`**
    *   📁 **`grafik/`**: Sistemden elde edilen ham verilerin görselleştirildiği performans grafikleri (Zaman-pH, Zaman-Optik Yoğunluk grafikleri).
    *   `data_visualizer.py`: Ham verileri işleyerek `grafik/` klasöründeki görsel çıktıları üreten veri analizi Python (Matplotlib) kodu.
    *   `DeneyVerileri.xlsx`: Matematiksel modelleme ve sensör kalibrasyon veri seti tablosu.

## 🧑‍💻 Kurulum ve Çalıştırma
1.  `src/main.ino` kodunu bilgisayarınıza indirin.
2.  Arduino IDE ortamında Kart Yöneticisine **Deneyap Kart** kütüphanesini ekleyin.
3.  Gerekli sensör kütüphanelerini (`DallasTemperature`, `LiquidCrystal_I2C`) IDE'ye dahil edin.
4.  Bağlantı şemasına göre devrenizi kurup kodu kartınıza yükleyin. Seri Port ekranını `115200 baud` hızında açarak sistem loglarını anlık takip edebilirsiniz.

---
*Bu proje, Etimesgut Şehit Ömer Halisdemir Anadolu Lisesi bünyesindeki projelendirme çalışmaları kapsamında TEKNOFEST 2026 yarışması için geliştirilmiştir.*s
