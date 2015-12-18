﻿#include "FMTOComplex.h"

#include <cassert>

FMTOComplex::FMTOComplex() :
    _duration(1.0),
    fmto(numMaxOp),
    out(numMaxOp)
{
}


TaikoOperator & FMTOComplex::op(int number)
{
    assert(0 <= number && number < numMaxOp);
    return fmto[number];
}


void FMTOComplex::setOp(int number, TaikoOperator & op)
{
    assert(0 <= number && number < numMaxOp);
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


float FMTOComplex::render(float time)
{
    fmto[0].setModInput(out[1]);
    fmto[1].setModInput(out[2]);
    //fmto[2].setModInput();

    for (int i = 0; i < fmto.size(); ++i)
    {
        out[i] = fmto[i].render(time / _duration);
    }

    return out[0];
}
