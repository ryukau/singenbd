#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <random>
#include <QDir>
#include <QDateTime>
#include <QSettings>
#include <QFileDialog>
#include "utils.h"

const QString defaultIniFile = "temp.ini";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , curOp(0)
    , curEnv(EnvType::Amp)
    , tempValue(0.0)
    , superSampling(1)
{
    ui->setupUi(this);

    // initialzie randomizeSettingsEnvelope
    randomizeSettingsEnvelope.resize((int)EnvType::End);
    for (auto &it : randomizeSettingsEnvelope)
        it.resize((int)EnvParams::End); // Envelope部の8つのパラメータ

    //
    setupWaveforms();
    setupContextMenu();

    // load ini
    if (!QFile::exists(defaultIniFile))
        saveSettings(defaultIniFile);

    loadSettings(defaultIniFile);
}

MainWindow::~MainWindow()
{
    saveSettings(defaultIniFile);
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

void MainWindow::on_pushButtonRandom_clicked()
{
    //std::random_device rnd; // DebugビルドにするとここでR6010がでてabort()
    std::mt19937 mt(10);//rnd());

    std::uniform_real_distribution<float> dist(0.0, 1.0);
    std::uniform_real_distribution<float> distPitch(0.0, 128.0);
    std::uniform_int_distribution<int> distEnvelopeType((int)Envelope::Type::Begin, (int)Envelope::Type::End - 1);
    std::uniform_int_distribution<int> distOscType((int)Oscillator::OscillatorType::Begin, (int)Oscillator::OscillatorType::End - 1);

    if (ui->checkBoxDelayTime->isChecked())
        ui->horizontalScrollBarDelayTime->setValue(dist(mt) * ui->horizontalScrollBarDelayTime->maximum());

    if (ui->checkBoxDelayDecay->isChecked())
        ui->horizontalScrollBarDelayDecay->setValue(dist(mt) * ui->horizontalScrollBarDelayDecay->maximum());

    if (ui->checkBoxClipGain->isChecked())
        ui->horizontalScrollBarClipGain->setValue(dist(mt) * ui->horizontalScrollBarClipGain->maximum());

    if (ui->checkBoxClipDecay->isChecked())
        ui->horizontalScrollBarClipDecay->setValue(dist(mt) * ui->horizontalScrollBarClipDecay->maximum());

    if (ui->checkBoxDuration->isChecked())
    {
        std::uniform_real_distribution<float> distDuration(0.0, 4.0);
        fmto.setDuration(distDuration(mt));
    }

    if (ui->checkBoxSampleRate->isChecked())
    {
        std::uniform_int_distribution<int> distSampleRate(100, 48000);
        SampleRate::set(distSampleRate(mt));
    }

    if (ui->checkBoxSuperSampling->isChecked())
    {
        std::uniform_int_distribution<int> distSuperSampling((int)SuperSampling::Begin, (int)SuperSampling::End - 1);
        ui->comboBoxSuperSampling->setCurrentIndex(distSuperSampling(mt));
    }

    for (int op = 0; op < fmto.maxOp(); ++op)
    {
        if (ui->checkBoxPitch->isChecked())
            fmto.op(op).setPitch(distPitch(mt));

        if (ui->checkBoxOscType->isChecked())
            fmto.op(op).osc.setType((Oscillator::OscillatorType)distOscType(mt));

        if (ui->checkBoxPhase->isChecked())
            fmto.op(op).osc.setPhaseOffset(dist(mt));

        if (ui->checkBoxPhase->isChecked())
            fmto.op(op).setModIndex(dist(mt));

        for (int env = (int)EnvType::Begin; env < (int)EnvType::End; ++env)
        {
            if(randomizeSettingsEnvelope[env][(int)EnvParams::D1Gain]   )
                getEnvelopeType(op, (EnvType)env).e1.setGain(dist(mt));

            if(randomizeSettingsEnvelope[env][(int)EnvParams::D1Time]   )
                getEnvelopeType(op, (EnvType)env).e1.setDecayTime(dist(mt));

            if(randomizeSettingsEnvelope[env][(int)EnvParams::D1Tension])
                getEnvelopeType(op, (EnvType)env).e1.setDecayTension(dist(mt));

            if(randomizeSettingsEnvelope[env][(int)EnvParams::D1Type]   )
                getEnvelopeType(op, (EnvType)env).e1.setType((Envelope::Type)distEnvelopeType(mt));

            if(randomizeSettingsEnvelope[env][(int)EnvParams::D2Gain]   )
                getEnvelopeType(op, (EnvType)env).e2.setGain(dist(mt));

            if(randomizeSettingsEnvelope[env][(int)EnvParams::D2Time]   )
                getEnvelopeType(op, (EnvType)env).e2.setDecayTime(dist(mt));

            if(randomizeSettingsEnvelope[env][(int)EnvParams::D2Tension])
                getEnvelopeType(op, (EnvType)env).e2.setDecayTension(dist(mt));

            if(randomizeSettingsEnvelope[env][(int)EnvParams::D2Type]   )
                getEnvelopeType(op, (EnvType)env).e2.setType((Envelope::Type)distEnvelopeType(mt));
        }
    }

    refresh();
}


// Oscillator //

void MainWindow::on_pushButtonOscillator1_clicked()
{
    setCurrentOperator(0);
}

void MainWindow::on_pushButtonOscillator2_clicked()
{
    setCurrentOperator(1);
}

void MainWindow::on_pushButtonOscillator3_clicked()
{
    setCurrentOperator(2);
}

void MainWindow::on_pushButtonOscillatorSub_clicked()
{
    setCurrentOperator(3);
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

void MainWindow::on_checkBoxMute_toggled(bool checked)
{
    fmto.op(curOp).mute = checked;
}


// FX //

void MainWindow::on_horizontalScrollBarDelayTime_valueChanged(int value)
{

}

void MainWindow::on_horizontalScrollBarDelayDecay_valueChanged(int value)
{

}

void MainWindow::on_horizontalScrollBarClipGain_valueChanged(int value)
{

}

void MainWindow::on_horizontalScrollBarClipDecay_valueChanged(int value)
{

}


// Misc //

void MainWindow::on_counterDuration_valueChanged(double value)
{
    fmto.setDuration(value);
}

void MainWindow::on_spinBoxSampleRate_valueChanged(int arg1)
{
    SampleRate::set(arg1);
}

void MainWindow::on_comboBoxSuperSampling_currentIndexChanged(const QString &arg1)
{
    if (arg1.contains("x128"))
        superSampling = 128;
    else if (arg1.contains("x32"))
        superSampling = 32;
    else if (arg1.contains("x8"))
        superSampling = 8;
    else if (arg1.contains("x1"))
        superSampling = 1;
}


// Envelope //

void MainWindow::on_pushButtonEnvelopeAmp_clicked()
{
    setCurrentEnvelope(EnvType::Amp);
}

void MainWindow::on_pushButtonEnvelopeShape_clicked()
{
    setCurrentEnvelope(EnvType::Shape);
}

void MainWindow::on_pushButtonEnvelopePitch_clicked()
{
    setCurrentEnvelope(EnvType::Pitch);
}


void MainWindow::on_horizontalScrollBarD1Gain_valueChanged(int value)
{
    float val = normalizeSliderInput(value, ui->horizontalScrollBarD1Gain->maximum());
    getEnvelopeType(curOp, curEnv).e1.setGain(val);
    refreshWaveformEnvelope(curEnv);
}

void MainWindow::on_horizontalScrollBarD1Time_valueChanged(int value)
{
    float val = normalizeSliderInput(value, ui->horizontalScrollBarD1Time->maximum());
    getEnvelopeType(curOp, curEnv).e1.setDecayTime(val);
    refreshWaveformEnvelope(curEnv);
}

void MainWindow::on_horizontalScrollBarD1Tension_valueChanged(int value)
{
    float val = normalizeSliderInput(value, ui->horizontalScrollBarD1Tension->maximum());
    getEnvelopeType(curOp, curEnv).e1.setDecayTension(val);
    refreshWaveformEnvelope(curEnv);
}

void MainWindow::on_comboBoxD1Type_currentIndexChanged(const QString &arg1)
{
    if (arg1.contains("BSplineSmooth"))
        getEnvelopeType(curOp, curEnv).e1.setType(Envelope::Type::BSplineSmooth);
    else if (arg1.contains("BSpline"))
        getEnvelopeType(curOp, curEnv).e1.setType(Envelope::Type::BSpline);
    else if (arg1.contains("Exponential"))
        getEnvelopeType(curOp, curEnv).e1.setType(Envelope::Type::Exponential);
    else if (arg1.contains("Linear"))
        getEnvelopeType(curOp, curEnv).e1.setType(Envelope::Type::Linear);

    refreshWaveformEnvelope(curEnv);
}


void MainWindow::on_horizontalScrollBarD2Gain_valueChanged(int value)
{
    float val = normalizeSliderInput(value, ui->horizontalScrollBarD2Gain->maximum());
    getEnvelopeType(curOp, curEnv).e2.setGain(val);
    refreshWaveformEnvelope(curEnv);
}

void MainWindow::on_horizontalScrollBarD2Time_valueChanged(int value)
{
    float val = normalizeSliderInput(value, ui->horizontalScrollBarD2Time->maximum());
    getEnvelopeType(curOp, curEnv).e2.setDecayTime(val);
    refreshWaveformEnvelope(curEnv);
}

void MainWindow::on_horizontalScrollBarD2Tension_valueChanged(int value)
{
    float val = normalizeSliderInput(value, ui->horizontalScrollBarD2Tension->maximum());
    getEnvelopeType(curOp, curEnv).e2.setDecayTension(val);
    refreshWaveformEnvelope(curEnv);
}

void MainWindow::on_comboBoxD2Type_currentIndexChanged(const QString &arg1)
{
    if (arg1.contains("BSplineSmooth"))
        getEnvelopeType(curOp, curEnv).e2.setType(Envelope::Type::BSplineSmooth);
    else if (arg1.contains("BSpline"))
        getEnvelopeType(curOp, curEnv).e2.setType(Envelope::Type::BSpline);
    else if (arg1.contains("Exponential"))
        getEnvelopeType(curOp, curEnv).e2.setType(Envelope::Type::Exponential);
    else if (arg1.contains("Linear"))
        getEnvelopeType(curOp, curEnv).e2.setType(Envelope::Type::Linear);

    refreshWaveformEnvelope(curEnv);
}


void MainWindow::on_checkBoxD1Gain_toggled(bool checked)
{
    randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D1Gain] = checked;
}

void MainWindow::on_checkBoxD1Time_toggled(bool checked)
{
    randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D1Time] = checked;
}

