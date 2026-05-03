/**
 * @file ResistiveSoilSensor.h
 * @brief Bibliothèque pour capteur d'humidité du sol résistif (YL-69 / FC-28)
 * @version 2.0.0
 * 
 * Cette bibliothèque gère correctement les deux configurations de montage :
 * - PULL_UP : Capteur côté GND (montage classique YL-69)
 * - PULL_DOWN : Capteur côté VCC
 */

#ifndef ResistiveSoilSensor_h
#define ResistiveSoilSensor_h

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#define RESISTIVE_SENSOR_VERSION "1.1.0"

/**
 * @brief Configuration du diviseur de tension
 * 
 * PULL_UP  : VCC --[Rd]-- A0 --[Rsol]-- GND  (capteur côté GND)
 *            Sec → V_A0 ≈ VCC, Mouillé → V_A0 ≈ 0V
 * 
 * PULL_DOWN: VCC --[Rsol]-- A0 --[Rd]-- GND  (capteur côté VCC)
 *            Sec → V_A0 ≈ 0V, Mouillé → V_A0 ≈ VCC
 */
enum class PullMode : uint8_t {
  PULL_UP,    ///< Capteur entre A0 et GND (montage par défaut)
  PULL_DOWN   ///< Capteur entre VCC et A0
};

/**
 * @brief État de l'humidité du sol
 */
enum class MoistureLevel : uint8_t {
  VERY_DRY,   ///< Très sec
  DRY,        ///< Sec
  MOIST,      ///< Humide
  WET,        ///< Mouillé
  VERY_WET,   ///< Très mouillé / inondé
  ERROR       ///< Erreur de lecture
};

/**
 * @brief Point de calibration pour le capteur
 */
struct CalibrationPoint {
  float resistance;   ///< Résistance en ohms
  uint8_t percent;    ///< Pourcentage d'humidité correspondant (0-100)
  
  CalibrationPoint(float r = 0.0f, uint8_t p = 0) 
    : resistance(r), percent(p) {}
};

/**
 * @brief Données brutes et calculées du capteur
 */
struct SensorData {
  uint16_t rawValue;      ///< Valeur ADC brute (0-1023)
  float voltage;          ///< Tension mesurée en Volts
  float soilResistance;   ///< Résistance du sol en ohms
  float moisturePercent;  ///< Pourcentage d'humidité (0-100)
  MoistureLevel level;    ///< Niveau qualitatif
  bool valid;             ///< Données valides ?
  
  SensorData() 
    : rawValue(0), voltage(0.0f), soilResistance(0.0f), 
      moisturePercent(0.0f), level(MoistureLevel::ERROR), valid(false) {}
};

class ResistiveSoilSensor {
public:
  // ============================================================
  // CONSTANTES
  // ============================================================
  
  static constexpr float DEFAULT_PULL_OHMS = 510000.0f;   ///< 510kΩ
  static constexpr float DEFAULT_VOLTAGE = 3.3f;          ///< Tension par défaut
  static constexpr float DEFAULT_DRY_OHMS = 300000.0f;    ///< 300kΩ (sec)
  static constexpr float DEFAULT_WET_OHMS = 1500.0f;      ///< 1.5kΩ (mouillé)
  
  static constexpr uint16_t ADC_MAX = 1023;               ///< Résolution Arduino UNO/Nano
  static constexpr float VOLTAGE_TOLERANCE = 0.005f;      ///< Seuil de saturation
  
  // Seuils pour les niveaux qualitatifs (personnalisables)
  static constexpr uint8_t THRESHOLD_VERY_DRY = 10;
  static constexpr uint8_t THRESHOLD_DRY = 30;
  static constexpr uint8_t THRESHOLD_MOIST = 60;
  static constexpr uint8_t THRESHOLD_WET = 80;

  // ============================================================
  // CONSTRUCTEURS
  // ============================================================

  /**
   * @brief Constructeur minimal (PULL_UP par défaut)
   * @param analogPin Pin analogique (A0, A1, etc.)
   */
  explicit ResistiveSoilSensor(uint8_t analogPin);

  /**
   * @brief Constructeur avec mode de pull
   * @param analogPin Pin analogique
   * @param mode Mode de montage (PULL_UP ou PULL_DOWN)
   */
  ResistiveSoilSensor(uint8_t analogPin, PullMode mode);

