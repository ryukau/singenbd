#include "TaikoOperator.h"

#include <cmath>
#include <cfloat>
#include "utils.h"

TaikoOperator::TaikoOperator()
    : mute(false)
    , frequency(0.0)
    , modIndex(0.0)
    , buffer(0.0)
    , phase(0.0)
    , input(0.0)
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
    frequency = op.frequency;
    modIndex = op.modIndex;

    return *this;
}


//
// render
//

void TaikoOperator::setModInput(float input)
{
    this->input = input;
}

void TaikoOperator::setFrequency(float freq)
{
    frequency = freq;
}

void TaikoOperator::setModIndex(float index)
{
    modIndex = index;
}


void TaikoOperator::clearBuffer()
{
    buffer = 0.0;
    phase = 0.0;
    input = 0.0;
}


void TaikoOperator::clearBuffer(float phase)
{
    clearBuffer();
    this->phase = phase;
}


float TaikoOperator::render(float time)
{
    if (mute)
        return 0.0;

    osc.setShape(envShape.at(time));

    // PM変調
    phase += (frequency + (PitchCalc::intervalToFreq(envPitch.at(time) * 24000))) / SampleRate::get();

    buffer = envAmp.at(time) * osc.Oscillation(phase + 8 * modIndex * input);

    return buffer;
}
