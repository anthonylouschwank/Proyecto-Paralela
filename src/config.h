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

// Caja envolvente para colisiones. Es mas chica que el sprite (64x64)
// porque el bitmap tiene bordes transparentes: asi los chuchos chocan
// cuando se tocan los dibujos y no cuando se tocan los cuadros vacios.
constexpr float kAnchoCaja = 56.0f;
constexpr float kAltoCaja = 52.0f;

// Color: cada chucho tiene un tono (0-360) y se pinta con esta
// saturacion y brillo, para que todos los colores sean vivos.
constexpr float kSaturacionTinte = 0.55f;
constexpr float kBrilloTinte = 1.0f;

// Al chocar, un chucho gira su tono estos grados (si esta activado).
// Despues de cambiar de color espera kPausaCambioColor segundos antes de
// poder cambiar otra vez; sin esto, con N grande los chuchos chocan casi
// en cada frame y el color parpadea.
constexpr bool kCambiarTinteAlChocar = true;
constexpr float kGiroTonoPorChoque = 30.0f;
constexpr float kPausaCambioColor = 0.3f;

// Cantidad de chuchos si no se pasa N por linea de comandos.
constexpr int kNPorDefecto = 50;

// Tope para dt (segundos). Evita saltos enormes si un frame se atrasa
// (por ejemplo, al arrastrar la ventana).
constexpr float kDtMaximo = 0.05f;

// Color de fondo de la zona secuencial (RGB)
constexpr unsigned char kFondoR = 28;
constexpr unsigned char kFondoG = 28;
constexpr unsigned char kFondoB = 26;

// Zona paralela: franja derecha de la pantalla cuyos chuchos se procesan
// con OpenMP. Su ancho es un porcentaje de la ventana (0 = todo secuencial,
// 100 = todo paralelo) y se ajusta en vivo con las flechas.
constexpr int kZonaPorDefecto = 50;
constexpr int kPasoZona = 10;

// Color de fondo de la zona paralela y de la linea que la separa (RGB)
constexpr unsigned char kFondoZonaR = 20;
constexpr unsigned char kFondoZonaG = 32;
constexpr unsigned char kFondoZonaB = 58;
constexpr unsigned char kBordeZonaR = 70;
constexpr unsigned char kBordeZonaG = 110;
constexpr unsigned char kBordeZonaB = 190;

}  // namespace config
