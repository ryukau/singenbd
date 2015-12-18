#pragma once

#include <algorithm>

class Filter
{
public:
    virtual float pass(float input) = 0;
    virtual void clearBuffer() {}

    virtual float Cutoff() { return cutoff; }
    virtual float Resonance() { return resonance; }

    virtual void setCutoff(float fc)
    {
        cutoff = std::min(std::max(0.0f, fc), 1.0f);
    }

    virtual void setResonance(float reso)
    {
        resonance = std::min(std::max(0.0f, reso), 1.0f);
    }

protected:
    float cutoff;
    float resonance;
};
