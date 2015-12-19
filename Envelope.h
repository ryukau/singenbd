#pragma once


class Envelope
{
public:
    enum class Type : int
    {
        BSplineSmooth = 0,
        BSpline,
        Exponential,
        Linear,
        End,
        Begin = 0,
    };

public:
    Envelope();
    ~Envelope();

    Envelope& operator=(Envelope& env);


    Type getType();
    float getGain();
    float getDecayTime();
    float getDecayTension();
    void setType(Type type);
    void setGain(float gain);
    void setDecayTime(float time);
    void setDecayTension(float tension);

    float at(float time);

private:
    Type envType;
    float Gain;
    float DecayTime;
    float DecayTension;

    float linearCurve(float time);
    float linearCurveDecay(float time, float tension);

    float expCurve(float time);
    float expCurveAttack(float time, float tension);

    float bSplineCurve(float time);

    float bSplineSmoothCurve(float time);
    float bSplineSmoothCurveAttack(float time, float tension);
};

