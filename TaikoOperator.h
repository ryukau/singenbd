#pragma once

#include "Oscillator.h"
#include "DecayEnvelope.h"

class TaikoOperator
{
public:
    TaikoOperator();
    ~TaikoOperator();

    TaikoOperator& operator=(TaikoOperator& op);

    bool mute;

    Oscillator osc;
    DecayEnvelope envAmp;
    DecayEnvelope envPitch;
    DecayEnvelope envShape;

    float getPitch();
    float getModIndex();
    void setModInput(float input);
    void setPitch(float cent);
    void setModIndex(float index);
    void clearBuffer();
    void clearBuffer(float phase);
    float render(float time);

private:
    float buffer;
    float phase;
    float input;
    float interval;
    float modIndex;
};

