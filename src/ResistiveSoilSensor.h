#ifndef Resistive_Sensor_h
#define Resistive_Sensor_h

#if ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#define RESISTIVE_SENSOR_VERSION "2.0.0"

/**
 * ResistiveSoilSensor - Bibliotheque pour capteur YL-69
 * 
 * Circuit: Pont diviseur avec pull-up fixe 510K vers VCC
 *          et resistance du sol (variable) vers GND
 * 
 *          VCC --[510K]-- A0 --[Rsol]-- GND
 * 
 * Formules:
 *   V_A0 = VCC * Rsol / (510K + Rsol)
 *   Rsol = 510K * V_A0 / (VCC - V_A0)
 */

class ResistiveSoilSensor {
public:
  /**
   * Constructeur
   * @param analogPin  Pin analogique (A0, A1, etc.)
   * @param voltage    Tension d'alimentation VCC (3.3V ou 5V)
   */
  ResistiveSoilSensor(uint8_t analogPin, float voltage = 3.3f)
    : _analogPin(analogPin),
      _voltage(voltage),
      _Rmax_SOL_0_pourcent(300.0f),    // Sol sec: Rsol > 300K
      _Rmin_SOL_100_pourcent(1.5f)     // Sol sature: Rsol ~ 1.5K
  {}

  /**
   * Lit la tension sur le pin analogique
   * @return Tension en Volts (0 a _voltage)
   */
  float readVoltage() {
    return _voltage * analogRead(_analogPin) / 1023.0f;
  }

  /**
   * Calcule la resistance du sol en kΩ
   * 
   * Formule du pont diviseur:
   *   V_A0 = VCC * Rsol / (Rd + Rsol)
   *   => Rsol = Rd * V_A0 / (VCC - V_A0)
   * 
   * @return Resistance du sol en kΩ
   */
  float readSoilResistance() {
    float v = readVoltage();
    // Protection: si V_A0 ≈ 0, sol tres conducteur (court-circuit)
    if (v <= 0.005f) return 0.0f;
    // Protection: si V_A0 ≈ VCC, sol tres resistif (circuit ouvert)
    if (v >= _voltage - 0.005f) return 999999.0f;
    // Formule CORRECTE: Rd * V_A0 / (VCC - V_A0)
    return PULLUP_RESISTOR * v / (_voltage - v);
  }

  float readSoilResistanceKOhms() {
    return readSoilResistance();
  }

  /**
   * Calcule le pourcentage d'humidite (0-100%)
   * 
   * Utilise la calibration par resistance:
   * - 0%  = sol sec   (R >= _Rmax_SOL_0_pourcent)
   * - 100% = sol sature (R <= _Rmin_SOL_100_pourcent)
   * - Interpolation lineaire entre les deux
   * 
   * @return Pourcentage d'humidite
   */
  float readMoisturePercent() {
    float r = readSoilResistance();
    return calculateMoistureFromResistance(r);
  }

  /**
   * Calcule l'humidite depuis une tension donnee
   * @param voltage  Tension mesuree en Volts
   * @return Pourcentage d'humidite
   */
  float readMoisturePercentFromVoltage(float voltage) {
    if (voltage <= 0.005f) return 100.0f;
    if (voltage >= _voltage - 0.005f) return 0.0f;
    // Calcul de Rsol depuis la tension, puis humidite
    float r = PULLUP_RESISTOR * voltage / (_voltage - voltage);
    return calculateMoistureFromResistance(r);
  }

  /**
   * Configure la tension d'alimentation
   * @param voltage  Tension en Volts (3.3 ou 5.0)
   */
  void setVoltage(float voltage) {
    _voltage = voltage;
  }

  /**
   * Configure les points de calibration
   * @param dryResistanceKOhms   Resistance du sol sec en kΩ (defaut: 300)
   * @param wetResistanceKOhms   Resistance du sol sature en kΩ (defaut: 1.5)
   */
  void setCalibration(float dryResistanceKOhms, float wetResistanceKOhms) {
    _Rmax_SOL_0_pourcent = dryResistanceKOhms;
    _Rmin_SOL_100_pourcent = wetResistanceKOhms;
  }

