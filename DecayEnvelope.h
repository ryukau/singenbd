﻿/*
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

    DecayEnvelope& operator=(DecayEnvelope& env);

    float at(float time);

    Envelope e1;
    Envelope e2;
};
