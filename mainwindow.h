#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "soundplayer.h"
#include "FMTOComplex.h"

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

    void playSound();
    void saveSound();
    void renderSound();

    void on_pushButtonRender_clicked();
    void on_pushButtonSave_clicked();

    void on_pushButtonPlay_clicked();

private:
    Ui::MainWindow *ui;

    // 機能
    SoundPlayer soundPlayer;
    FMTOComplex fmto;

    // 音データ
    QVector<float> waveSound;
    QVector<float> waveEnvelope;

    // 関数
    void setupWaveforms();
    int getNumberOfSamples();
};

#endif // MAINWINDOW_H