void MainWindow::on_checkBoxD1Tension_toggled(bool checked)
{
    randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D1Tension] = checked;
}

void MainWindow::on_checkBoxD1Type_toggled(bool checked)
{
    randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D1Type] = checked;
}

void MainWindow::on_checkBoxD2Gain_toggled(bool checked)
{
    randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D2Gain] = checked;
}

void MainWindow::on_checkBoxD2Time_toggled(bool checked)
{
    randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D2Time] = checked;
}

void MainWindow::on_checkBoxD2Tension_toggled(bool checked)
{
    randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D2Tension] = checked;
}

void MainWindow::on_checkBoxD2Type_toggled(bool checked)
{
    randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D2Type] = checked;
}


// File //

void MainWindow::on_pushButtonSaveIni_clicked()
{
    saveSettings(ui->lineEditFilePrefix->text() + ".ini");
}

void MainWindow::on_pushButtonLoadIni_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load File"), QCoreApplication::applicationDirPath(), tr("INI Files (*.ini)"));
    loadSettings(fileName);
}


// Actions //

void MainWindow::on_actionCopyValue_triggered()
{
    // 一時保存される値は[0.0, 1.0]の範囲に正規化される

    Counter *counter = dynamic_cast<Counter*>(focusWidget());
    if (counter)
    {
        tempValue = counter->value() / counter->maximum();
    }

    QAbstractSlider *scrollBar = dynamic_cast<QAbstractSlider*>(focusWidget());
    if (scrollBar)
    {
        tempValue = (double)scrollBar->value() / scrollBar->maximum();
    }
}

