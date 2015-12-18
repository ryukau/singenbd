#include "soundplayer.h"

#include <QVector>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include "utils.h"

SoundPlayer::SoundPlayer() :
    output(nullptr)
{
    setupAudioOutput();
}


SoundPlayer::~SoundPlayer()
{
    delete output;
}


void SoundPlayer::setWave(QVector<float> &wave)
{
    // qtEndianが浮動小数点に対応していないのでvector<char>を利用
    buf.resize(wave.size() * sizeof(float));
    float* p = (float*)&buf[0];

    for (int i = 0; i < wave.size(); i++)
    {
        *p = static_cast<float>(wave.at(i));
        p++;
    }

    if (buffer.isOpen())
        buffer.close();

    buffer.setData(&buf[0], static_cast<int>(buf.size()));
}

void SoundPlayer::refresh()
{
    setupAudioOutput();
}


void SoundPlayer::play()
{
    buffer.open(QIODevice::ReadOnly);
    output->start(&buffer);
}


void SoundPlayer::stop()
{
    output->stop();
    buffer.reset();
}


void SoundPlayer::setupAudioOutput()
{
    format.setSampleRate(SampleRate::get()); // sample rate
    format.setChannelCount(1);
    format.setSampleSize(32);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::Float);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
       qWarning() << "Raw audio format not supported by backend, cannot play audio.";
       return;
    }

    if (output)
    {
        delete output;
        output = nullptr;
    }

    output = new QAudioOutput(format);
}


void SoundPlayer::SaveWaveFile(QString fileName)
{
    //
    // 32 bit float のみ対応
    //

    // ファイルへの書き込み
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QDataStream wavfs(&file);
    wavfs.setByteOrder(QDataStream::LittleEndian);

    //
    // riffCkSize =          // RIFFチャンクの ckSize は次のように決まる
    //     + sizeof("RIFF")
    //     + 4               // fileSize自身の大きさ（4byte）
    //     + sizeof("WAVE")  // fileSizeにはこれ以降の大きさを格納する。
    //     + sizeof("fmt ")
    //     + fmtCkSize       // _xx_CkSize には自身の大きさが含まれていない。
    //     + sizeof("fact")  // フォーマットがPCMの場合はfactチャンク無し。
    //     + factCkSize
    //     + sizeof("data")
    //     + dataCkSize;
    //
    unsigned int fmtCkSize = 18;
    unsigned int factCkSize = 4;
    unsigned int riffCkSize = 50 + static_cast<unsigned int>(buf.size());

    wavfs << (qint32)'FFIR';
    wavfs << (qint32)riffCkSize;
    wavfs << (qint32)'EVAW';
    wavfs << (qint32)' tmf';
    wavfs << (qint32)fmtCkSize;
    wavfs << (qint16)0x0003; // wFormatTag: IEEE 32bit float
    wavfs << (qint16)format.channelCount(); // nChannels
    wavfs << (qint32)format.sampleRate(); // nSamplesPerSec
    wavfs << (qint32)(format.sampleRate() * (format.sampleSize() / 8) * format.channelCount()); // nAvgBytesPerSec
    wavfs << (qint16)((format.sampleSize() / 8) * format.channelCount()); // nBlockAlign
    wavfs << (qint16)format.sampleSize(); // wBitsPerSample

    wavfs << (qint16)0x0000; // cbSize
    wavfs << (qint32)'tcaf';
    wavfs << (qint32)factCkSize;
    wavfs << (qint32)(buf.size() / (format.sampleSize() / 8) / format.channelCount()); // dwSampleLength

    wavfs << (qint32)'atad';
    wavfs << (qint32)buf.size();

    wavfs << buffer.buffer();
}

