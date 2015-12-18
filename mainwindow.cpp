#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QDateTime>
#include "utils.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , curOp(0)
    , curEnv(EnvType::Amp)
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


// render //

void MainWindow::on_pushButtonRender_clicked()
{
    renderSound();

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


// Oscillator //

void MainWindow::on_pushButtonOscillator1_clicked()
{
    curOp = 0;
}

void MainWindow::on_pushButtonOscillator2_clicked()
{
    curOp = 1;
}

void MainWindow::on_pushButtonOscillator3_clicked()
{
    curOp = 2;
}

void MainWindow::on_counterPitch_valueChanged(double value)
{
    fmto.op(curOp).setPitch(value);
}

void MainWindow::on_comboBoxOscType_currentIndexChanged(const QString &arg1)
{
    if (arg1.contains("Sin"))
        fmto.op(curOp).osc.setType(Oscillator::OscillatorType::Sin);
    else if (arg1.contains("Saw"))
        fmto.op(curOp).osc.setType(Oscillator::OscillatorType::Saw);
    else if (arg1.contains("Square"))
        fmto.op(curOp).osc.setType(Oscillator::OscillatorType::Square);
}

void MainWindow::on_horizontalScrollBarPhase_valueChanged(int value)
{
    fmto.op(curOp).osc.setPhaseOffset(value / (float)ui->horizontalScrollBarPhase->maximum());
}

void MainWindow::on_horizontalScrollBarMod_valueChanged(int value)
{
    fmto.op(curOp).setModIndex(value / (float)ui->horizontalScrollBarMod->maximum());
}


// Misc //

void MainWindow::on_spinBoxSampleRate_valueChanged(int arg1)
{
    SampleRate::set(arg1);
}


// Envelope //

void MainWindow::on_pushButtonEnvelopeAmp_clicked()
{
    curEnv = EnvType::Amp;
}

void MainWindow::on_pushButtonEnvelopeShape_clicked()
{
    curEnv = EnvType::Shape;
}

void MainWindow::on_pushButtonEnvelopePitch_clicked()
{
    curEnv = EnvType::Pitch;
}


//
// protected events
//

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    waveEnvelope.resize(ui->waveformEnvelope->width());
    ui->waveformEnvelope->refresh();
}


//
// private
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

void MainWindow::renderSound()
{
    int numSamples = getNumberOfSamples();

    const int minSamples = 200;
    if (numSamples < minSamples)
        numSamples = minSamples; // 最低サンプル数の保証

    fmto.clearBuffer();
    waveSound.resize(numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        waveSound[i] = fmto.render((float)i / SampleRate::get());
        //waveSound[i] = sin(2.0f * PI * 100.0f * i / SampleRate::get());
    }

    if (ui->checkBoxDcBlock->isChecked())
        DcKill();

    if (ui->checkBoxDeclick->isChecked())
        Declick(minSamples);
}

void MainWindow::DcKill()
{
    dcBlocker.clearBuffer();

    for (int sample = 0; sample < waveSound.size(); ++sample)
    {
        waveSound[sample] = dcBlocker.pass(waveSound[sample]);
    }
}

void MainWindow::Declick(int declickLength)
{
    for (int i = 0; i < declickLength && i < waveSound.size(); ++i)
    {
        float ratio = (float)i / declickLength;
        waveSound[i] = ratio * waveSound[i];
    }

    for (int i = 0; i < declickLength; ++i)
    {
        int index = waveSound.size() - i - 1;
        if (index < 0)
            break;

        float ratio = (float)i / declickLength;
        waveSound[index] = ratio * waveSound[index];
    }
}

void MainWindow::setupWaveforms()
{
    ui->waveformMain->setWave(&waveSound);
    ui->waveformEnvelope->setWave(&waveEnvelope);
}

int MainWindow::getNumberOfSamples()
{
    // counterDuration->valueの単位は秒
    return static_cast<int>(ui->counterDuration->value() * SampleRate::get());
}
