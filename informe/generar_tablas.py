"""Genera las tablas LaTeX del informe a partir de los resultados del benchmark.

Uso (desde la raiz del repo, despues de correr `make bench`):
    python informe/generar_tablas.py

Lee:
    resultados/benchmark.csv          promedios por configuracion
    resultados/benchmark_detalle.csv  cada medicion individual
    resultados/fps_screensaver.txt    FPS del screensaver real (opcional)
Escribe en informe/generado/:
    resumen.tex, bitacora.tex, speedup.tex, etapas.tex, fps.tex
"""

import csv
import re
from collections import defaultdict
from pathlib import Path

RAIZ = Path(__file__).resolve().parent.parent
RESULTADOS = RAIZ / "resultados"
SALIDA = RAIZ / "informe" / "generado"


def leer_csv(nombre):
    with open(RESULTADOS / nombre, newline="", encoding="utf-8") as f:
        return list(csv.DictReader(f))


def etiqueta(fila):
    if fila["version"] == "secuencial":
        return "Secuencial"
    return f"OpenMP {fila['hilos']}h"


def tabla_resumen(filas):
    lineas = [
        r"\begin{tabular}{rlrrrrrr}",
        r"\toprule",
        r"$N$ & Versión & Hilos & ms/frame & Desv. & $S$ (sec) & $S$ (1h) & $E$ \\",
        r"\midrule",
    ]
    n_anterior = None
    for f in filas:
        if n_anterior is not None and f["n"] != n_anterior:
            lineas.append(r"\midrule")
        n_anterior = f["n"]
        hilos = f["hilos"] or "--"
        s1h = f"{float(f['speedup_vs_1hilo']):.2f}" if f["speedup_vs_1hilo"] else "--"
        lineas.append(
            f"{f['n']} & {f['version']} & {hilos} & {float(f['ms_promedio']):.3f} & "
            f"{float(f['ms_desviacion']):.3f} & {float(f['speedup_vs_secuencial']):.2f} & "
            f"{s1h} & {float(f['eficiencia']):.2f} \\\\"
        )
    lineas += [r"\bottomrule", r"\end{tabular}"]
    return "\n".join(lineas)


def tabla_etapas(filas):
    lineas = [
        r"\begin{tabular}{rlrrrr}",
        r"\toprule",
        r"$N$ & Versión & Mover (ms) & Detectar (ms) & Aplicar (ms) & \% detectar \\",
        r"\midrule",
    ]
    for f in filas:
        if f["version"] != "secuencial" and f["hilos"] != "8":
            continue
        total = float(f["ms_promedio"])
        detectar = float(f["ms_detectar"])
        lineas.append(
            f"{f['n']} & {etiqueta(f)} & {float(f['ms_mover']):.3f} & {detectar:.3f} & "
            f"{float(f['ms_aplicar']):.3f} & {100 * detectar / total:.1f}\\% \\\\"
        )
    lineas += [r"\bottomrule", r"\end{tabular}"]
    return "\n".join(lineas)


def grafica_speedup(filas):
    """Coordenadas para pgfplots: una serie por N (speedup vs secuencial)."""
    series = defaultdict(list)
    for f in filas:
        if f["version"] == "openmp":
            series[f["n"]].append((int(f["hilos"]), float(f["speedup_vs_secuencial"])))
    partes = []
    for n, puntos in series.items():
        coords = " ".join(f"({h},{s:.3f})" for h, s in puntos)
        partes.append(f"\\addplot+[mark=*] coordinates {{{coords}}};\n\\addlegendentry{{$N={n}$}}")
    return "\n".join(partes)