  /**
   * @brief Constructeur complet
   * @param analogPin Pin analogique
   * @param mode Mode de montage
   * @param pullOhms Valeur de la résistance de pull (ohms)
   * @param voltage Tension d'alimentation (Volts)
   */
  ResistiveSoilSensor(uint8_t analogPin, PullMode mode, 
                      float pullOhms, float voltage);

  // ============================================================
  // CONFIGURATION
  // ============================================================

  /** @brief Définir le mode de montage */
  void setPullMode(PullMode mode);
  
  /** @brief Définir la résistance de pull (ohms) */
  void setPullResistor(float ohms);
  
  /** @brief Définir la tension d'alimentation (Volts) */
  void setVoltage(float voltage);
  
  /** @brief Définir les points de calibration */
  void setCalibration(float dryOhms, float wetOhms);
  
  /** @brief Définir le pin de sortie digitale (D0 du LM393) */
  void setDigitalPin(uint8_t digitalPin);
  
  /** @brief Activer/désactiver le lissage des lectures */
  void setSmoothing(bool enable, uint8_t samples = 10);
  
  /** @brief Définir les seuils pour les niveaux qualitatifs */
  void setLevelThresholds(uint8_t veryDry, uint8_t dry, 
                          uint8_t moist, uint8_t wet);

  // ============================================================
  // LECTURES
  // ============================================================

  /** @brief Lire toutes les données du capteur */
  SensorData read();
  
  /** @brief Lire la valeur ADC brute (0-1023) */
  uint16_t readRaw();
  
  /** @brief Lire la tension en Volts */
  float readVoltage();
  
  /** @brief Lire la résistance du sol en ohms */
  float readSoilResistance();
  
  /** @brief Lire le pourcentage d'humidité (0-100) */
  float readMoisturePercent();
  
  /** @brief Lire le niveau qualitatif */
  MoistureLevel readMoistureLevel();
  
  /** @brief Lire l'état de la sortie digitale (D0) */
  bool readDigitalOutput();

  // ============================================================
  // ACCÈS AUX PARAMÈTRES
  // ============================================================

  PullMode getPullMode() const;
  float getPullResistor() const;
  float getVoltage() const;
  float getDryOhms() const;
  float getWetOhms() const;
  uint8_t getAnalogPin() const;
  bool hasDigitalPin() const;

  // ============================================================
  // FONCTIONS UTILITAIRES STATIQUES
  // ============================================================

  /**
   * @brief Calculer la résistance du sol depuis la tension
   * @param v Tension mesurée
   * @param voltage Tension d'alimentation
   * @param pullOhms Résistance de pull
   * @param mode Mode de montage
   * @return Résistance du sol en ohms
   */
  static float calculateSoilResistance(float v, float voltage, 
                                        float pullOhms, PullMode mode);

  /**
   * @brief Calculer le pourcentage d'humidité depuis la résistance
   * @param resistance Résistance du sol
   * @param dryOhms Résistance à l'état sec
   * @param wetOhms Résistance à l'état mouillé
   * @return Pourcentage (0-100)
   */
  static float calculateMoisturePercent(float resistance, 
                                         float dryOhms, float wetOhms);

  /**
   * @brief Calculer la tension attendue depuis la résistance
   * @param resistance Résistance du sol
   * @param voltage Tension d'alimentation
   * @param pullOhms Résistance de pull
   * @param mode Mode de montage
   * @return Tension attendue en Volts
   */
  static float calculateVoltage(float resistance, float voltage, 
                                 float pullOhms, PullMode mode);

  /**
   * @brief Convertir un pourcentage en niveau qualitatif
   */
  static MoistureLevel percentToLevel(uint8_t percent, 
                                       uint8_t veryDry, uint8_t dry,
                                       uint8_t moist, uint8_t wet);

private:
  uint8_t _analogPin;
  uint8_t _digitalPin;
  PullMode _pullMode;
  float _pullOhms;
  float _voltage;
  float _dryOhms;
  float _wetOhms;
  
  // Lissage
  bool _smoothingEnabled;
  uint8_t _sampleCount;
  
  // Seuils
  uint8_t _thresholdVeryDry;
  uint8_t _thresholdDry;
  uint8_t _thresholdMoist;
  uint8_t _thresholdWet;
  
  // Buffer pour lissage
  float* _samples;
  uint8_t _sampleIndex;
  
  float averageRaw();
  void addSample(float value);
};

