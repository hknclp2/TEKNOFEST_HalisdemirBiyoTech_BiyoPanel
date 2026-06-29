# Alg Paneli Veri Analizi

Bu proje, **Excel dosyasındaki deney verilerini okuyarak** üç farklı grafik oluşturur.

## Oluşturulan Grafikler

- **1_alg_buyumesi.png**
  - Aktif ve Pasif panel alg büyümesi karşılaştırması

- **2_ph.png**
  - Aktif ve Pasif panel pH değişimi

- **3_co2_ph.png**
  - CO₂ enjeksiyon süresi ve Aktif panel pH grafiği (çift eksen)

---

# Gereksinimler

- Python **3.10 veya üzeri** önerilir.

Gerekli kütüphaneler:

- pandas
- matplotlib
- openpyxl

---

# Kurulum

Önce depoyu indirin veya dosyaları aynı klasöre koyun.

Daha sonra terminal (CMD / PowerShell / Terminal) açıp aşağıdaki komutu çalıştırın.

```bash
pip install pandas matplotlib openpyxl
```

Alternatif olarak Python Launcher kullanıyorsanız:

```bash
py -m pip install pandas matplotlib openpyxl
```

Linux / macOS:

```bash
python3 -m pip install pandas matplotlib openpyxl
```

---

# Dosya Yapısı

```
proje/
│
├── deney_verileri.xlsx
├── grafikler.py
├── README.md
```

> **Not:** Excel dosyasının adı **deney_verileri.xlsx** olmalıdır.

---

# Çalıştırma

Windows:

```bash
python grafikler.py
```

veya

```bash
py grafikler.py
```

Linux / macOS:

```bash
python3 grafikler.py
```

---

# Çıktılar

Program başarıyla çalıştığında aynı klasörde aşağıdaki dosyalar oluşacaktır.

```
1_alg_buyumesi.png
2_ph.png
3_co2_ph.png
```

Ayrıca ekranda şu mesaj görüntülenir:

```
Grafikler oluşturuldu.
```

ve grafikler ayrı pencerelerde açılır.

---

# Olası Hatalar

## ModuleNotFoundError

Örneğin:

```
ModuleNotFoundError: No module named 'pandas'
```

Çözüm:

```bash
pip install pandas matplotlib openpyxl
```

---

## FileNotFoundError

```
FileNotFoundError: deney_verileri.xlsx
```

Çözüm:

- Excel dosyasının proje klasöründe olduğundan emin olun.
- Dosya adının tam olarak

```
deney_verileri.xlsx
```

olduğunu kontrol edin.

---

## Excel okunamıyor

Eğer şu hatayı alırsanız:

```
ImportError: Missing optional dependency 'openpyxl'
```

Çözüm:

```bash
pip install openpyxl
```

---

# Kullanılan Kütüphaneler

- pandas
- matplotlib
- openpyxl

---

# Lisans

Bu proje eğitim ve araştırma amaçlı hazırlanmıştır.