void MainWindow::on_actionPasteValue_triggered()
{
    Counter *counter = dynamic_cast<Counter*>(focusWidget());
    if (counter)
    {
        counter->setValue(tempValue * counter->maximum());
    }

    QAbstractSlider *scrollBar = dynamic_cast<QAbstractSlider*>(focusWidget());
    if (scrollBar)
    {
        scrollBar->setValue(static_cast<int>(tempValue * scrollBar->maximum()));
    }
}

void MainWindow::on_actionCopyOscillator_triggered()
{
    QString name = focusWidget()->objectName();

    if (name.contains("1"))
        tempOp = fmto.op(0);
    else if (name.contains("2"))
        tempOp = fmto.op(1);
    else if (name.contains("3"))
        tempOp = fmto.op(2);
    else if (name.contains("Sub"))
        tempOp = fmto.op(3);
}

void MainWindow::on_actionPasteOscillator_triggered()
{
    QString name = focusWidget()->objectName();

    if (name.contains("1"))
        fmto.op(0) = tempOp;
    else if (name.contains("2"))
        fmto.op(1) = tempOp;
    else if (name.contains("3"))
        fmto.op(2) = tempOp;
    else if (name.contains("Sub"))
        fmto.op(3) = tempOp;

    refreshOscillator();
}