// ============================================================
// IMPLÉMENTATION INLINE
// ============================================================

// --- Constructeurs ---

inline ResistiveSoilSensor::ResistiveSoilSensor(uint8_t analogPin)
  : _analogPin(analogPin),
    _digitalPin(255),
    _pullMode(PullMode::PULL_UP),
    _pullOhms(DEFAULT_PULL_OHMS),
    _voltage(DEFAULT_VOLTAGE),
    _dryOhms(DEFAULT_DRY_OHMS),
    _wetOhms(DEFAULT_WET_OHMS),
    _smoothingEnabled(false),
    _sampleCount(10),
    _thresholdVeryDry(THRESHOLD_VERY_DRY),
    _thresholdDry(THRESHOLD_DRY),
    _thresholdMoist(THRESHOLD_MOIST),
    _thresholdWet(THRESHOLD_WET),
    _samples(nullptr),
    _sampleIndex(0)
{}

inline ResistiveSoilSensor::ResistiveSoilSensor(uint8_t analogPin, PullMode mode)
  : ResistiveSoilSensor(analogPin) {
  _pullMode = mode;
}

inline ResistiveSoilSensor::ResistiveSoilSensor(uint8_t analogPin, PullMode mode,
                                                  float pullOhms, float voltage)
  : ResistiveSoilSensor(analogPin, mode) {
  _pullOhms = pullOhms;
  _voltage = voltage;
}

// --- Configuration ---

inline void ResistiveSoilSensor::setPullMode(PullMode mode) {
  _pullMode = mode;
}

inline void ResistiveSoilSensor::setPullResistor(float ohms) {
  _pullOhms = ohms;
}

inline void ResistiveSoilSensor::setVoltage(float voltage) {
  _voltage = voltage;
}

inline void ResistiveSoilSensor::setCalibration(float dryOhms, float wetOhms) {
  _dryOhms = dryOhms;
  _wetOhms = wetOhms;
}

inline void ResistiveSoilSensor::setDigitalPin(uint8_t digitalPin) {
  _digitalPin = digitalPin;
  pinMode(_digitalPin, INPUT);
}

inline void ResistiveSoilSensor::setSmoothing(bool enable, uint8_t samples) {
  if (_samples != nullptr) {
    delete[] _samples;
    _samples = nullptr;
  }
  
  _smoothingEnabled = enable;
  _sampleCount = samples;
  
  if (enable && samples > 1) {
    _samples = new float[samples]();
    _sampleIndex = 0;
  }
}

inline void ResistiveSoilSensor::setLevelThresholds(uint8_t veryDry, uint8_t dry,
                                                     uint8_t moist, uint8_t wet) {
  _thresholdVeryDry = veryDry;
  _thresholdDry = dry;
  _thresholdMoist = moist;
  _thresholdWet = wet;
}

// --- Lectures ---

inline SensorData ResistiveSoilSensor::read() {
  SensorData data;
  
  data.rawValue = readRaw();
  data.voltage = readVoltage();
  data.soilResistance = readSoilResistance();
  data.moisturePercent = readMoisturePercent();
  data.level = readMoistureLevel();
  data.valid = (data.level != MoistureLevel::ERROR);
  
  return data;
}

inline uint16_t ResistiveSoilSensor::readRaw() {
  if (_smoothingEnabled && _samples != nullptr) {
    return static_cast<uint16_t>(averageRaw());
  }
  return analogRead(_analogPin);
}

inline float ResistiveSoilSensor::readVoltage() {
  return _voltage * readRaw() / static_cast<float>(ADC_MAX);
}

inline float ResistiveSoilSensor::readSoilResistance() {
  float v = readVoltage();
  return calculateSoilResistance(v, _voltage, _pullOhms, _pullMode);
}

inline float ResistiveSoilSensor::readMoisturePercent() {
  float r = readSoilResistance();
  return calculateMoisturePercent(r, _dryOhms, _wetOhms);
}

inline MoistureLevel ResistiveSoilSensor::readMoistureLevel() {
  uint8_t percent = static_cast<uint8_t>(readMoisturePercent());
  return percentToLevel(percent, _thresholdVeryDry, _thresholdDry,
                        _thresholdMoist, _thresholdWet);
}

inline bool ResistiveSoilSensor::readDigitalOutput() {
  if (_digitalPin == 255) return false;
  return digitalRead(_digitalPin) == HIGH;
}

