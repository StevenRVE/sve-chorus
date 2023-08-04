//
// Created by steve on 4-8-2023.
//

#include "oscillator.hpp"

Oscillator::Oscillator(double sampleRate, float frequency, Waveform waveform) :
frequency(frequency),
waveform(waveform)
{

}

Oscillator::~Oscillator()
{

}

void Oscillator::setSampleRate(double sampleRate)
{
    this->sampleRate = sampleRate;
}

void Oscillator::setFrequency(float frequency)
{
    this->frequency = frequency;
}

void Oscillator::setWaveform(Waveform waveform)
{
    this->waveform = waveform;
}

void Oscillator::setAmplitude(float value)
{
    this->amplitude = value;
}

void Oscillator::tick()
{
    calculateNextSineSample();
}

float Oscillator::getSample()
{
    return sample;
}

void Oscillator::calculateNextSineSample()
{
    double value = amplitude * sin(phase);
    phase += TWOPI * frequency / sampleRate;

    if (phase > TWOPI) {
        phase -= TWOPI;
    }

    sample = value;
}