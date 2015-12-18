#pragma once

#include "Oscillator.h"
#include "Envelope.h"

class TaikoOperator
{
public:
    TaikoOperator();
    ~TaikoOperator();

    TaikoOperator& operator=(TaikoOperator& op);

    bool mute;

    Oscillator osc;
    Envelope envAmp;
    Envelope envPitch;
    Envelope envShape;

    void setModInput(float input);
    void setFrequency(float freq);
    void setModIndex(float index);
    void clearBuffer();
    void clearBuffer(float phase);
    float render(float time);

private:
    float buffer;
    float phase;
    float input;
    float frequency;
    float modIndex;
};

