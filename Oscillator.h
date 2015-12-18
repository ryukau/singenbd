#pragma once

#include <deque>
#include <random>
#include "Ranged.h"


class Oscillator
{
public:
    enum class OscillatorType : int
    {
        Simple = 0,
        Additive,
        Last,
    };

public:
    Oscillator();
    ~Oscillator();

    Oscillator & operator=(Oscillator& osc);

    // getter setter
    OscillatorType getType();
    float getShape();
    float getPhaseOffset();
    float getNoiseRatio();
    int getHarmonics();
    void setType(OscillatorType oscType);
    void setShape(float shape);
    void setPhaseOffset(float phase);
    void setNoiseRatio(float ratio);
    void setHarmonics(int harmonics);

    //
    float Oscillation(float phase);

private:
    OscillatorType type;
    float shape;
    float phaseOffset;
    float noiseRatio;
    int harmonics;

	std::mt19937_64 mt;
    std::uniform_real_distribution<float> rnd;

    float simpleOsc(float phase);
    float additiveOsc(float phase);

    float Sin(float phase);
    float Cos(float phase);
    float SimpleSaw(float phase, float pw); // pw: 波形を変更するパラメータ
    float SimpleSquare(float phase, float pw);
    float AdditiveSaw(float phase, float pw);
    float AdditiveSquare(float phase, float pw);
    float Noise();
};
