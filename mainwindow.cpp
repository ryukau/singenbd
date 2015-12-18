#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QDateTime>
#include "utils.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupWaveforms();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//
// private slots
//

void MainWindow::playSound()
{
    soundPlayer.play();
}

void MainWindow::saveSound()
{
    QString dirNameRender = "render/";

    // 保存先のディレクトリが無ければ作る
    QDir dir;
    if (!dir.exists(dirNameRender))
        dir.mkdir(dirNameRender);

    // wavの保存
    QString name;
    name.append(dirNameRender);
    name.append(ui->lineEditFilePrefix->text());
    name.append(QDateTime::currentDateTime().toString("_yyyy.MM.dd_hh.mm.ss.zzz"));
    name.append(".wav");
    soundPlayer.SaveWaveFile(name);
}

void MainWindow::on_pushButtonRender_clicked()
{
    renderSinewave();

    soundPlayer.setWave(waveSound);
    ui->waveformMain->refresh();
    playSound();
}

void MainWindow::on_pushButtonPlay_clicked()
{
    playSound();
}

void MainWindow::on_pushButtonSave_clicked()
{
    saveSound();
}

//
// private
//

void MainWindow::setupWaveforms()
{
    ui->waveformMain->setWave(&waveSound);

    waveEnvelope.resize(ui->waveformEnvelope->width()); // ここにバグがある
    ui->waveformEnvelope->setWave(&waveEnvelope);
}

void MainWindow::renderSinewave()
{
    int numSamples = 48000;

    waveSound.resize(numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        waveSound[i] = sin(2.0f * PI * 100.0f * i / SampleRate::get());
    }
}