void MainWindow::on_actionCopyEnvelope_triggered()
{
    QString type = focusWidget()->objectName();

    if (type.contains("Amp"))
        tempEnv = getEnvelopeType(curOp, EnvType::Amp);
    else if (type.contains("Shape"))
        tempEnv = getEnvelopeType(curOp, EnvType::Shape);
    else if (type.contains("Pitch"))
        tempEnv = getEnvelopeType(curOp, EnvType::Pitch);
}

void MainWindow::on_actionPasteEnvelope_triggered()
{
    QString type = focusWidget()->objectName();

    if (type.contains("Amp"))
        getEnvelopeType(curOp, EnvType::Amp) = tempEnv;
    else if (type.contains("Shape"))
        getEnvelopeType(curOp, EnvType::Shape) = tempEnv;
    else if (type.contains("Pitch"))
        getEnvelopeType(curOp, EnvType::Pitch) = tempEnv;

    refreshEnvelope(curEnv);
}




//
// protected events
//

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    int size = ui->waveformEnvelope->width() < 512 ? 512 : ui->waveformEnvelope->width();
    waveEnvelope.resize(size);
    refreshWaveformEnvelope(curEnv);
    ui->waveformEnvelope->refresh();
}


//
// private
//

void MainWindow::saveSettings(QString fileName)
{
    // iniファイルを実行ファイルと同じディレクトリに保存
    QSettings settings(fileName, QSettings::IniFormat);

    // randomize settings
    settings.setValue("Pitch/Random", ui->checkBoxPitch->checkState());
    settings.setValue("OscType/Random", ui->checkBoxOscType->checkState());
    settings.setValue("Phase/Random", ui->checkBoxPhase->checkState());
    settings.setValue("Modulation/Random", ui->checkBoxModulation->checkState());

    settings.setValue("Delay/Time/Random", ui->checkBoxDelayTime->checkState());
    settings.setValue("Delay/Decay/Random", ui->checkBoxDelayDecay->checkState());
    settings.setValue("Clip/Gain/Random", ui->checkBoxClipGain->checkState());
    settings.setValue("Clip/Decay/Random", ui->checkBoxClipDecay->checkState());

    settings.setValue("Duration/Random", ui->checkBoxDuration->checkState());
    settings.setValue("SampleRate/Random", ui->checkBoxSampleRate->checkState());
    settings.setValue("SuperSampling/Random", ui->checkBoxSuperSampling->checkState());

    for (int env = (int)EnvType::Begin; env < (int)EnvType::End; ++env)
    {
        settings.setValue("D1Gain/Random",    randomizeSettingsEnvelope[env][(int)EnvParams::D1Gain]);
        settings.setValue("D1Time/Random",    randomizeSettingsEnvelope[env][(int)EnvParams::D1Time]);
        settings.setValue("D1Tension/Random", randomizeSettingsEnvelope[env][(int)EnvParams::D1Tension]);
        settings.setValue("D1Type/Random",    randomizeSettingsEnvelope[env][(int)EnvParams::D1Type]);
        settings.setValue("D2Gain/Random",    randomizeSettingsEnvelope[env][(int)EnvParams::D2Gain]);
        settings.setValue("D2Time/Random",    randomizeSettingsEnvelope[env][(int)EnvParams::D2Time]);
        settings.setValue("D2Tension/Random", randomizeSettingsEnvelope[env][(int)EnvParams::D2Tension]);
        settings.setValue("D2Type/Random",    randomizeSettingsEnvelope[env][(int)EnvParams::D2Type]);
    }

    // FX
    settings.setValue("Delay/Time", ui->horizontalScrollBarDelayTime->value());
    settings.setValue("Delay/Decay", ui->horizontalScrollBarDelayDecay->value());
    settings.setValue("Clip/Gain", ui->horizontalScrollBarClipGain->value());
    settings.setValue("Clip/Decay", ui->horizontalScrollBarClipDecay->value());

    // misc
    settings.setValue("Duration", static_cast<double>(fmto.duration()));
    settings.setValue("SampleRate", static_cast<int>(SampleRate::get()));
    settings.setValue("SuperSampling", ui->comboBoxSuperSampling->currentIndex());
    settings.setValue("DcBlock", ui->checkBoxDcBlock->isChecked());
    settings.setValue("Declick", ui->checkBoxDeclick->isChecked());

    // sub osc
    // 何もできていない

    // file
    settings.setValue("WaveFilePrefix", ui->lineEditFilePrefix->text());

    // oscillator, envelope
    for (int op = 0; op < fmto.maxOp(); ++op)
    {
        QString opPrefix = QString("Op%1/").arg(op);

        settings.setValue(opPrefix + "Pitch",      static_cast<double>(fmto.op(op).getPitch()));
        settings.setValue(opPrefix + "OscType",    static_cast<int>(fmto.op(op).osc.getType()));
        settings.setValue(opPrefix + "Phase",      static_cast<double>(fmto.op(op).osc.getPhaseOffset()));
        settings.setValue(opPrefix + "Modulation", static_cast<double>(fmto.op(op).getModIndex()));
        settings.setValue(opPrefix + "Mute", fmto.op(op).mute);

        for (int env = (int)EnvType::Begin; env < (int)EnvType::End; ++env)
        {
            QString envPrefix = opPrefix + QString("Envelope%1/").arg(env);
            DecayEnvelope denv = getEnvelopeType(op, (EnvType)env);

            settings.setValue(envPrefix + "D1Gain",    static_cast<double>(denv.e1.getGain()        ));
            settings.setValue(envPrefix + "D1Time",    static_cast<double>(denv.e1.getDecayTime()   ));
            settings.setValue(envPrefix + "D1Tension", static_cast<double>(denv.e1.getDecayTension()));
            settings.setValue(envPrefix + "D1Type",    static_cast<int>(denv.e1.getType()));
            settings.setValue(envPrefix + "D2Gain",    static_cast<double>(denv.e2.getGain()        ));
            settings.setValue(envPrefix + "D2Time",    static_cast<double>(denv.e2.getDecayTime()   ));
            settings.setValue(envPrefix + "D2Tension", static_cast<double>(denv.e2.getDecayTension()));
            settings.setValue(envPrefix + "D2Type",    static_cast<int>(denv.e2.getType()));
        }
    }
}

