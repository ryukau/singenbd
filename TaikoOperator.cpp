#include "TaikoOperator.h"

#include <cmath>
#include <cfloat>
#include "utils.h"

TaikoOperator::TaikoOperator()
    : mute(false)
    , interval(0.0)
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
    interval = op.interval;
    modIndex = op.modIndex;

    return *this;
}


//
// render
//

float TaikoOperator::getPitch()
{
    return interval;
}

float TaikoOperator::getModIndex()
{
    return modIndex;
}

void TaikoOperator::setModInput(float input)
{
    this->input = input;
}

void TaikoOperator::setPitch(float cent)
{
    interval = cent;
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


float TaikoOperator::render(float time, float deltaTime)
{
    if (mute)
        return 0.0;

    osc.setShape(envShape.at(time));

    // PM変調
    phase += PitchCalc::intervalToFreq(interval * 100.0f + envPitch.at(time) * 12800.0f) * deltaTime;

    buffer = envAmp.at(time) * osc.Oscillation(phase + 8 * modIndex * input);

    return buffer;
}
