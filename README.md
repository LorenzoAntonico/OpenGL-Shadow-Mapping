

##  Requisiti

- **Sistema**: Windows  
- **Compilatore**: MinGW (32-bit consigliato)

##  Librerie utilizzate

- **GLFW** → `libglfw3.a`
- **GLAD** → `glad.c` + header
- **stb_image** → caricamento texture
- **GLM** → matematica vettoriale/matriciale

##  Controlli da tastiera

| Tasto | Azione |
|-------|--------|
| W / A / S / D | Movimento della camera |
| P | Aumenta intensità luce **direzionale** |
| O | Diminuisce intensità luce **direzionale** |
| I | Aumenta intensità luce **point** |
| U | Diminuisce intensità luce **point** |
| L | Aumenta intensità luce **spot** |
| K | Diminuisce intensità luce **spot** |
| 1 | Attiva/disattiva **PCF** |
| 2 | Attiva/disattiva **bias** |
| 3 | Attiva/disattiva **PCF per point light** |

##  Istruzioni di compilazione


1. Apri il terminale MSYS MinGW32 nella **cartella root** del progetto
2. Esegui:

```bash
make