void MainWindow::loadSettings(QString fileName)
{
    if (!QFile::exists(fileName))
        return;

    QSettings settings(fileName, QSettings::IniFormat);

    // randomize settings
    ui->checkBoxPitch->setChecked(settings.value("Pitch/Random").toBool());
    ui->checkBoxOscType->setChecked(settings.value("OscType/Random").toBool());
    ui->checkBoxPhase->setChecked(settings.value("Phase/Random").toBool());
    ui->checkBoxModulation->setChecked(settings.value("Modulation/Random").toBool());

    ui->checkBoxDelayTime->setChecked(settings.value("Delay/Time/Random").toBool());
    ui->checkBoxDelayDecay->setChecked(settings.value("Delay/Decay/Random").toBool());
    ui->checkBoxClipGain->setChecked(settings.value("Clip/Gain/Random").toBool());
    ui->checkBoxClipDecay->setChecked(settings.value("Clip/Decay/Random").toBool());

    ui->checkBoxDuration->setChecked(settings.value("Duration/Random").toBool());
    ui->checkBoxSampleRate->setChecked(settings.value("SampleRate/Random").toBool());
    ui->checkBoxSuperSampling->setChecked(settings.value("SuperSampling/Random").toBool());

    for (int env = (int)EnvType::Begin; env < (int)EnvType::End; ++env)
    {
        randomizeSettingsEnvelope[env][(int)EnvParams::D1Gain]    = (settings.value("D1Gain/Random").toBool());
        randomizeSettingsEnvelope[env][(int)EnvParams::D1Time]    = (settings.value("D1Time/Random").toBool());
        randomizeSettingsEnvelope[env][(int)EnvParams::D1Tension] = (settings.value("D1Tension/Random").toBool());
        randomizeSettingsEnvelope[env][(int)EnvParams::D1Type]    = (settings.value("D1Type/Random").toBool());
        randomizeSettingsEnvelope[env][(int)EnvParams::D2Gain]    = (settings.value("D2Gain/Random").toBool());
        randomizeSettingsEnvelope[env][(int)EnvParams::D2Time]    = (settings.value("D2Time/Random").toBool());
        randomizeSettingsEnvelope[env][(int)EnvParams::D2Tension] = (settings.value("D2Tension/Random").toBool());
        randomizeSettingsEnvelope[env][(int)EnvParams::D2Type]    = (settings.value("D2Type/Random").toBool());
    }

    // FX
    ui->horizontalScrollBarDelayTime->setValue(settings.value("Delay/Time").toInt());
    ui->horizontalScrollBarDelayDecay->setValue(settings.value("Delay/Decay").toInt());
    ui->horizontalScrollBarClipGain->setValue(settings.value("Clip/Gain").toInt());
    ui->horizontalScrollBarClipDecay->setValue(settings.value("Clip/Decay").toInt());

    // misc
    fmto.setDuration(settings.value("Duration").toDouble());
    SampleRate::set(settings.value("SampleRate").toInt());
    ui->comboBoxSuperSampling->setCurrentIndex(settings.value("SuperSampling").toInt());
    ui->checkBoxDcBlock->setChecked(settings.value("DcBlock").toBool());
    ui->checkBoxDeclick->setChecked(settings.value("Declick").toBool());

    // sub osc
    // 何もできていない

    // file
    ui->lineEditFilePrefix->setText(settings.value("WaveFilePrefix").toString());

    // oscillator, envelope
    for (int op = 0; op < fmto.maxOp(); ++op)
    {
        QString opPrefix = QString("Op%1/").arg(op);

        fmto.op(op).setPitch(settings.value(opPrefix + "Pitch").toDouble());
        fmto.op(op).osc.setType(static_cast<Oscillator::OscillatorType>(settings.value(opPrefix + "OscType").toInt()));
        fmto.op(op).osc.setPhaseOffset(settings.value(opPrefix + "Phase").toDouble());
        fmto.op(op).setModIndex(settings.value(opPrefix + "Modulation").toDouble());
        fmto.op(op).mute = (settings.value(opPrefix + "Mute").toBool());

        for (int env = (int)EnvType::Begin; env < (int)EnvType::End; ++env)
        {
            QString envPrefix = opPrefix + QString("Envelope%1/").arg(env);

            getEnvelopeType(op, (EnvType)env).e1.setGain(   settings.value(envPrefix + "D1Gain"   ).toDouble());
            getEnvelopeType(op, (EnvType)env).e1.setDecayTime(   settings.value(envPrefix + "D1Time"   ).toDouble());
            getEnvelopeType(op, (EnvType)env).e1.setDecayTension(settings.value(envPrefix + "D1Tension").toDouble());
            getEnvelopeType(op, (EnvType)env).e1.setType(static_cast<Envelope::Type>(settings.value(envPrefix + "D1Type"   ).toInt()));
            getEnvelopeType(op, (EnvType)env).e2.setGain(   settings.value(envPrefix + "D2Gain"   ).toDouble());
            getEnvelopeType(op, (EnvType)env).e2.setDecayTime(   settings.value(envPrefix + "D2Time"   ).toDouble());
            getEnvelopeType(op, (EnvType)env).e2.setDecayTension(settings.value(envPrefix + "D2Tension").toDouble());
            getEnvelopeType(op, (EnvType)env).e2.setType(static_cast<Envelope::Type>(settings.value(envPrefix + "D2Type"   ).toInt()));
        }
    }

    refresh();
}


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

    float tt = 1.0f / (superSampling * SampleRate::get());
    for (int i = 0; i < numSamples; ++i)
    {
        float temp = 0.0f;
        for (int ss = 0; ss < superSampling; ++ss)
        {
            temp += fmto.render((i * superSampling + ss) * (tt), tt);
        }
        waveSound[i] = temp / superSampling;
        //waveSound[i] = sin(2.0f * PI * 100.0f * i / SampleRate::get());
    }

    if (0 < ui->horizontalScrollBarDelayDecay->value())
        delay(waveSound);

    if (0 < ui->horizontalScrollBarClipGain->value())
        clip(waveSound);

    if (ui->checkBoxDcBlock->isChecked())
        dcKill();

    if (ui->checkBoxDeclick->isChecked())
        declick(minSamples);
}

