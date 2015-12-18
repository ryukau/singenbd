#include "TaikoOperator.h"

#include <cmath>
#include <cfloat>
#include "PitchCalc.h"
#include "Globals.h"

TaikoOperator::TaikoOperator() :
    mute(false),
    Frequency(0.0, -FLT_MAX, FLT_MAX),
    ModIndex(0.0),
    Feedback(0.0),
    buffer(0.0),
    phase(0.0),
    input(0.0),
    lfo(false),
    modType(ModulationType::PM),
    _filterType(FilterType::Bypass),
    _filter(nullptr)
{
}


TaikoOperator::~TaikoOperator()
{
}


TaikoOperator& TaikoOperator::operator=(TaikoOperator &op)
{
    mute = op.mute;
    osc = op.osc;
    envAmp = op.envAmp;
    envPitch = op.envPitch;
    envShape = op.envShape;
    envPhaseOffset = op.envPhaseOffset;
    envNoise = op.envNoise;
    envCutoff = op.envCutoff;
    envResonance = op.envResonance;
    setFilterType(op.filterType());
    Frequency = op.Frequency;
    Feedback = op.Feedback;
    ModIndex = op.ModIndex;

    return *this;
}


//
// filter
//

void TaikoOperator::setFilterType(FilterType type)
{
    switch (type)
    {
    case FilterType::Moog:
        _filter = &moogFilter;
        break;

    case FilterType::Karlsen:
        _filter = &karlsenFilter;
        break;

    case FilterType::EMSVCS3:
        _filter = &emsVcs3Filter;
        break;

    case FilterType::MovingAverage:
        _filter = &movingAverageFilter;
        break;

    case FilterType::TB18dB:
        _filter = &tb18dbFilter;
        break;

    case FilterType::Bypass:
    default:
        _filter = nullptr;
    }

    _filterType = type;
}

TaikoOperator::ModulationType TaikoOperator::modulationType()
{
    return modType;
}

void TaikoOperator::setModulationType(TaikoOperator::ModulationType type)
{
    modType = type;
}

void TaikoOperator::setLFO(bool flag)
{
    lfo = flag;
}

bool TaikoOperator::isLFO()
{
    return lfo;
}


TaikoOperator::FilterType TaikoOperator::filterType()
{
    return _filterType;
}


//
// render
//

void TaikoOperator::setModInput(float input)
{
    this->input = input;
}

void TaikoOperator::clearBuffer()
{
    buffer = 0.0;
    phase = 0.0;
    input = 0.0;

    if (RangedFloat::state == RangedFloat::State::Mid)
    {
        Frequency.setMid();
        ModIndex.setMid();
        Feedback.setMid();

        envAmp.setMid();
        envPitch.setMid();
        envShape.setMid();
        envPhaseOffset.setMid();
        envNoise.setMid();
        envCutoff.setMid();
        envResonance.setMid();
    }

    moogFilter.clearBuffer();
    tb18dbFilter.clearBuffer();
    karlsenFilter.clearBuffer();
    emsVcs3Filter.clearBuffer();
}

void TaikoOperator::clearBuffer(float phase)
{
    clearBuffer();
    this->phase = phase;
}

float TaikoOperator::render(float time, float frequencyMul, float frequencyAdd, float frequencyMod)
{
    if (mute || (envAmp.AttackTime.get() + envAmp.DecayTime.get()) < time)
        return 0.0;

    osc.setShape(envShape.at(time));
    osc.setPhaseOffset(envPhaseOffset.at(time));
    osc.setNoiseRatio(envNoise.at(time));

    float frequency;
    if (lfo)
        frequency = Frequency.get() + PitchCalc::intervalToFreq(envPitch.at(time) * 10000);
    else
        frequency = fmod((frequencyMul * Frequency.get()) + frequencyAdd + PitchCalc::intervalToFreq(envPitch.at(time) * 10000), frequencyMod);

    switch (modType)
    {
    case ModulationType::PM:
        buffer = Pm(time, frequency);
        break;
    case ModulationType::FM:
        buffer = Fm(time, frequency);
        break;
    case ModulationType::AM:
        buffer = Am(time, frequency);
        break;
    }

    if (_filter)
    {
        _filter->setCutoff(envCutoff.at(time));
        _filter->setResonance(envResonance.at(time));
        buffer = _filter->pass(buffer);
    }

    return buffer;
}


//
// private
//
float TaikoOperator::Pm(float time, float frequency)
{
    float modIndex = 8.0f * ModIndex.get();//pow(\d+\.\d+f, ModIndex.get()) - \d+\.\d+f;
    float feedback = pow(1.5f, Feedback.get()) - 1.0f;

    phase += frequency / Globals::SampleRate();
    return envAmp.at(time) * osc.Oscillation(phase + modIndex * input + feedback * buffer);
}

float TaikoOperator::Fm(float time, float frequency)
{
    float modIndex = frequency * ModIndex.get();//pow(frequency, ModIndex.get());//\d+\.\d+f * ModIndex.get();
    float feedback = frequency * 0.99f * Feedback.get();

    phase += (frequency + (modIndex * input + feedback * buffer)) / Globals::SampleRate();
    return envAmp.at(time) * osc.Oscillation(phase); // fm
}

float TaikoOperator::Am(float time, float frequency)
{
    float feedback = frequency * 0.99f * Feedback.get();

    phase += (frequency + feedback * buffer) / Globals::SampleRate();

    float car = envAmp.at(time) * osc.Oscillation(phase);
    float mod = input;

    return ModIndex.get() * car * mod + (1.0f - ModIndex.get()) * car;
}
