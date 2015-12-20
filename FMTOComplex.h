//
// オペレータは以下のように接続される。
//
// 2
// |
// 1---+
// |   |
// 0   3(sub)
// +---+
//     |
//     FX
//     |
//     out
//

#pragma once

#include <deque>
#include "TaikoOperator.h"


class FMTOComplex
{
public:
    FMTOComplex();

    TaikoOperator &op(int number);
    void setOp(int number, TaikoOperator & op);

    float duration();
    void setDuration(float dur);

    void clearBuffer();
    float render(float time, float deltaTime);

    int maxOp();

private:
    std::deque<TaikoOperator> fmto;
    std::deque<float> out;

    float _duration;

    const int numMaxOp;
};