def tabla_bitacora(detalle, resumen):
    """Una tabla por N: cada columna es una prueba (version) y cada fila una
    medicion individual; al final van promedio, speedup y eficiencia."""
    por_prueba = defaultdict(list)
    for d in detalle:
        por_prueba[(d["n"], d["version"], d["hilos"])].append(float(d["ms_total"]))
    metricas = {(f["n"], f["version"], f["hilos"]): f for f in resumen}

    bloques = []
    for n in dict.fromkeys(k[0] for k in por_prueba):
        pruebas = [k for k in por_prueba if k[0] == n]
        nombres = ["Secuencial" if k[1] == "secuencial" else f"OMP {k[2]}h" for k in pruebas]
        reps = max(len(por_prueba[k]) for k in pruebas)

        lineas = [
            r"\begin{table}[H]",
            r"\centering\small",
            f"\\caption{{Bitácora de mediciones, $N={n}$ (ms por frame de simulación)}}",
            f"\\begin{{tabular}}{{l{'r' * len(pruebas)}}}",
            r"\toprule",
            " & ".join(["Medición"] + nombres) + r" \\",
            r"\midrule",
        ]
        for i in range(reps):
            celdas = [f"{por_prueba[k][i]:.3f}" if i < len(por_prueba[k]) else "" for k in pruebas]
            lineas.append(" & ".join([f"M{i + 1}"] + celdas) + r" \\")
        lineas.append(r"\midrule")
        for titulo, campo in [("Promedio", "ms_promedio"), ("Desv. estándar", "ms_desviacion"),
                              ("Speedup $S$", "speedup_vs_secuencial"), ("Eficiencia $E$", "eficiencia")]:
            celdas = [f"{float(metricas[k][campo]):.3f}" for k in pruebas]
            lineas.append(" & ".join([titulo] + celdas) + r" \\")
        lineas += [r"\bottomrule", r"\end{tabular}", r"\end{table}"]
        bloques.append("\n".join(lineas))
    return "\n\n".join(bloques)


def tabla_fps():
    ruta = RESULTADOS / "fps_screensaver.txt"
    if not ruta.exists():
        return r"\textit{(No se encontró resultados/fps\_screensaver.txt)}"
    patron = re.compile(r"FPS promedio: ([\d.]+).*secuencial: ([\d.]+), openmp: ([\d.]+)\s+n=(\d+) zona=(\d+)")
    datos = defaultdict(dict)
    for linea in ruta.read_text(encoding="utf-8").splitlines():
        m = patron.search(linea)
        if m:
            fps, ms_sec, ms_par, n, zona = m.groups()
            datos[int(n)][int(zona)] = (float(fps), float(ms_sec), float(ms_par))
    lineas = [
        r"\begin{tabular}{rrrrrr}",
        r"\toprule",
        r"& \multicolumn{2}{c}{Secuencial (zona 0\%)} & \multicolumn{2}{c}{OpenMP (zona 100\%)} & \\",
        r"\cmidrule(lr){2-3}\cmidrule(lr){4-5}",
        r"$N$ & FPS & ms sim. & FPS & ms sim. & Mejora FPS \\",
        r"\midrule",
    ]
    for n in sorted(datos):
        sec = datos[n].get(0)
        par = datos[n].get(100)
        if not sec or not par:
            continue
        lineas.append(f"{n} & {sec[0]:.1f} & {sec[1]:.2f} & {par[0]:.1f} & {par[2]:.2f} & "
                      f"{par[0] / sec[0]:.2f}$\\times$ \\\\")
    lineas += [r"\bottomrule", r"\end{tabular}"]
    return "\n".join(lineas)


def main():
    SALIDA.mkdir(parents=True, exist_ok=True)
    resumen = leer_csv("benchmark.csv")
    detalle = leer_csv("benchmark_detalle.csv")

    archivos = {
        "resumen.tex": tabla_resumen(resumen),
        "etapas.tex": tabla_etapas(resumen),
        "speedup.tex": grafica_speedup(resumen),
        "bitacora.tex": tabla_bitacora(detalle, resumen),
        "fps.tex": tabla_fps(),
    }
    for nombre, contenido in archivos.items():
        (SALIDA / nombre).write_text(contenido + "\n", encoding="utf-8")
        print(f"escrito informe/generado/{nombre}")


if __name__ == "__main__":
    main()
