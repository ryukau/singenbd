#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "soundplayer.h"
#include "FMTOComplex.h"
#include "Filter/FixedPointDCBlocker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // Render
    void on_pushButtonRender_clicked();
    void on_pushButtonPlay_clicked();
    void on_pushButtonSave_clicked();

    // Oscillator
    void on_pushButtonOscillator1_clicked();
    void on_pushButtonOscillator2_clicked();
    void on_pushButtonOscillator3_clicked();
    void on_counterPitch_valueChanged(double value);
    void on_comboBoxOscType_currentIndexChanged(const QString &arg1);
    void on_horizontalScrollBarPhase_valueChanged(int value);
    void on_horizontalScrollBarMod_valueChanged(int value);

    // Misc
    void on_spinBoxSampleRate_valueChanged(int arg1);

    // Envelope
    void on_pushButtonEnvelopeAmp_clicked();
    void on_pushButtonEnvelopeShape_clicked();
    void on_pushButtonEnvelopePitch_clicked();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    enum class EnvType : int
    {
        Amp = 0,
        Pitch,
        Shape,
        End,
        Begin = 0,
    };

    Ui::MainWindow *ui;

    // 機能
    SoundPlayer soundPlayer;
    FMTOComplex fmto;
    FixedPointDCBlocker dcBlocker;

    // 音データ
    QVector<float> waveSound;
    QVector<float> waveEnvelope;

    // UIの状態
    int curOp;
    EnvType curEnv;

    // 関数
    void playSound();
    void saveSound();
    void renderSound();
    void DcKill();
    void Declick(int declickLength);

    void setupWaveforms();
    int getNumberOfSamples();
};

#endif // MAINWINDOW_H
