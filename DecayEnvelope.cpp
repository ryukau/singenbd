#include "DecayEnvelope.h"

#include <algorithm>

DecayEnvelope::DecayEnvelope()
{

}

DecayEnvelope::~DecayEnvelope()
{

}

DecayEnvelope::at(float time)
{
    return std::max(0.0f, std::min(e1.at(time) + e2.at(time), 1.0f));
}

