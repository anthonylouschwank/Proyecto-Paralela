#pragma once

// Constantes globales del screensaver. Todo lo que sea "numero magico"
// (tamanos, velocidades, colores) vive aqui para poder ajustarlo en un solo lugar.
namespace config {

// Ventana
constexpr int kAnchoVentana = 640;
constexpr int kAltoVentana = 480;
constexpr const char* kTituloVentana = "Screensaver de Chuchos";

// Sprite del chucho (cuadrado)
constexpr int kTamSprite = 64;

// Rango valido para la esquina superior izquierda de cada sprite:
// x en [0, 576], y en [0, 416].
constexpr float kMinX = 0.0f;
constexpr float kMinY = 0.0f;
constexpr float kMaxX = static_cast<float>(kAnchoVentana - kTamSprite);
constexpr float kMaxY = static_cast<float>(kAltoVentana - kTamSprite);

// Rapidez inicial de cada chucho, en pixeles por segundo.
constexpr float kRapidezMin = 60.0f;
constexpr float kRapidezMax = 240.0f;

// Cantidad de chuchos si no se pasa N por linea de comandos.
constexpr int kNPorDefecto = 50;

// Tope para dt (segundos). Evita saltos enormes si un frame se atrasa
// (por ejemplo, al arrastrar la ventana).
constexpr float kDtMaximo = 0.05f;

// Color de fondo (RGB)
constexpr unsigned char kFondoR = 28;
constexpr unsigned char kFondoG = 28;
constexpr unsigned char kFondoB = 26;

}  // namespace config
