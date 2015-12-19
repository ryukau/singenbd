#include "DecayEnvelope.h"

#include <algorithm>

DecayEnvelope::DecayEnvelope()
{

}

DecayEnvelope::~DecayEnvelope()
{

}

DecayEnvelope &DecayEnvelope::operator=(DecayEnvelope &env)
{
    this->e1 = env.e1;
    this->e2 = env.e2;

    return *this;
}

float DecayEnvelope::at(float time)
{
    // e1 と e2 の係数で、エンベロープの質感をチューニングすることができる
    return std::max(0.0f, std::min(e1.at(time) + e2.at(time), 1.0f));
}

