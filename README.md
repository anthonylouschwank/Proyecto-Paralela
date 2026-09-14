# Proyecto-Paralela

Screensaver de chuchos en C++ con SDL2 — Proyecto 1 de Computación Paralela y Distribuida (UVG).

N chuchos de colores (sprites de 64×64 generados por código) rebotan y chocan entre sí en una ventana
de 640×480. La franja derecha de la pantalla (fondo azul) es la **zona paralela**: los chuchos que
están ahí se procesan con OpenMP y los demás en un solo hilo. El HUD muestra los FPS y cuántos
milisegundos tarda cada zona por frame.

## Requisitos (Ubuntu / WSL)

```bash
sudo apt install build-essential libsdl2-dev
```

## Compilar y ejecutar

```bash
make                                          # genera build/screensaver y build/benchmark
make run N=500                                # 500 chuchos, zona paralela al 50%
make run N=2000 ARGS="7 --zona 100 --hilos 4" # semilla 7, todo paralelo, 4 hilos
make bench                                    # benchmark sin ventana -> resultados/*.csv
make clean
```

### Benchmark e informe

`make bench` mide la simulación sin ventana para N = 500, 1000, 2000, 4000 y 1, 2, 4, 8 hilos
(10 repeticiones de 30 frames cada una). Compara la versión secuencial (triángulo `i<j`) contra
OpenMP, calcula speedup y eficiencia, y verifica que el resultado sea idéntico bit a bit. Guarda
`resultados/benchmark.csv` (promedios) y `resultados/benchmark_detalle.csv` (cada medición).
Opciones: `make bench BENCH_ARGS="--n 1000,2000 --hilos 1,4 --reps 10 --frames 30"`.

El informe está en `informe/informe.tex`. Para actualizar sus tablas después de medir:

```bash
python informe/generar_tablas.py
cd informe && pdflatex informe.tex && pdflatex informe.tex
```

Directamente: `./build/screensaver [N] [semilla] [--zona P] [--hilos H]`

- `N`: cantidad de chuchos (por defecto 50).
- `semilla`: fija la escena inicial para que las corridas se puedan comparar (por defecto, la hora actual).
- `--zona P`: porcentaje de la pantalla procesado con OpenMP. `0` = todo secuencial, `100` = todo paralelo (por defecto 50).
- `--hilos H`: hilos de OpenMP (por defecto, los que decida OpenMP).

Teclas: **←** agranda la zona paralela, **→** la achica, **Esc** cierra. Al salir se imprime el FPS
promedio y los ms promedio de cada zona.

## Estructura

| Archivo | Responsabilidad |
|---|---|
| `src/config.h` | Constantes: ventana, sprite, velocidades, cajas de choque, colores, zona. |
| `src/chucho.h/.cpp` | Estado de un chucho y creación aleatoria de los N chuchos. |
| `src/color.h/.cpp` | Conversión de tono (HSV) al tinte RGB de cada chucho. |
| `src/zona.h/.cpp` | Zona paralela: su tamaño y qué chuchos caen dentro. |
| `src/simulacion.h/.cpp` | Paso de simulación: separa los chuchos por zona, los mueve y resuelve choques, midiendo cada zona. |
| `src/colisiones.h/.cpp` | Choques entre chuchos: detección con cajas envolventes (O(N²)) y respuesta en dos fases. |
| `src/estadisticas.h/.cpp` | FPS y ms por zona (actuales y promedio). |
| `src/sprite.h/.cpp` | Bitmap 16×16 del chucho convertido a textura de 64×64. |
| `src/texto.h/.cpp` | Fuente de píxeles propia (sin SDL2_ttf). |
| `src/render.h/.cpp` | Fondo con zonas, chuchos y etiquetas de cada zona. |
| `src/hud.h/.cpp` | Recuadro con FPS, N, hilos, zona y ms por zona. |
| `src/main.cpp` | Argumentos, inicialización de SDL y ciclo principal. |

## Paralelización con OpenMP

Cada etapa usa `#pragma omp parallel for schedule(static) if (paralelo)`: el mismo código corre en
paralelo para la zona paralela y en un solo hilo para la secuencial.

1. **Movimiento y rebote**: cada chucho solo se toca a sí mismo, no hay dependencias.
2. **Detección de choques**: cada chucho revisa a todos los demás y escribe solo en su propia entrada
   del buffer, así no hay condiciones de carrera (sin locks ni atomics). Cuesta N·(N−1) revisiones en
   vez de las N·(N−1)/2 del triángulo `i < j`, pero el triángulo escribe en `i` y en `j` y en paralelo
   eso es una condición de carrera.
3. **Aplicar choques**: cada chucho actualiza solo su propio estado.

El resultado es **idéntico bit a bit** sin importar la zona ni la cantidad de hilos, porque cada
chucho suma a sus compañeros siempre en el mismo orden.

### Notas para medir

- **`OMP_WAIT_POLICY=passive`** (ya viene en `make run`): en WSLg el dibujo corre en la CPU
  (llvmpipe), y con la espera activa por defecto los hilos de OpenMP le roban núcleos al render.
  Si se ejecuta el binario directo, conviene exportar la variable.
- La laptop de desarrollo (i5-1035G1) tiene 4 núcleos físicos y 8 hilos lógicos; con turbo y
  temperatura los tiempos varían, así que conviene repetir cada medición.
- Para calcular el speedup contra el mejor secuencial, hay que tomar en cuenta que el recorrido
  completo secuencial cuesta ~2× lo que costaba el triángulo `i < j` de la Parte 2.

## Avance

- [x] Parte 1: ventana, sprite, movimiento, rebote, N por línea de comandos, HUD de FPS.
- [x] Parte 2: colisiones entre chuchos (cajas envolventes, intercambio de velocidades, cambio de tinte).
- [x] Parte 3: zona paralela con OpenMP, visible en pantalla, con ms por zona en el HUD.
- [x] Benchmark (`make bench`) e informe en LaTeX (`informe/`).
