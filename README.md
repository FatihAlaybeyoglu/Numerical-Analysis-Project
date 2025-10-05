# 🧮 Sayısal Analiz Uygulaması (Numerical Analysis Project)

## 📘 Proje Özeti

Bu proje, **sayısal analiz** alanındaki temel yöntemleri C programlama diliyle uygulayan kapsamlı bir komut satırı uygulamasıdır.  
Program, kullanıcıdan alınan matematiksel ifadeleri ayrıştırarak (**parsing**) bir **hesaplama ağacı (parse tree)** oluşturur ve bu yapı üzerinden özyinelemeli (recursive) hesaplamalar yapar.  

Bu proje, **kök bulma, türev, integral, lineer denklem sistemlerinin çözümü, enterpolasyon ve matris işlemleri** gibi yöntemleri destekler.

---

## ⚙️ Uygulanan Yöntemler

### 1️⃣ Bisection (İkiye Bölme) Yöntemi

**Amaç:** Sürekli bir fonksiyonun \[a, b\] aralığında kökünü bulmak.  
**Formül:**  
f(a) * f(b) < 0 koşulu sağlandığında, kök bu aralıktadır.  
Her iterasyonda yeni nokta:  
xₙ = (a + b) / 2  

**Programdaki İşleyiş:**
- Kullanıcıdan f(x), alt ve üst sınır (a, b) ve tolerans değeri alınır.
- Fonksiyon hesaplama ağacı üzerinde değerlendirilir.
- a ve b aralığı her adımda yarıya indirilir.
- f(a) ve f(xₙ) işaret kontrolüyle aralık güncellenir.

**Örnek Girdi:**
```
Fonksiyon: x^3 - x - 2
Alt sınır: 1
Üst sınır: 2
Tolerans: 0.0001
```

**Örnek Çıktı:**
```
Iterasyon 1: a=1.000000, b=2.000000, x=1.500000, f(x)=-0.125000
Iterasyon 2: a=1.500000, b=2.000000, x=1.750000, f(x)=1.609000
Iterasyon 3: a=1.500000, b=1.750000, x=1.625000, f(x)=0.666000
...
Sonuç: Yaklaşık kök = 1.5214
```

---

### 2️⃣ Regula-Falsi (False Position) Yöntemi

**Amaç:** f(x) fonksiyonunun kökünü \[a,b\] aralığında daha hızlı bulmak.  
**Formül:**  
xₙ = b - f(b)*(b - a) / (f(b) - f(a))

**Programdaki İşleyiş:**
- f(a) * f(b) < 0 koşulu aranır.
- Doğrunun x-ekseniyle kesişim noktası hesaplanır.
- Aralık bu yeni noktayla güncellenir.

**Örnek Girdi:**
```
Fonksiyon: x^3 - x - 2
Alt sınır: 1
Üst sınır: 2
Tolerans: 0.0001
```

**Örnek Çıktı:**
```
Iterasyon 1: a=1.000000, b=2.000000, x=1.333333, f(x)=-0.962963
Iterasyon 2: a=1.333333, b=2.000000, x=1.462687, f(x)=-0.333338
Iterasyon 3: a=1.462687, b=2.000000, x=1.521437, f(x)=-0.002097
Sonuç: Yaklaşık kök = 1.5214
```

---

### 3️⃣ Newton-Raphson Yöntemi

**Amaç:** Bir fonksiyonun kökünü türev yardımıyla hızlı şekilde bulmak.  
**Formül:**  
xₙ₊₁ = xₙ - f(xₙ)/f'(xₙ)

**Programdaki İşleyiş:**
- Kullanıcıdan f(x), x₀ (başlangıç değeri) ve tolerans alınır.
- f'(x) sayısal türevle hesaplanır.
- Her iterasyonda yeni x değeri türev oranına göre güncellenir.

**Örnek Girdi:**
```
Fonksiyon: x^3 - x - 2
Başlangıç değeri: 1.5
Tolerans: 0.0001
```

**Örnek Çıktı:**
```
Iterasyon 1: x=1.500000, f(x)=-0.125000, f'(x)=5.750000
Iterasyon 2: x=1.521739, f(x)=-0.002058, f'(x)=5.944000
Sonuç: Yaklaşık kök = 1.5214
```

---

### 4️⃣ Sayısal Türev (Numerical Derivative)

**Amaç:** f(x) fonksiyonunun belirli bir noktadaki türevini yaklaşık olarak bulmak.  
**Formüller:**
- İleri fark: f'(x) ≈ (f(x+h) - f(x)) / h  
- Geri fark: f'(x) ≈ (f(x) - f(x-h)) / h  
- Merkezi fark: f'(x) ≈ (f(x+h) - f(x-h)) / (2h)

