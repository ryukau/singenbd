//
// オペレータは以下のように接続される。
//
// 2   5-+ 8-+
// |   | | | |
// 1   4-+ 7 |
// |   |   | |
// 0   3   6-+  左から順にtower1, tower2, tower3。
// +---+---+
//     |
//     FX       エフェクト（未実装）
//     |
//     out
//
//
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
    float render(float time, float frequencyMul, float frequencyAdd, float frequencyMod);

private:
    std::deque<TaikoOperator> fmto;
    std::deque<float> out;

    float _duration;
};
