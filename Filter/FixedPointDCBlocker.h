#pragma once

#include "Filter.h"
#include <vector>


class FixedPointDCBlocker : public Filter
{
public:
    FixedPointDCBlocker();

    float pass(float input);
    void clearBuffer();

private:
    float x, y;
};
