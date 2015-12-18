#include "FixedPointDCBlocker.h"

FixedPointDCBlocker::FixedPointDCBlocker()
    : x(0.0)
    , y(0.0)
{
    setCutoff(0.9912f);
}


//
// http://www.digitalsignallabs.com/dcblock.pdf の Fixed-Point DC Blocker
//
float FixedPointDCBlocker::pass(float input)
{
    float w = input - x;
    y = cutoff * y + w;

    // replace buffer
    x = input;

    return y;
}

void FixedPointDCBlocker::clearBuffer()
{
    x = 0.0f;
    y = 0.0f;
}

