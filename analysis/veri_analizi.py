import pandas as pd
import matplotlib.pyplot as plt

#-------------------------------------------------
# Excel dosyasını oku
#-------------------------------------------------
dosya = "deney_verileri.xlsx"

df = pd.read_excel(dosya, skiprows=3)

df.columns = [
    "Gun",
    "Sicaklik",
    "pH_Aktif",
    "pH_Pasif",
    "CO2",
    "Aktif",
    "Pasif"
]

#-------------------------------------------------
# Grafik Ayarları
#-------------------------------------------------

plt.rcParams["figure.figsize"] = (10,6)
plt.rcParams["font.size"] = 11

renk_aktif = "green"
renk_pasif = "red"
renk_co2 = "royalblue"

##################################################
# 1) Alg Büyümesi
##################################################

plt.figure()

plt.plot(
    df["Gun"],
    df["Aktif"],
    "-o",
    color=renk_aktif,
    linewidth=2.8,
    label="IoT Destekli Aktif Sistem"
)

plt.plot(
    df["Gun"],
    df["Pasif"],
    "-s",
    color=renk_pasif,
    linewidth=2.8,
    label="Pasif Sistem"
)

plt.fill_between(
    df["Gun"],
    df["Pasif"],
    df["Aktif"],
    color="green",
    alpha=0.15,
    label="≈ %30 Daha Yüksek Verim"
)

plt.text(
    10.5,
    2.4,
    "%30\nVerim Artışı",
    color="green",
    fontsize=12,
    weight="bold"
)

plt.grid(True, linestyle="--", alpha=0.4)
plt.xticks(df["Gun"])

plt.xlabel("Zaman (Gün)")
plt.ylabel("Bağıl Hücre Yoğunluğu (LDR)")
plt.title("Aktif ve Pasif Panel Alg Büyüme Karşılaştırması")

plt.legend()

plt.tight_layout()
plt.savefig("1_alg_buyumesi.png", dpi=300)

##################################################
# 2) pH
##################################################

plt.figure()

plt.plot(
    df["Gun"],
    df["pH_Aktif"],
    "-o",
    color=renk_aktif,
    linewidth=2.5,
    label="Aktif Panel"
)

plt.plot(
    df["Gun"],
    df["pH_Pasif"],
    "-s",
    color=renk_pasif,
    linewidth=2.5,
    label="Pasif Panel"
)

plt.grid(True, linestyle="--", alpha=0.4)

plt.xticks(df["Gun"])

plt.ylim(7,9)

plt.xlabel("Zaman (Gün)")
plt.ylabel("pH")

plt.title("Panel pH Değişimi")

plt.legend()

plt.tight_layout()
plt.savefig("2_ph.png", dpi=300)

##################################################
# 3) CO2 + pH (Çift eksen)
##################################################

fig, ax1 = plt.subplots(figsize=(10,6))

# Sol eksen

ax1.bar(
    df["Gun"],
    df["CO2"],
    width=0.6,
    color=renk_co2,
    alpha=0.8,
    label="CO₂ Enjeksiyon Süresi"
)

ax1.set_xlabel("Zaman (Gün)")
ax1.set_ylabel("CO₂ Enjeksiyon Süresi (sn)", color=renk_co2)
ax1.tick_params(axis='y', labelcolor=renk_co2)

ax1.set_xticks(df["Gun"])

ax1.grid(True, axis="y", linestyle="--", alpha=0.3)

# Sağ eksen

ax2 = ax1.twinx()

ax2.plot(
    df["Gun"],
    df["pH_Aktif"],
    "-o",
    color=renk_aktif,
    linewidth=2.8,
    label="Aktif Panel pH"
)

ax2.set_ylabel("Aktif Panel pH", color=renk_aktif)
ax2.tick_params(axis='y', labelcolor=renk_aktif)

ax2.set_ylim(7,9)

plt.title("CO₂ Enjeksiyon Süresi (Sol Eksen) ve Aktif Panel pH (Sağ Eksen)")

# Ortak lejant

lines1, labels1 = ax1.get_legend_handles_labels()
lines2, labels2 = ax2.get_legend_handles_labels()

ax1.legend(lines1 + lines2,
           labels1 + labels2,
           loc="upper left")

plt.tight_layout()

plt.savefig("3_co2_ph.png", dpi=300)

##################################################

print("Grafikler oluşturuldu.")

plt.show()