void MainWindow::clip(QVector<float> &wav)
{
    float gain = 1.0f + normalizeSliderInput(ui->horizontalScrollBarClipGain->value(), ui->horizontalScrollBarClipGain->maximum());
    float decayVal = std::max(1e-20f, normalizeSliderInput(ui->horizontalScrollBarClipDecay->value(), ui->horizontalScrollBarClipDecay->maximum()));

    // Waveshaper :: Gloubi-boulga
    for (int i = 0; i < wav.size(); ++i)
    {
        float decay = std::min((float)i / (wav.size() * decayVal), 1.0f);
        float x = (2.0f - decay * decay) * gain * wav[i] * 0.686306;
        float a = 1.0 + exp(sqrt(fabs(x)) * -0.75);
        wav[i] = 0.5f * ((exp(x) - exp(-x * a)) / (exp(x) + exp(-x)));
    }
}

void MainWindow::delay(QVector<float> &wav)
{
    // 時間の単位はsec, 最小0.1ms, 最大50.1ms
    float time = 0.0001f + 0.050f * normalizeSliderInput(ui->horizontalScrollBarDelayTime->value(), ui->horizontalScrollBarDelayTime->maximum());

    // ここでの decay は feedback と同義
    float decay = normalizeSliderInput(ui->horizontalScrollBarDelayDecay->value(), ui->horizontalScrollBarDelayDecay->maximum());
    float attenuation = 0.9f * decay;

    // 長さ分のバッファを用意
    QVector<float> buf;
    buf.resize(time * SampleRate::get() * superSampling);

    int bufIndex = 0;
    float amp = (1.0f - 0.6f * decay);
    for (int i = 0; i < wav.size(); ++i)
    {
        float temp = 0.0f;
        for (int j = 0; j < superSampling; ++j)
        {
            buf.push_back(wav[i] + attenuation * buf[0]);
            temp += (wav[i] + buf[0]) * amp;

            buf.pop_front();
            bufIndex = (bufIndex + 1) % buf.size();
        }
        wav[i] = temp / superSampling;
    }
}