// --- Accesseurs ---

inline PullMode ResistiveSoilSensor::getPullMode() const {
  return _pullMode;
}

inline float ResistiveSoilSensor::getPullResistor() const {
  return _pullOhms;
}

inline float ResistiveSoilSensor::getVoltage() const {
  return _voltage;
}

inline float ResistiveSoilSensor::getDryOhms() const {
  return _dryOhms;
}

inline float ResistiveSoilSensor::getWetOhms() const {
  return _wetOhms;
}

inline uint8_t ResistiveSoilSensor::getAnalogPin() const {
  return _analogPin;
}

inline bool ResistiveSoilSensor::hasDigitalPin() const {
  return _digitalPin != 255;
}

// --- Fonctions utilitaires statiques ---

inline float ResistiveSoilSensor::calculateSoilResistance(float v, float voltage,
                                                           float pullOhms, 
                                                           PullMode mode) {
  // Protection contre les valeurs extrêmes
  if (v <= VOLTAGE_TOLERANCE) {
    // Tension ≈ 0V
    return (mode == PullMode::PULL_UP) ? 0.0f : 999999999.0f;
  }
  if (v >= voltage - VOLTAGE_TOLERANCE) {
    // Tension ≈ VCC
    return (mode == PullMode::PULL_UP) ? 999999999.0f : 0.0f;
  }

  if (mode == PullMode::PULL_UP) {
    // PULL_UP: Rsol = Rd * V / (VCC - V)
    return pullOhms * v / (voltage - v);
  } else {
    // PULL_DOWN: Rsol = Rd * (VCC - V) / V
    return pullOhms * (voltage - v) / v;
  }
}

inline float ResistiveSoilSensor::calculateMoisturePercent(float resistance,
                                                            float dryOhms,
                                                            float wetOhms) {
  if (resistance <= wetOhms) return 100.0f;
  if (resistance >= dryOhms) return 0.0f;
  if (dryOhms <= wetOhms) return 0.0f;  // Protection division par zéro

  float h = 100.0f * (dryOhms - resistance) / (dryOhms - wetOhms);
  return constrain(h, 0.0f, 100.0f);
}

inline float ResistiveSoilSensor::calculateVoltage(float resistance, float voltage,
                                                    float pullOhms, PullMode mode) {
  if (resistance < 0.0f) return 0.0f;
  if (resistance == 0.0f) {
    return (mode == PullMode::PULL_UP) ? 0.0f : voltage;
  }

  float totalResistance = pullOhms + resistance;
  
  if (mode == PullMode::PULL_UP) {
    // PULL_UP: V = VCC * Rsol / (Rd + Rsol)
    return voltage * resistance / totalResistance;
  } else {
    // PULL_DOWN: V = VCC * Rd / (Rd + Rsol)
    return voltage * pullOhms / totalResistance;
  }
}

inline MoistureLevel ResistiveSoilSensor::percentToLevel(uint8_t percent,
                                                          uint8_t veryDry,
                                                          uint8_t dry,
                                                          uint8_t moist,
                                                          uint8_t wet) {
  if (percent <= veryDry) return MoistureLevel::VERY_DRY;
  if (percent <= dry) return MoistureLevel::DRY;
  if (percent <= moist) return MoistureLevel::MOIST;
  if (percent <= wet) return MoistureLevel::WET;
  if (percent <= 100) return MoistureLevel::VERY_WET;
  return MoistureLevel::ERROR;
}

// --- Méthodes privées ---

inline float ResistiveSoilSensor::averageRaw() {
  if (_samples == nullptr || _sampleCount == 0) {
    return static_cast<float>(analogRead(_analogPin));
  }
  
  // Ajouter nouvel échantillon
  _samples[_sampleIndex] = static_cast<float>(analogRead(_analogPin));
  _sampleIndex = (_sampleIndex + 1) % _sampleCount;
  
  // Calculer la moyenne
  float sum = 0.0f;
  for (uint8_t i = 0; i < _sampleCount; i++) {
    sum += _samples[i];
  }
  return sum / _sampleCount;
}

inline void ResistiveSoilSensor::addSample(float value) {
  if (_samples != nullptr) {
    _samples[_sampleIndex] = value;
    _sampleIndex = (_sampleIndex + 1) % _sampleCount;
  }
}

#endif // ResistiveSoilSensor_h