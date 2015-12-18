#include "FMTOComplex.h"

#include <cassert>

FMTOComplex::FMTOComplex() :
    _duration(1.0),
    fmto(9),
    out(9)
{
}


TaikoOperator & FMTOComplex::op(int number)
{
    assert(0 <= number && number < 9);
    return fmto[number];
}


void FMTOComplex::setOp(int number, TaikoOperator & op)
{
    assert(0 <= number && number < 9);
    fmto[number] = op;
}


float FMTOComplex::duration()
{
    return _duration;
}


void FMTOComplex::setDuration(float dur)
{
    _duration = dur;
}


void FMTOComplex::clearBuffer()
{
    for (auto &it : fmto)
    {
        it.clearBuffer();
    }
}


float FMTOComplex::render(float time, float frequencyMul, float frequencyAdd, float frequencyMod)
{
    fmto[0].setModInput(out[1]);
    fmto[1].setModInput(out[2]);
    //fmto[2].setModInput();
    fmto[3].setModInput(out[4]);
    fmto[4].setModInput(out[5]);
    fmto[5].setModInput(out[4]);
    fmto[6].setModInput(out[7]);
    fmto[7].setModInput(out[8]);
    fmto[8].setModInput(out[6]);

    for (int i = 0; i < fmto.size(); ++i)
    {
        out[i] = fmto[i].render(time / _duration, frequencyMul, frequencyAdd, frequencyMod);
    }

    return (out[0] + out[3] + out[6]) / 3.0f;//out[0];
}
