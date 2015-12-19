#include "Envelope.h"

#include <cmath>
#include <cassert>
#include "BSplineCurve.h"

const float declickLength = 0.001f; // 単位は[msec]
const float expEps = 0.001f; // expCurveの計算終了値


Envelope::Envelope()
    : Gain(0.0f)
    , DecayTime(1.0f)
    , DecayTension(0.2f)
    , envType(Type::BSplineSmooth)
{
}


Envelope::~Envelope()
{
}


Envelope& Envelope::operator=(Envelope &env)
{
    Gain = env.getGain();
    DecayTime = env.getDecayTime();
    DecayTension = env.getDecayTension();
    setType(env.getType());

    return *this;
}


Envelope::Type Envelope::getType()
{
    return envType;
}

float Envelope::getGain()
{
    return Gain;
}

float Envelope::getDecayTime()
{
    return DecayTime;
}

float Envelope::getDecayTension()
{
    return DecayTension;
}


void Envelope::setType(Envelope::Type type)
{
    envType = type;
}

void Envelope::setGain(float gain)
{
    Gain = gain;
}

void Envelope::setDecayTime(float time)
{
    DecayTime = time;
}

void Envelope::setDecayTension(float tension)
{
    DecayTension = tension;
}


float Envelope::at(float time)
{
    float out = 0.0;

    switch (envType)
    {
    case Type::Linear:
        out = linearCurve(time);
        break;

    case Type::Exponential:
        out = expCurve(time);
        break;

    case Type::BSpline:
        out = bSplineCurve(time);
        break;

    case Type::BSplineSmooth:
        out = bSplineSmoothCurve(time);
        break;

    default:
        assert(0);
        break;
    }

    return Gain * out;
}


//
// 直線のエンベロープ
//
float Envelope::linearCurve(float time)
{
    if (time < DecayTime)
    {
        return linearCurveDecay(time / DecayTime, DecayTension);
    }
    else
    {
        return 0.0f;
    }
}

float Envelope::linearCurveDecay(float time, float tension)
{
    // normalized tension
    float s = tension / (1.0f - tension);

    if (time < tension)
        return 1.0f - time / s;
    else
        return s * (1.0f - time);
}


//
// 指数エンベロープ
//
float Envelope::expCurve(float time)
{
    if (time < DecayTime)
    {
        return expCurveAttack(1.0f - time / DecayTime, DecayTension);
    }
    else
    {
        return 0.0f;
    }
}

float Envelope::expCurveAttack(float time, float tension)
{
    if (tension < 0.5f)
        return 2.0f * (tension * time + (0.5f - tension) * exp((1.0f - time) * std::log(expEps)));
    else
        return 2.0f * ((1.0f - tension) * time + (tension - 0.5f) * (1.0f - exp(time * std::log(expEps))));
}


//
// B-Spline エンベロープ
//
float Envelope::bSplineCurve(float time)
{
    if (time < DecayTime)
    {
        return BSplineCurve::get(1.0f - time / DecayTime, DecayTension) * 0.01f;
    }
    else
    {
        return 0.0f;
    }
}

float Envelope::bSplineSmoothCurve(float time)
{
    if (time < DecayTime)
    {
        return bSplineSmoothCurveAttack(1.0f - time / DecayTime, DecayTension) * 0.01f;
    }
    else
    {
        return 0.0f;
    }
}

float Envelope::bSplineSmoothCurveAttack(float time, float tension)
{
    float nTime = time * 100.0f;
    float ratio = (nTime - floor(nTime));

    return ratio * BSplineCurve::get(time + 0.01f, tension) + (1.0f - ratio) * BSplineCurve::get(time, tension);
}


