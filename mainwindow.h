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
    void on_counterDuration_valueChanged(double value);
    void on_spinBoxSampleRate_valueChanged(int arg1);

    // Envelope
    void on_pushButtonEnvelopeAmp_clicked();
    void on_pushButtonEnvelopeShape_clicked();
    void on_pushButtonEnvelopePitch_clicked();
    void on_horizontalScrollBarD1Gain_valueChanged(int value);
    void on_horizontalScrollBarD1Time_valueChanged(int value);
    void on_horizontalScrollBarD1Tension_valueChanged(int value);
    void on_comboBoxD1Type_currentIndexChanged(const QString &arg1);
    void on_horizontalScrollBarD2Gain_valueChanged(int value);
    void on_horizontalScrollBarD2Time_valueChanged(int value);
    void on_horizontalScrollBarD2Tension_valueChanged(int value);
    void on_comboBoxD2Type_currentIndexChanged(const QString &arg1);


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
    void saveSettings(QString fileName);

    void playSound();
    void saveSound();
    void renderSound();
    void DcKill();
    void Declick(int declickLength);

    void setupWaveforms();
    int getNumberOfSamples();
    float normalizeSliderInput(int value, int maximum);
    int normalizeSliderValue(float value, int maximum);

    void setCurrentOperator(int op);
    void refreshOscillator();

    void setCurrentEnvelope(EnvType type);
    DecayEnvelope &getEnvelopeType(int op, EnvType type);
    void refreshEnvelope(EnvType type);
    void refreshWaveformEnvelope(EnvType type);
};

#endif // MAINWINDOW_H
