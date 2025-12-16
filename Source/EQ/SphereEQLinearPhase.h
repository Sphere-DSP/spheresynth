/*
  ==============================================================================
    SphereEQLinearPhase.h
    FFT-based Linear Phase EQ using overlap-add convolution
    
    Implements block-based FFT convolution with symmetric FIR kernels
    for zero phase shift equalization.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SphereEQTypes.h"
#include "SphereEQCookbook.h"
#include <array>
#include <vector>
#include <complex>
#include <cmath>

namespace Sphere {

// ============================================================================
// Simple FFT Implementation (Cooley-Tukey radix-2)
// ============================================================================
class SimpleFFT {
public:
    SimpleFFT(int order) : fftOrder(order), fftSize(1 << order) {
        // Precompute twiddle factors
        twiddleFactors.resize(fftSize / 2);
        for (int i = 0; i < fftSize / 2; ++i) {
            double angle = -2.0 * M_PI * i / fftSize;
            twiddleFactors[i] = std::complex<double>(std::cos(angle), std::sin(angle));
        }
    }
    
    void forward(std::vector<std::complex<double>>& data) {
        performFFT(data, false);
    }
    
    void inverse(std::vector<std::complex<double>>& data) {
        performFFT(data, true);
        // Normalize
        double scale = 1.0 / fftSize;
        for (auto& c : data) {
            c *= scale;
        }
    }
    
    int getSize() const { return fftSize; }
    
private:
    void performFFT(std::vector<std::complex<double>>& data, bool inverse) {
        // Bit reversal permutation
        int n = fftSize;
        for (int i = 1, j = 0; i < n; ++i) {
            int bit = n >> 1;
            for (; j & bit; bit >>= 1) {
                j ^= bit;
            }
            j ^= bit;
            if (i < j) {
                std::swap(data[i], data[j]);
            }
        }
        
        // Cooley-Tukey iterative FFT
        for (int len = 2; len <= n; len <<= 1) {
            double angle = (inverse ? 2.0 : -2.0) * M_PI / len;
            std::complex<double> wlen(std::cos(angle), std::sin(angle));
            
            for (int i = 0; i < n; i += len) {
                std::complex<double> w(1.0, 0.0);
                for (int j = 0; j < len / 2; ++j) {
                    std::complex<double> u = data[i + j];
                    std::complex<double> v = data[i + j + len / 2] * w;
                    data[i + j] = u + v;
                    data[i + j + len / 2] = u - v;
                    w *= wlen;
                }
            }
        }
    }
    
    int fftOrder;
    int fftSize;
    std::vector<std::complex<double>> twiddleFactors;
};

// ============================================================================
// FIR Kernel Length Options
// ============================================================================
enum class LinearPhaseLength {
    Short = 512,    // ~11.6ms @ 44.1kHz - Low latency, less accurate
    Medium = 2048,  // ~46.4ms @ 44.1kHz - Balanced
    Long = 8192     // ~185.8ms @ 44.1kHz - High accuracy, more latency
};

// ============================================================================
// Linear Phase FIR Kernel Generator
// ============================================================================
class FIRKernelGenerator {
public:
    // Generate symmetric FIR kernel from target magnitude response
    static std::vector<double> generateKernel(
        const std::vector<double>& magnitudeResponse,  // Linear magnitude (not dB)
        int kernelLength,
        double sampleRate)
    {
        // Ensure power of 2 for FFT
        int fftSize = kernelLength * 2;
        int fftOrder = static_cast<int>(std::log2(fftSize));
        if ((1 << fftOrder) != fftSize) {
            fftOrder++;
            fftSize = 1 << fftOrder;
        }
        
        SimpleFFT fft(fftOrder);
        
        // Create frequency domain representation
        std::vector<std::complex<double>> spectrum(fftSize);
        
        // Interpolate magnitude response to FFT bins
        int numBins = fftSize / 2 + 1;
        for (int i = 0; i < numBins; ++i) {
            double freq = (i * sampleRate) / fftSize;
            double mag = interpolateMagnitude(magnitudeResponse, freq, sampleRate);
            
            // Linear phase = zero phase in frequency domain
            spectrum[i] = std::complex<double>(mag, 0.0);
        }
        
        // Mirror for negative frequencies (conjugate symmetric)
        for (int i = 1; i < fftSize / 2; ++i) {
            spectrum[fftSize - i] = std::conj(spectrum[i]);
        }
        
        // Inverse FFT to get impulse response
        fft.inverse(spectrum);
        
        // Extract and window the kernel (centered, symmetric)
        std::vector<double> kernel(kernelLength);
        int center = kernelLength / 2;
        
        // The IFFT result needs to be circularly shifted
        for (int i = 0; i < kernelLength; ++i) {
            int srcIdx = (i - center + fftSize) % fftSize;
            kernel[i] = spectrum[srcIdx].real();
        }
        
        // Apply window function (Blackman-Harris for good stopband)
        applyBlackmanHarrisWindow(kernel);
        
        // Normalize for unity gain at DC
        double sum = 0.0;
        for (double s : kernel) sum += s;
        if (std::abs(sum) > 1e-10) {
            double scale = 1.0 / sum;
            for (double& s : kernel) s *= scale;
        }
        
        return kernel;
    }
    
    // Generate magnitude response from EQ band parameters
    static std::vector<double> generateMagnitudeFromBands(
        const std::array<EQBandParams, MAX_EQ_BANDS>& bands,
        int numActiveBands,
        double sampleRate,
        int numPoints = 1024)
    {
        std::vector<double> magnitude(numPoints, 1.0);  // Linear magnitude
        
        double freqStep = (sampleRate / 2.0) / (numPoints - 1);
        
        for (int bandIdx = 0; bandIdx < numActiveBands; ++bandIdx) {
            const auto& params = bands[bandIdx];
            if (params.bypass) continue;
            
            // Calculate magnitude response for this band
            BiquadCoeffs coeffs = RBJCookbook::calculate(
                params.type, sampleRate, params.frequency, params.q, params.gainDb
            );
            
            for (int i = 0; i < numPoints; ++i) {
                double freq = i * freqStep;
                if (freq < 1.0) freq = 1.0;  // Avoid DC issues
                
                double mag = calculateBiquadMagnitude(coeffs, freq, sampleRate);
                
                // For cut filters with higher slopes, cascade the magnitude
                if (params.type == EQFilterType::LowCut || params.type == EQFilterType::HighCut) {
                    int stages = static_cast<int>(params.slope) / 2;
                    mag = std::pow(mag, stages);
                }
                
                magnitude[i] *= mag;
            }
        }
        
        return magnitude;
    }
    
private:
    static double interpolateMagnitude(const std::vector<double>& magResponse,
                                       double freq, double sampleRate) {
        if (magResponse.empty()) return 1.0;
        
        double nyquist = sampleRate / 2.0;
        double normalized = freq / nyquist;
        double index = normalized * (magResponse.size() - 1);
        
        int idx0 = static_cast<int>(index);
        int idx1 = idx0 + 1;
        
        if (idx0 < 0) return magResponse[0];
        if (idx1 >= static_cast<int>(magResponse.size())) return magResponse.back();
        
        double frac = index - idx0;
        return magResponse[idx0] * (1.0 - frac) + magResponse[idx1] * frac;
    }
    
    static double calculateBiquadMagnitude(const BiquadCoeffs& c, double freq, double sampleRate) {
        double omega = 2.0 * M_PI * freq / sampleRate;
        double cosW = std::cos(omega);
        double cos2W = std::cos(2.0 * omega);
        double sinW = std::sin(omega);
        double sin2W = std::sin(2.0 * omega);
        
        // H(e^jw) = (b0 + b1*e^-jw + b2*e^-2jw) / (a0 + a1*e^-jw + a2*e^-2jw)
        // |H|^2 = |numerator|^2 / |denominator|^2
        
        double numReal = c.b0 + c.b1 * cosW + c.b2 * cos2W;
        double numImag = -c.b1 * sinW - c.b2 * sin2W;
        double denReal = 1.0 + c.a1 * cosW + c.a2 * cos2W;
        double denImag = -c.a1 * sinW - c.a2 * sin2W;
        
        double numMagSq = numReal * numReal + numImag * numImag;
        double denMagSq = denReal * denReal + denImag * denImag;
        
        if (denMagSq < 1e-20) return 1.0;
        return std::sqrt(numMagSq / denMagSq);
    }
    
    static void applyBlackmanHarrisWindow(std::vector<double>& data) {
        const double a0 = 0.35875;
        const double a1 = 0.48829;
        const double a2 = 0.14128;
        const double a3 = 0.01168;
        
        int N = static_cast<int>(data.size());
        for (int i = 0; i < N; ++i) {
            double n = static_cast<double>(i) / (N - 1);
            double window = a0 - a1 * std::cos(2.0 * M_PI * n)
                              + a2 * std::cos(4.0 * M_PI * n)
                              - a3 * std::cos(6.0 * M_PI * n);
            data[i] *= window;
        }
    }
};

// ============================================================================
// Overlap-Add FFT Convolver
// ============================================================================
class FFTConvolver {
public:
    FFTConvolver() = default;
    
    void prepare(int kernelLength, double sampleRate) {
        this->kernelLength = kernelLength;
        this->sampleRate = sampleRate;
        
        // FFT size should be at least 2x kernel for linear convolution
        // Use next power of 2 for efficiency
        fftOrder = static_cast<int>(std::ceil(std::log2(kernelLength * 2)));
        fftSize = 1 << fftOrder;
        blockSize = fftSize - kernelLength + 1;
        
        fft = std::make_unique<SimpleFFT>(fftOrder);
        
        // Allocate buffers
        inputBuffer.resize(fftSize, 0.0);
        outputBuffer.resize(fftSize, 0.0);
        overlapBuffer.resize(kernelLength - 1, 0.0);
        kernelSpectrum.resize(fftSize);
        tempSpectrum.resize(fftSize);
        
        inputWritePos = 0;
        outputReadPos = 0;
        outputAvailable = 0;
        
        kernelReady = false;
    }
    
    void setKernel(const std::vector<double>& kernel) {
        if (kernel.size() != static_cast<size_t>(kernelLength)) return;
        
        // Zero-pad kernel to FFT size and transform
        std::vector<std::complex<double>> kernelPadded(fftSize);
        for (int i = 0; i < kernelLength; ++i) {
            kernelPadded[i] = std::complex<double>(kernel[i], 0.0);
        }
        
        fft->forward(kernelPadded);
        kernelSpectrum = kernelPadded;
        kernelReady = true;
    }
    
    void reset() {
        std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0);
        std::fill(outputBuffer.begin(), outputBuffer.end(), 0.0);
        std::fill(overlapBuffer.begin(), overlapBuffer.end(), 0.0);
        inputWritePos = 0;
        outputReadPos = 0;
        outputAvailable = 0;
    }
    
    // Process sample-by-sample with internal buffering
    float processSample(float input) {
        if (!kernelReady) return input;
        
        // Add input to buffer
        inputBuffer[inputWritePos++] = input;
        
        // When we have enough samples, process a block
        if (inputWritePos >= blockSize) {
            processBlock();
            inputWritePos = 0;
        }
        
        // Return output if available
        if (outputAvailable > 0) {
            float out = static_cast<float>(outputBuffer[outputReadPos++]);
            outputAvailable--;
            if (outputReadPos >= fftSize) outputReadPos = 0;
            return out;
        }
        
        return 0.0f;  // Latency period
    }
    
    int getLatencySamples() const {
        return kernelLength / 2;  // Symmetric kernel = half kernel latency
    }
    
private:
    void processBlock() {
        // Zero-pad input to FFT size
        for (int i = blockSize; i < fftSize; ++i) {
            inputBuffer[i] = 0.0;
        }
        
        // Copy to complex buffer
        for (int i = 0; i < fftSize; ++i) {
            tempSpectrum[i] = std::complex<double>(inputBuffer[i], 0.0);
        }
        
        // Forward FFT
        fft->forward(tempSpectrum);
        
        // Multiply spectra (convolution in time domain)
        for (int i = 0; i < fftSize; ++i) {
            tempSpectrum[i] *= kernelSpectrum[i];
        }
        
        // Inverse FFT
        fft->inverse(tempSpectrum);
        
        // Overlap-add
        int overlapLen = kernelLength - 1;
        
        // Add overlap from previous block
        for (int i = 0; i < overlapLen; ++i) {
            outputBuffer[i] = tempSpectrum[i].real() + overlapBuffer[i];
        }
        
        // Copy rest of output
        for (int i = overlapLen; i < fftSize; ++i) {
            outputBuffer[i] = tempSpectrum[i].real();
        }
        
        // Save overlap for next block
        for (int i = 0; i < overlapLen; ++i) {
            overlapBuffer[i] = tempSpectrum[blockSize + i].real();
        }
        
        outputReadPos = 0;
        outputAvailable = blockSize;
    }
    
    int kernelLength = 512;
    int fftSize = 1024;
    int fftOrder = 10;
    int blockSize = 513;
    double sampleRate = 44100.0;
    
    std::unique_ptr<SimpleFFT> fft;
    
    std::vector<double> inputBuffer;
    std::vector<double> outputBuffer;
    std::vector<double> overlapBuffer;
    std::vector<std::complex<double>> kernelSpectrum;
    std::vector<std::complex<double>> tempSpectrum;
    
    int inputWritePos = 0;
    int outputReadPos = 0;
    int outputAvailable = 0;
    
    bool kernelReady = false;
};

// ============================================================================
// Stereo Linear Phase EQ Processor
// ============================================================================
class LinearPhaseEQ {
public:
    LinearPhaseEQ() = default;
    
    void prepare(double sampleRate, int maxBlockSize, LinearPhaseLength length = LinearPhaseLength::Medium) {
        this->sampleRate = sampleRate;
        this->firLength = static_cast<int>(length);
        
        for (auto& conv : convolvers) {
            conv.prepare(firLength, sampleRate);
        }
        
        kernelDirty = true;
    }
    
    void reset() {
        for (auto& conv : convolvers) {
            conv.reset();
        }
    }
    
    void setFIRLength(LinearPhaseLength length) {
        if (static_cast<int>(length) != firLength) {
            firLength = static_cast<int>(length);
            for (auto& conv : convolvers) {
                conv.prepare(firLength, sampleRate);
            }
            kernelDirty = true;
        }
    }
    
    void updateBandParameters(const std::array<EQBandParams, MAX_EQ_BANDS>& bands, int numActive) {
        this->bandParams = bands;
        this->numActiveBands = numActive;
        kernelDirty = true;
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer) {
        // Rebuild kernel if parameters changed
        if (kernelDirty) {
            rebuildKernel();
            kernelDirty = false;
        }
        
        const int numSamples = buffer.getNumSamples();
        const int numChannels = std::min(buffer.getNumChannels(), 2);
        
        for (int ch = 0; ch < numChannels; ++ch) {
            float* data = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i) {
                data[i] = convolvers[ch].processSample(data[i]);
            }
        }
    }
    
    int getLatencySamples() const {
        return firLength / 2;
    }
    
private:
    void rebuildKernel() {
        // Generate combined magnitude response
        auto magnitude = FIRKernelGenerator::generateMagnitudeFromBands(
            bandParams, numActiveBands, sampleRate, 2048
        );
        
        // Generate symmetric FIR kernel
        auto kernel = FIRKernelGenerator::generateKernel(magnitude, firLength, sampleRate);
        
        // Set kernel for both channels
        for (auto& conv : convolvers) {
            conv.setKernel(kernel);
        }
    }
    
    double sampleRate = 44100.0;
    int firLength = static_cast<int>(LinearPhaseLength::Medium);
    int numActiveBands = 0;
    
    std::array<EQBandParams, MAX_EQ_BANDS> bandParams;
    std::array<FFTConvolver, 2> convolvers;
    
    bool kernelDirty = true;
};

} // namespace Sphere