void MainWindow::dcKill()
{
    dcBlocker.clearBuffer();

    for (int sample = 0; sample < waveSound.size(); ++sample)
    {
        waveSound[sample] = dcBlocker.pass(waveSound[sample]);
    }
}

void MainWindow::declick(int declickLength)
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

    int size = ui->waveformEnvelope->width() < 512 ? 512 : ui->waveformEnvelope->width();
    waveEnvelope.resize(size);
    ui->waveformEnvelope->setWave(&waveEnvelope);
}

void MainWindow::setupContextMenu()
{
    // context menu slider
    QList<QAction*> actionListSlider;
    actionListSlider.append(ui->actionCopyValue);
    actionListSlider.append(ui->actionPasteValue);

    ui->counterPitch->addActions(actionListSlider);
    ui->horizontalScrollBarPhase->addActions(actionListSlider);
    ui->horizontalScrollBarMod->addActions(actionListSlider);

    ui->horizontalScrollBarDelayTime->addActions(actionListSlider);
    ui->horizontalScrollBarDelayDecay->addActions(actionListSlider);
    ui->horizontalScrollBarClipGain->addActions(actionListSlider);
    ui->horizontalScrollBarClipDecay->addActions(actionListSlider);

    ui->counterDuration->addActions(actionListSlider);

    ui->horizontalScrollBarD1Gain->addActions(actionListSlider);
    ui->horizontalScrollBarD1Time->addActions(actionListSlider);
    ui->horizontalScrollBarD1Tension->addActions(actionListSlider);
    ui->horizontalScrollBarD2Gain->addActions(actionListSlider);
    ui->horizontalScrollBarD2Time->addActions(actionListSlider);
    ui->horizontalScrollBarD2Tension->addActions(actionListSlider);

    // context menu envelope
    QList<QAction*> actionListEnvelope;
    actionListEnvelope.append(ui->actionCopyEnvelope);
    actionListEnvelope.append(ui->actionPasteEnvelope);

    ui->pushButtonEnvelopeAmp->addActions(actionListEnvelope);
    ui->pushButtonEnvelopeShape->addActions(actionListEnvelope);
    ui->pushButtonEnvelopePitch->addActions(actionListEnvelope);

    // context menu operator
    QList<QAction*> actionListOscillator;
    actionListOscillator.append(ui->actionCopyOscillator);
    actionListOscillator.append(ui->actionPasteOscillator);

    ui->pushButtonOscillator1->addActions(actionListOscillator);
    ui->pushButtonOscillator2->addActions(actionListOscillator);
    ui->pushButtonOscillator3->addActions(actionListOscillator);
    ui->pushButtonOscillatorSub->addActions(actionListOscillator);
}

int MainWindow::getNumberOfSamples()
{
    // counterDuration->valueの単位は秒
    return static_cast<int>(fmto.duration() * SampleRate::get());
}


