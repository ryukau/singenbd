#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "soundplayer.h"

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
    void on_pushButtonRender_clicked();

private:
    Ui::MainWindow *ui;

    SoundPlayer soundPlayer;

    QVector<float> waveSound;
    QVector<float> waveEnvelope;

    void setupWaveforms();
    void renderSinewave();
};

#endif // MAINWINDOW_H
