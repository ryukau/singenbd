#pragma once

#include "Ranged.h"
#include <unordered_map>
#include <cstring>

class Envelope
{
public:
    enum class Type : int
    {
        Exponential = 0,
        Linear,
        BSpline,
        BSplineSmooth,
        Last,
    };

    enum class Parameters : int
    {
        Type = 0,
        Gain,
        Offset,
        AttackTime,
        AttackTension,
        DecayTime,
        DecayTension,
        Last,
    };

public:
    Envelope();
    ~Envelope();

    Envelope& operator=(Envelope& env);

    RangedFloat Gain;
    RangedFloat Offset;
    RangedFloat AttackTime;
    RangedFloat AttackTension;
    RangedFloat DecayTime;
    RangedFloat DecayTension;

    Type getType();
    void setType(Type type);

    void setMid();

    float at(float time);

private:
    Type envType;

    float linearCurve(float time);
    float linearCurveAttack(float time, float tension);
    float linearCurveDecay(float time, float tension);

    float expCurve(float time);
    float expCurveAttack(float time, float tension);

    float bSplineCurve(float time);
    float bSplineSmoothCurve(float time);
    float bSplineSmoothCurveAttack(float time, float tension);
};