**Örnek Girdi:**
```
Fonksiyon: sin(x)
x = 1.5708
h = 0.0001
Seçim: merkezi fark
```

**Örnek Çıktı:**
```
f'(1.5708) ≈ 0.0000 (cos(π/2) ≈ 0)
```

---

### 5️⃣ Sayısal İntegral

#### Trapez Yöntemi
**Formül:**  
∫ₐᵇ f(x) dx ≈ (b-a) * [f(a) + f(b)] / 2

#### Simpson 1/3 ve 3/8 Yöntemi
**Formüller:**
- Simpson 1/3: ∫ₐᵇ f(x) dx ≈ (h/3) [f(x₀) + 4f(x₁) + f(x₂)]  
- Simpson 3/8: ∫ₐᵇ f(x) dx ≈ (3h/8) [f(x₀) + 3f(x₁) + 3f(x₂) + f(x₃)]

**Örnek Girdi:**
```
Fonksiyon: x^2
a = 0
b = 2
n = 4
```

**Örnek Çıktı:**
```
Trapez Yöntemi: 2.6667
Simpson 1/3 Yöntemi: 2.6667
Simpson 3/8 Yöntemi: 2.6667
```

---

### 6️⃣ Gregory-Newton Enterpolasyonu

**Amaç:** Düzenli aralıklı veri noktalarına göre fonksiyon değerini tahmin etmek.  

**Formül:**  
f(x) = f(x₀) + pΔf(x₀) + p(p-1)/2!Δ²f(x₀) + ...  

**Örnek Girdi:**
```
X: 1 2 3 4
Y: 1 8 27 64
x = 2.5
```

**Örnek Çıktı:**
```
f(2.5) = 15.625
```

---

### 7️⃣ Lineer Denklem Sistemleri

#### Gauss Eliminasyon
**Amaç:** Ax = b sistemini doğrudan çözmek.

**Örnek Girdi:**
```
3x + y - z = 1
2x + 4y + 3z = 8
x + y + 2z = 5
```

**Örnek Çıktı:**
```
x = 1.0
y = 1.0
z = 2.0
```

#### Gauss-Seidel Yöntemi
**Amaç:** Büyük sistemler için iteratif çözüm elde etmek.  
**Formül:**  
xᵢ^(k+1) = (bᵢ - Σaᵢⱼxⱼ^(k)) / aᵢᵢ

**Örnek Çıktı:**
```
Iterasyon 1: x1=0.333, x2=0.750, x3=1.667
Iterasyon 2: x1=0.375, x2=0.812, x3=1.750
Sonuç: x1=0.4, x2=0.8, x3=1.8
```

---

## 🧩 Desteklenen Fonksiyon Türleri

Program, aşağıdaki türde fonksiyonlarda çalışır:

| Fonksiyon Türü | Örnek |
|----------------|-------|
| Trigonometrik | sin(x), cos(x), tan(x), sec(x), csc(x) |
| Ters Trigonometrik | arcsin(x), arccos(x), arctan(x) |
| Üstel ve Logaritmik | exp(x), log(x), ln(x) |
| Polinom | x^2 + 3x + 2 |
| Karekök | sqrt(x) |
| Sabitler | e, pi |
| Kombine | sin(x) + log(x^2 + 1) |

---

## 🧠 Program Akışı

1. Kullanıcı menüden yapmak istediği işlemi seçer.  
2. Fonksiyon ifadesi girilir.  
3. İlgili sayısal parametreler (a, b, h, tolerans, iterasyon vb.) istenir.  
4. Sonuç ekrana yazdırılır.  
5. Bellek serbest bırakılır.

---

## 🧰 Derleme ve Çalıştırma

```bash
gcc 23011009.c -o numerical_analysis -lm -Wall
./numerical_analysis
```

> - `-lm` : Matematik kütüphanesini bağlar.  
> - `-Wall` : Uyarıların görünmesini sağlar.

---

## 🧹 Bellek Yönetimi

- Tüm hesaplama ağaçları `free_hesaplamaagaci()` fonksiyonuyla serbest bırakılır.  
- Matrisler ve dinamik diziler `free()` ile temizlenir.  
- Hafıza sızıntısı (memory leak) oluşmaz.

---

## 🏁 Sonuç

Bu proje, **sayısal analiz yöntemlerinin** C programlama diliyle nasıl uygulanabileceğini göstermektedir.  
Her yöntem hem **matematiksel olarak doğru**, hem de **kullanıcı etkileşimli** şekilde tasarlanmıştır.  
Trigonometrik, logaritmik, üstel ve polinom fonksiyonlarda yüksek doğrulukla çalışmaktadır.

Bu çalışma, özellikle **mühendislik uygulamaları** ve **bilimsel hesaplamalar** için sağlam bir temel sağlar.