  /**
   * Lit la sortie digitale D0 du module LM393
   * @param digitalPin  Pin connecte a D0
   * @return Etat de la sortie digitale
   */
  bool readDigitalOutput(uint8_t digitalPin) {
    return digitalRead(digitalPin);
  }

  static const uint8_t DEFAULT_ANALOG_PIN = A0;
  static constexpr float PULLUP_RESISTOR = 510.0f;  // Pull-up fixe du YL-69

private:
  uint8_t _analogPin;
  float _voltage;
  float _Rmax_SOL_0_pourcent;
  float _Rmin_SOL_100_pourcent;

  /**
   * Calcule l'humidite depuis la resistance du sol
   */
  float calculateMoistureFromResistance(float resistance) {
    // Sol sature ou court-circuit
    if (resistance <= _Rmin_SOL_100_pourcent) {
      return 100.0f;
    }
    // Sol tres sec ou circuit ouvert
    if (resistance >= _Rmax_SOL_0_pourcent || resistance > 999900.0f) {
      return 0.0f;
    }
    // Interpolation lineaire entre sec et humide
    float h = 100.0f * (_Rmax_SOL_0_pourcent - resistance) 
                  / (_Rmax_SOL_0_pourcent - _Rmin_SOL_100_pourcent);
    // Clamp
    if (h > 100.0f) h = 100.0f;
    if (h < 0.0f) h = 0.0f;
    return h;
  }
};

// ============================================================
// FONCTIONS UTILITAIRES STATIQUES
// ============================================================

/**
 * Calcule la resistance du sol depuis la tension mesuree
 * Formule: Rsol = Rd * V_A0 / (VCC - V_A0)
 * 
 * @param v         Tension mesuree en Volts
 * @param voltage   Tension d'alimentation en Volts (defaut: 3.3V)
 * @param pullUp    Resistance pull-up en kΩ (defaut: 510.0k)
 * @return Resistance du sol en kΩ
 */
float Calcule_Rsol(float v, float voltage = 3.3f, float pullUp = 510.0f) {
  if (v <= 0.005f) return 0.0f;
  if (v >= voltage - 0.005f) return 999999.0f;
  return pullUp * v / (voltage - v);  // CORRIGE: pullUp * v / (voltage - v)
}

/**
 * Calcule le pourcentage d'humidite depuis la resistance du sol
 * 
 * @param r         Resistance du sol en kΩ
 * @param dryKOhms  Resistance sol sec en kΩ (defaut: 300k)
 * @param wetKOhms  Resistance sol humide en kΩ (defaut: 1.5k)
 * @return Pourcentage d'humidite (0-100%)
 */
float Calcule_Hsol(float r, float dryKOhms = 300.0f, float wetKOhms = 1.5f) {
  if (r <= wetKOhms) return 100.0f;
  if (r >= dryKOhms || r > 999900.0f) return 0.0f;
  float h = 100.0f * (dryKOhms - r) / (dryKOhms - wetKOhms);
  if (h > 100.0f) h = 100.0f;
  if (h < 0.0f) h = 0.0f;
  return h;
}

/**
 * Calcule la tension attendue pour une resistance de sol donnee
 * Utile pour la calibration et les tests
 * 
 * @param rsol      Resistance du sol en kΩ
 * @param voltage   Tension d'alimentation en Volts (defaut: 3.3V)
 * @param pullUp    Resistance pull-up en kΩ (defaut: 510.0k)
 * @return Tension attendue en Volts
 */
float Calcule_Tension(float rsol, float voltage = 3.3f, float pullUp = 510.0f) {
  if (rsol <= 0) return 0.0f;
  return voltage * rsol / (pullUp + rsol);
}

#endif