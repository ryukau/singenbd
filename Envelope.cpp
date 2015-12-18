#include "Envelope.h"

#include <cmath>
#include <cassert>
#include "Globals.h"
#include "BSplineCurve.h"

const float declickLength = 0.001f; // 単位は[msec]
const float expEps = 0.001f; // expCurveの計算終了値


Envelope::Envelope() :
    Gain(0.0, -1.0, 1.0),
    Offset(0.0),
    AttackTime(0.5),
    AttackTension(0.5),
    DecayTime(0.5),
    DecayTension(0.5),
    envType(Type::Exponential)
{
}


Envelope::~Envelope()
{
}


Envelope& Envelope::operator=(Envelope &env)
{
    Gain = env.Gain;
    Offset = env.Offset;
    AttackTime = env.AttackTime;
    AttackTension = env.AttackTension;
    DecayTime = env.DecayTime;
    DecayTension = env.DecayTension;
    setType(env.getType());

    return *this;
}


Envelope::Type Envelope::getType()
{
    return envType;
}


void Envelope::setType(Envelope::Type type)
{
    envType = type;
}


void Envelope::setMid()
{
    Gain.setMid();
    Offset.setMid();
    AttackTime.setMid();
    AttackTension.setMid();
    DecayTime.setMid();
    DecayTension.setMid();
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

    return std::max(0.0f, std::min(Gain.get() * out + Offset.get(), 1.0f));
}


//
// 直線のエンベロープ
//
float Envelope::linearCurve(float time)
{
    if (time < AttackTime.get())
    {
        return linearCurveAttack(time / AttackTime.get(), AttackTension.get());
    }
    else if (time < (AttackTime.get() + DecayTime.get()))
    {
        return linearCurveDecay((time - AttackTime.get()) / DecayTime.get(), DecayTension.get());
    }
    else
    {
        return 0.0f;
    }
}

float Envelope::linearCurveAttack(float time, float tension)
{
    // normalized tension
    float s = tension / (1.0f - tension);

    if (time <= 1.0f - tension)
        return s * time;
    else
        return (time - 1.0f) / s + 1.0f;
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
    if (time < AttackTime.get())
    {
        return expCurveAttack(time / AttackTime.get(), AttackTension.get());
    }
    else if (time < (AttackTime.get() + DecayTime.get()))
    {
        return expCurveAttack(1.0f - (time - AttackTime.get()) / DecayTime.get(), DecayTension.get());
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
    if (time < AttackTime.get())
    {
        return BSplineCurve::get(time / AttackTime.get(), AttackTension.get()) * 0.01f;
    }
    else if (time < (AttackTime.get() + DecayTime.get()))
    {
        return BSplineCurve::get(1.0f - (time - AttackTime.get()) / DecayTime.get(), DecayTension.get()) * 0.01f;
    }
    else
    {
        return 0.0f;
    }
}

float Envelope::bSplineSmoothCurve(float time)
{
    if (time < AttackTime.get())
    {
        return bSplineSmoothCurveAttack(time / AttackTime.get(), AttackTension.get()) * 0.01f;
    }
    else if (time < (AttackTime.get() + DecayTime.get()))
    {
        return bSplineSmoothCurveAttack(1.0f - (time - AttackTime.get()) / DecayTime.get(), DecayTension.get()) * 0.01f;
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


