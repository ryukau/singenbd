#pragma once

#include <cmath>

const float PI = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214f;
const float TWOPI = PI * 2;
const float E = 2.71828182845904523536028747135266249775724709369995957496696762772407663035354759457138217852516642742746f;
const float EPS = 1e-50f;

//
// sample rate
//

class SampleRate
{
public:
    static unsigned int get() { return sampleRate; }
    static void set(unsigned int rate)
    {
        sampleRate = rate;
    }

private:
    static unsigned int sampleRate;
};

//
// pitch calculator
//

// 音程はMIDIノートナンバーに基づいて計算する
// MIDIノートナンバーが69の時、440Hzとなるので、これを使う

// cent はfloatのテーブルにできる
// 8 * 120000 ~= 960 kByte 程度なので余裕でメモリに乗る

const float intervalCC = 1.0f / 1200.0f;
const float baseFreq = 440 * pow(2.0f, -6900.0f * intervalCC);
const float bendCC = 2.0f;

class PitchCalc
{
public:
    static float intervalToFreq(float cent)
    {
        if (cent == 0.0f)
            return 0.0f;

        // 1 cent は 1 / 100 半音
        return baseFreq * pow(2.0f, cent * intervalCC);
    }

    static float bendFreq(float freq, float bend)
    {
        if (bend == 0.0f)
            return 0.0f;

        return baseFreq * pow(freq, bend * bendCC);
    }
};
