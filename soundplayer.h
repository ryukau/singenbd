#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QBuffer>
#include <QtMultimedia/QAudioOutput>
#include <vector>

class SoundPlayer
{
public:
    SoundPlayer();
    ~SoundPlayer();

    void setWave(QVector<float> & wave);
    void refresh();

    void play();
    void stop();

    void SaveWaveFile(QString fileName);

signals:

public slots:

private:
    QAudioFormat format;
    QAudioOutput *output;
    QBuffer buffer;
    std::vector<char> buf;

    void setupAudioOutput();
};

#endif // SOUNDPLAYER_H
