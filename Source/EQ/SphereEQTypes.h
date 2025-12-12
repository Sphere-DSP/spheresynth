#pragma once
#include <JuceHeader.h>
#include <array>
#include <cmath>

namespace Sphere {

// ============================================================================
// EQ Filter Types
// ============================================================================
enum class EQFilterType {
  Bell,
  LowShelf,
  HighShelf,
  LowCut,
  HighCut,
  Notch,
  BandPass,
  Tilt,
  AllPass
};

// ============================================================================
// Stereo Processing Modes
// ============================================================================
enum class EQStereoMode {
  Stereo, // Normal stereo (same processing both channels)
  Left,   // Process left only
  Right,  // Process right only
  Mid,    // Process mid (L+R)/2
  Side    // Process side (L-R)/2
};

// ============================================================================
// Dynamic EQ Modes
// ============================================================================
enum class EQDynamicMode {
  Off,
  Compress, // Downward compression - reduce gain when signal exceeds threshold
  Expand,   // Upward expansion - increase gain when signal exceeds threshold
  Gate,  // Downward expansion - reduce gain when signal falls below threshold
  DeEss, // Specialized compression for high frequency reduction
  SpectralCompress, // FFT-based per-bin compression
  SpectralExpand    // FFT-based per-bin expansion
};

// ============================================================================
// Spectral Dynamics Quality
// ============================================================================
enum class SpectralQuality {
  Normal, // 2048 FFT, 50% overlap (~23ms latency)
  High    // 4096 FFT, 75% overlap (~46ms latency)
};

// ============================================================================
// Dynamic Detection Modes
// ============================================================================
enum class EQDetectionMode {
  Peak,   // Fast peak detection - good for transients
  RMS,    // RMS (root mean square) - smoother, more musical
  PeakRMS // Combined: peak for attack, RMS for release
};

// ============================================================================
// Character/Analog Modeling Modes
// ============================================================================
enum class EQCharacterMode {
  Clean,  // Transparent digital
  Subtle, // Light transformer saturation
  Warm    // Stronger tube saturation
};

// ============================================================================
// Phase Modes
// ============================================================================
enum class EQPhaseMode {
  MinimumPhase, // Standard IIR (introduces phase shift)
  LinearPhase,  // FIR-based (latency, no phase shift)
  NaturalPhase  // Hybrid approach
};

// ============================================================================
// Filter Slope (dB/octave for cut filters)
// ============================================================================
enum class EQSlope {
  dB6 = 1,   // 6 dB/oct  - 1st order (not biquad, special case)
  dB12 = 2,  // 12 dB/oct - 1 biquad (2nd order)
  dB18 = 3,  // 18 dB/oct - 1st + 2nd order
  dB24 = 4,  // 24 dB/oct - 2 biquads (4th order)
  dB36 = 6,  // 36 dB/oct - 3 biquads (6th order)
  dB48 = 8,  // 48 dB/oct - 4 biquads (8th order)
  dB72 = 12, // 72 dB/oct - 6 biquads (12th order)
  dB96 = 16  // 96 dB/oct - 8 biquads (16th order)
};

// ============================================================================
// Biquad Coefficients Structure
// ============================================================================
struct BiquadCoeffs {
  double b0 = 1.0, b1 = 0.0, b2 = 0.0;
  double a0 = 1.0, a1 = 0.0, a2 = 0.0;

  // Normalize coefficients by a0
  void normalize() {
    if (std::abs(a0) > 1e-15) {
      b0 /= a0;
      b1 /= a0;
      b2 /= a0;
      a1 /= a0;
      a2 /= a0;
      a0 = 1.0;
    }
  }

  // Check for numerical stability
  bool isStable() const {
    // For stability: |a2| < 1 and |a1| < 1 + a2
    return std::abs(a2) < 1.0 && std::abs(a1) < (1.0 + a2);
  }
};

// ============================================================================
// EQ Band Parameters Structure
// ============================================================================
struct EQBandParams {
  bool bypass = false;
  EQFilterType type = EQFilterType::Bell;
  double frequency = 1000.0;     // Hz (20 - 20000)
  double q = 1.0;                // Q factor (0.1 - 30)
  double gainDb = 0.0;           // dB (-30 to +30)
  EQSlope slope = EQSlope::dB12; // For cut filters
  EQStereoMode stereoMode = EQStereoMode::Stereo;
  EQDynamicMode dynamicMode = EQDynamicMode::Off;
  EQCharacterMode characterMode = EQCharacterMode::Clean;
  EQPhaseMode phaseMode = EQPhaseMode::MinimumPhase;

  // Dynamic EQ parameters
  double dynamicThreshold = -20.0; // dB (-60 to +12)
  double dynamicRatio = 2.0; // ratio (1.0 to infinity, 1.0 = no compression)
  double dynamicAttack =
      10.0; // ms (0.1 to 500 - fast transients to slow mastering)
  double dynamicRelease = 100.0; // ms (5 to 5000 - fast to slow release)
  double dynamicRange = 12.0;    // dB (0 to 48 - max gain change)
  double dynamicKnee = 6.0;      // dB (0 = hard knee, up to 24 = very soft)
  EQDetectionMode dynamicDetection = EQDetectionMode::RMS;

  // Advanced dynamic parameters
  double dynamicMakeupGain = 0.0; // dB auto or manual makeup gain
  double dynamicMix = 100.0;      // % (0-100) parallel compression
  double dynamicSidechainQ =
      1.0; // Q for sidechain filter (0.1-10, higher = narrower)
  bool dynamicAutoMakeup = false; // Auto-calculate makeup gain

  // Spectral dynamics parameters
  SpectralQuality spectralQuality = SpectralQuality::Normal;

  bool operator==(const EQBandParams &other) const {
    return bypass == other.bypass && type == other.type &&
           std::abs(frequency - other.frequency) < 0.001 &&
           std::abs(q - other.q) < 0.0001 &&
           std::abs(gainDb - other.gainDb) < 0.01 && slope == other.slope &&
           stereoMode == other.stereoMode && dynamicMode == other.dynamicMode;
  }

  bool operator!=(const EQBandParams &other) const { return !(*this == other); }
};

// ============================================================================
// Constants
// ============================================================================
constexpr int MAX_EQ_BANDS = 24;
constexpr int MAX_BIQUADS_PER_BAND = 8; // For 96 dB/oct slopes
constexpr double MIN_FREQUENCY = 20.0;
constexpr double MAX_FREQUENCY = 20000.0;
constexpr double MIN_Q = 0.1;
constexpr double MAX_Q = 30.0;
constexpr double MIN_GAIN_DB = -30.0;
constexpr double MAX_GAIN_DB = 30.0;

// Denormal threshold
constexpr double DENORMAL_THRESHOLD = 1e-15;

// ============================================================================
// Helper: Flush denormals to zero
// ============================================================================
inline double flushDenormal(double value) {
  return (std::abs(value) < DENORMAL_THRESHOLD) ? 0.0 : value;
}

inline float flushDenormal(float value) {
  return (std::abs(value) < static_cast<float>(DENORMAL_THRESHOLD)) ? 0.0f
                                                                    : value;
}

} // namespace Sphere
