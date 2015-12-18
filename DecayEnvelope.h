/*
 *
 * ディケイのみのエンベロープ。
 *
 * 2つのディケイが設定できる。
 * 単なる足し合わせ。
 *
 */

#pragma once

#include "Envelope.h"

class DecayEnvelope
{
public:
    DecayEnvelope();
    ~DecayEnvelope();

    at(float time);

    Envelope one;
    Envelope two;
};
