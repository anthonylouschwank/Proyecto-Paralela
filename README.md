# Proyecto-Paralela

Screensaver de chuchos en C++ con SDL2 — Proyecto 1 de Computación Paralela y Distribuida (UVG).

N chuchos de colores (sprites de 64×64 generados por código) rebotan en una ventana de 640×480.
En la esquina se muestran los FPS y N, que son la métrica para comparar la versión secuencial
contra la paralela (OpenMP).

## Requisitos (Ubuntu / WSL)

```bash
sudo apt install build-essential libsdl2-dev
```

## Compilar y ejecutar

```bash
make                 # genera build/screensaver
make run N=500       # compila y corre con 500 chuchos
./build/screensaver [N] [semilla]
make clean
```

- `N`: cantidad de chuchos (por defecto 50).
- `semilla`: fija la escena inicial para que las corridas se puedan comparar (por defecto, la hora actual).
- Se cierra con la ventana o con `Esc`; al salir imprime el FPS promedio de la corrida.

## Estructura

| Archivo | Responsabilidad |
|---|---|
| `src/config.h` | Constantes: tamaño de ventana y sprite, rango de velocidades, colores. |
| `src/chucho.h/.cpp` | Estado de un chucho y creación aleatoria de los N chuchos. |
| `src/simulacion.h/.cpp` | Paso de simulación: integración de posición y rebote en bordes. |
| `src/sprite.h/.cpp` | Bitmap 16×16 del chucho convertido a textura de 64×64. |
| `src/render.h/.cpp` | Dibujo del fondo y los chuchos (con tinte y volteo según dirección). |
| `src/hud.h/.cpp` | Contador de FPS y HUD con fuente de píxeles propia. |
| `src/main.cpp` | Argumentos, inicialización de SDL y ciclo principal. |

## Avance

- [x] Parte 1: ventana, sprite, movimiento, rebote, N por línea de comandos, HUD de FPS.
- [ ] Parte 2: colisiones entre chuchos (cajas envolventes, intercambio de velocidades).
- [ ] Parte 3: versión paralela con OpenMP y medición de speedup.