float MainWindow::normalizeSliderInput(int value, int maximum)
{
    // QAbstructSliderを入力とするべきだった
    // ただしcounterを書き直す必要がある
    return static_cast<float>(value) / static_cast<float>(maximum);
}

int MainWindow::normalizeSliderValue(float value, int maximum)
{
    return static_cast<int>(value * static_cast<float>(maximum));
}


void MainWindow::refresh()
{
    ui->counterDuration->setValue(fmto.duration());
    ui->spinBoxSampleRate->setValue(SampleRate::get());

    refreshOscillator();
}


void MainWindow::setCurrentOperator(int op)
{
    curOp = op;
    refreshOscillator();
}

void MainWindow::refreshOscillator()
{
    ui->counterPitch->setValue(fmto.op(curOp).getPitch());
    ui->comboBoxOscType->setCurrentIndex(static_cast<int>(fmto.op(curOp).osc.getType()));
    ui->horizontalScrollBarPhase->setValue(normalizeSliderValue(fmto.op(curOp).osc.getPhaseOffset(), ui->horizontalScrollBarPhase->maximum()));
    ui->horizontalScrollBarMod->setValue(normalizeSliderValue(fmto.op(curOp).getModIndex(), ui->horizontalScrollBarMod->maximum()));
    ui->checkBoxMute->setChecked(fmto.op(curOp).mute);

    refreshEnvelope(curEnv);
}


void MainWindow::setCurrentEnvelope(EnvType type)
{
    curEnv = type;
    refreshEnvelope(curEnv);
}

DecayEnvelope& MainWindow::getEnvelopeType(int op, EnvType type)
{
    switch(type)
    {
    case EnvType::Amp:
        return fmto.op(op).envAmp;

    case EnvType::Pitch:
        return fmto.op(op).envPitch;

    case EnvType::Shape:
        return fmto.op(op).envShape;

    default:
        Q_ASSERT(0);
        return fmto.op(curOp).envAmp;
    }
}

void MainWindow::refreshEnvelope(EnvType type)
{
    DecayEnvelope env = getEnvelopeType(curOp, type);

    ui->horizontalScrollBarD1Gain->setValue(normalizeSliderValue(env.e1.getGain(), ui->horizontalScrollBarD1Gain->maximum()));
    ui->horizontalScrollBarD1Time->setValue(normalizeSliderValue(env.e1.getDecayTime(), ui->horizontalScrollBarD1Time->maximum()));
    ui->horizontalScrollBarD1Tension->setValue(normalizeSliderValue(env.e1.getDecayTension(), ui->horizontalScrollBarD1Tension->maximum()));
    ui->comboBoxD1Type->setCurrentIndex(static_cast<int>(env.e1.getType()));
    ui->horizontalScrollBarD2Gain->setValue(normalizeSliderValue(env.e2.getGain(), ui->horizontalScrollBarD2Gain->maximum()));
    ui->horizontalScrollBarD2Time->setValue(normalizeSliderValue(env.e2.getDecayTime(), ui->horizontalScrollBarD2Time->maximum()));
    ui->horizontalScrollBarD2Tension->setValue(normalizeSliderValue(env.e2.getDecayTension(), ui->horizontalScrollBarD2Tension->maximum()));
    ui->comboBoxD2Type->setCurrentIndex(static_cast<int>(env.e2.getType()));

    ui->checkBoxD1Gain->setChecked(   randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D1Gain]);
    ui->checkBoxD1Time->setChecked(   randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D1Time]);
    ui->checkBoxD1Tension->setChecked(randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D1Tension]);
    ui->checkBoxD1Type->setChecked(   randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D1Type]);
    ui->checkBoxD2Gain->setChecked(   randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D2Gain]);
    ui->checkBoxD2Time->setChecked(   randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D2Time]);
    ui->checkBoxD2Tension->setChecked(randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D2Tension]);
    ui->checkBoxD2Type->setChecked(   randomizeSettingsEnvelope[(int)curEnv][(int)EnvParams::D2Type]);

    refreshWaveformEnvelope(type);
}

void MainWindow::refreshWaveformEnvelope(EnvType type)
{
    DecayEnvelope env = getEnvelopeType(curOp, type);

    for (int i = 0; i < waveEnvelope.size(); ++i)
    {
        waveEnvelope[i] = - env.at((float)i / (float)waveEnvelope.size());
    }

    ui->waveformEnvelope->refresh();
}
