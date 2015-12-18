#pragma once


class Oscillator
{
public:
    enum class OscillatorType : int
    {
        Sin = 0,
        Saw,
        Square,
        Last,
    };

public:
    Oscillator();
    ~Oscillator();

    Oscillator & operator=(Oscillator& osc);

    // getter setter
    OscillatorType getType();
    float getShape();
    float getPhaseOffset();
    void setType(OscillatorType oscType);
    void setShape(float shape);
    void setPhaseOffset(float phase);

    //
    float Oscillation(float phase);

private:
    OscillatorType type;
    float shape;
    float phaseOffset;

    float Sin(float phase);
    float SimpleSaw(float phase, float pw); // pw: 波形を変更するパラメータ
    float SimpleSquare(float phase, float pw);
};
