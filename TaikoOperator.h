#pragma once

#include "Oscillator.h"
#include "Envelope.h"
#include "Filter/MoogFilter.h"
#include "Filter/TB18dBFilter.h"
#include "Filter/KarlsenFilter.h"
#include "Filter/EMSVCS3Filter.h"
#include "Filter/LinearPhaseDCBlocker.h"


class TaikoOperator
{
public:
    enum class FilterType : int
    {
        Bypass = 0,
        Moog,
        Karlsen,
        EMSVCS3,
        MovingAverage,
        TB18dB,
        Last = TB18dB,
    };

    enum class ModulationType : int
    {
        PM = 0,
        FM,
        AM,
        Last,
    };

public:
    TaikoOperator();
    ~TaikoOperator();

    TaikoOperator& operator=(TaikoOperator& op);

    bool mute;

    Oscillator osc;
    Envelope envAmp;
    Envelope envPitch;
    Envelope envShape;
    Envelope envPhaseOffset;
    Envelope envNoise;

    Envelope envCutoff;
    Envelope envResonance;

    // filter
    FilterType filterType();
    void setFilterType(FilterType type);

    // pitch
    RangedFloat Frequency;

    // modulation
    RangedFloat ModIndex;
    RangedFloat Feedback;

    ModulationType modulationType();
    void setModulationType(ModulationType type);

    void setLFO(bool flag);
    bool isLFO();

    // render
    void setModInput(float input);
    void clearBuffer();
    void clearBuffer(float phase);
    float render(float time, float frequencyMul, float frequencyAdd, float frequencyMod);

private:
    ModulationType modType;
    float buffer;
    float phase;
    float input;
    bool lfo;

    FilterType _filterType;
    Filter * _filter;
    MoogFilter moogFilter;
    KarlsenFilter karlsenFilter;
    TB18dBFilter tb18dbFilter;
    EMSVCS3Filter emsVcs3Filter;
    LinearPhaseDCBlocker movingAverageFilter;

    float Pm(float time, float frequency);
    float Fm(float time, float frequency);
    float Am(float time, float frequency);
};

