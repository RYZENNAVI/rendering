# Mathematik des Farbmischens
## 1. Farbrepräsentation im RGB-Modell
Farben werden im RGB-Modell durch Tupel von Ganzzahlen im Bereich von [0, 255] für jeden Farbkanal dargestellt.

Beispiel:  
**Rot = (255, 0, 0)**

Gegeben:  
Farbe₁ = (R₁, G₁, B₁)
Farbe₂ = (R₂, G₂, B₂)

## 2. Einfaches digitales Farbmischen
Eine einfache Methode zum Mischen zweier Farben besteht darin, den Durchschnitt ihrer einzelnen RGB-Komponenten zu berechnen:

Gemischte Farbe = ((R₁ + R₂)/2, (G₁ + G₂)/2, (B₁ + B₂)/2)

## 3. Farbmodell mit Transparenz (RGBA)
Zur Darstellung von Transparenz wird das RGBA-Modell verwendet:

A (Alpha): Transparenzwert im Bereich [0, 1] (0 = vollständig transparent, 1 = vollständig deckend)

### Alpha-Komposition
Dabei wird berücksichtigt, wie „durchsichtig“ eine Farbe ist, wenn sie über eine andere gelegt wird.

**Farbe A** (Vordergrundfarbe, z. B. ein halbtransparentes Objekt)

**Farbe B** (Hintergrundfarbe)

**αA** ist der Alphawert von Farbe A, wobei _αA ∈ [0,1]_

Wenn eine Farbe (A) über eine andere Farbe (B) gelegt wird, ergibt sich das Ergebnis wie folgt:

Ergebnis = αA × A + (1 − αA) × B  
(Pro Farbkanal)

## 4. Alternative Mischtechniken
### Gewichtete Mischung
**Kombinieren zwei Farben anteilig. Anders als beim einfachen Mittelwert berücksichtigt diese Methode eine **Gewichtung** zwischen den beiden Farben.**

**w1: Gewicht für Farbe 1**

**w2: Gewicht für Farbe 2**

**Gewichtete Mischung mit w₁ + w₂ = 1:**  
**Ergebnis = (w₁ × R₁ + w₂ × R₂, w₁ × G₁ + w₂ × G₂, w₁ × B₁ + w₂ × B₂)**

### Gamma-Korrektur
Die direkte Mischung von RGB-Werten ist visuell oft nicht korrekt, da RGB-Werte normalerweise gamma-kodiert sind. Eine korrekt gamma-korrigierte Mischung berücksichtigt dies durch folgende Schritte:

1. Normierung: RGB-Werte in den Bereich 0.0 – 1.0 umwandeln (durch 255 teilen).

2. Gamma-Dekodierung: Jedes Ergebnis wird auf lineares Licht umgerechnet, typischerweise mit einem Exponenten von 2.2:  
\( L = (Wert)^{2.2} \)

3. Mischung:
- Ungewichtet (50/50): Mittelwert der linearen Farbwerte.
- Gewichtet: lineare Werte mit den jeweiligen Gewichten multiplizieren und summieren.

4. Gamma-Kodierung: Rückumwandlung der Werte mit \( L^{1/2.2} \) und anschließender Multiplikation mit 255.

**Beispiel**  
Farbe 1 (Gelb): (255, 255, 0) → (1.0, 1.0, 0.0)  
Farbe 2 (Rot): (255, 0, 0) → (1.0, 0.0, 0.0)  

50/50 Mischung:
→ lineare Mischung: (1.0, 0.5, 0.0)  
→ gamma-kodiert: \( (1.0^{1/2.2}, 0.5^{1/2.2}, 0.0^{1/2.2}) × 255 ≈ (255, 186, 0) \)

Gewichtete Mischung (25% Gelb, 75% Rot): 
→Linear gemischt: (1.0, 0.25, 0.0)  
→ gamma-kodiert: \( (1.0^{1/2.2}, 0.25^{1/2.2}, 0.0^{1/2.2}) × 255 ≈ (255, 136, 0) \)

Diese Methode liefert realistischere Farbmischungen auf digitalen Displays.

## 5. Übergang zwischen zwei Farben (LERP)
Lineare Interpolation (LERP) stufenlose Übergänge zwischen zwei Farben basierend auf einem Verhältnis t ∈ [0, 1]:

Interpolierte Farbe = (1 − t) × Farbe₁ + t × Farbe₂

Pro Kanal:  
R = (1 − t) × R₁ + t × R₂  
G = (1 − t) × G₁ + t × G₂  
B = (1 − t) × B₁ + t × B₂  
A = (1 − t) × A₁ + t × A₂ (bei Verwendung von Alpha)

### Erweiterung: Mehrstufige Interpolation (Farbverläufe)
LERP kann verwendet werden, um kontinuierliche **Farbverläufe** (Gradients) zwischen mehreren Farben zu erzeugen. Dies geschieht, indem man t schrittweise von 0 bis 1 erhöht.

***Beispiel: Drei Farben***

Farbverlauf von Farbe₁ → Farbe₂ → Farbe₃

_t ∈ [0,0.5]:_ interpolieren zwischen Farbe₁ und Farbe₂

_t ∈ [0,5,1]:_ interpolieren zwischen Farbe₂ und Farbe₃ (umgerechnet)

