#include "waveform.h"


#include <QDebug>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <cmath>


const double eps = 1e-52;
const double PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;

const double xScaleCoff = 2.0;
const int windowLength = 256;


//
// constructor, destructor
//

Waveform::Waveform(QWidget *parent) :
    QWidget(parent),
    dummyData(1),
    data(&dummyData),
    dataDecimated(1),
    dataMin(1),
    dataMax(1),
    inputSize(data->size()),
    inputOffset(0),
    _colorWave(80, 80, 80),
    _colorAxis(180, 180, 220),
    _displaySampleNumber(true),
    _zoom(true),
    _upperHalf(false),
    draggingWaveform(false)
{
    setStyleSheet("background-color:white;");
}


Waveform::Waveform(QVector<float> * wave, QWidget *parent) :
    QWidget(parent),
    dummyData(1),
    dataDecimated(1),
    dataMin(1),
    dataMax(1),
    inputSize(data->size()),
    inputOffset(0),
    _colorWave(80, 80, 80),
    _colorAxis(180, 180, 220),
    _displaySampleNumber(true),
    _zoom(true),
    _upperHalf(false),
    draggingWaveform(false)
{
    setStyleSheet("background-color:white;");

    setWave(wave);
}


Waveform::~Waveform()
{
    data = nullptr;
}

QColor Waveform::colorWave()
{
    return _colorWave;
}

QColor Waveform::colorAxis()
{
    return _colorAxis;
}

bool Waveform::displaySampleNumber()
{
    return _displaySampleNumber;
}

bool Waveform::zoom()
{
    return _zoom;
}

bool Waveform::upperHalf()
{
    return _upperHalf;
}

void Waveform::setColorWave(QColor color)
{
    _colorWave = color;
}

void Waveform::setColorAxis(QColor color)
{
    _colorAxis = color;
}

void Waveform::setDisplaySampleNumber(bool display)
{
    _displaySampleNumber = display;
}

void Waveform::setZoom(bool z)
{
    _zoom = z;
}

void Waveform::setUpperHalf(bool uh)
{
    _upperHalf = uh;
}


void Waveform::setWave(QVector<float> *wave)
{
    data = wave;

    if (data->size() <= 0)
        data->append(dummyData);

    refresh();
}


QSize Waveform::sizeHint() const
{
    return QSize(400, 200);
}


//
// events
//

void Waveform::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen wavePen(_colorWave);
    wavePen.setWidthF(1.0);
    QPen axisPen(_colorAxis);
    axisPen.setWidthF(2.0);

    // 上半分だけの描画かどうかを設定
    int _height = height();
    if (!_upperHalf)
        _height /= 2;

    // スタイルシートを描画
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    // 軸を描画
    painter.setPen(axisPen);
    painter.drawLine(0, _height, width(), _height);

    // 波形を描画
    painter.setPen(wavePen);
    painter.translate(0, _height); // 整列: x軸左揃え、y軸中央揃え

    for (int i = 1; i < width(); ++i)
    {
        painter.drawLine(i - 1, dataDecimated.at(i - 1) * _height, i, dataDecimated.at(i) * _height);
        //painter.drawLine(i - 1, dataMin.at(i - 1) * heightPerTwo, i, dataMin.at(i) * heightPerTwo);
        //painter.drawLine(i - 1, dataMax.at(i - 1) * heightPerTwo, i, dataMax.at(i) * heightPerTwo);
        painter.drawLine(i, dataMin.at(i) * _height, i, dataMax.at(i) * _height);

        if (dataPoint.at(i))
            painter.drawArc(i - 4, dataDecimated.at(i) * _height - 4, 8, 8, 0, 5760);
    }

    // テキストを表示
    if (_displaySampleNumber)
    {
        QFont font;
        painter.setFont(font);
        painter.setPen(QColor::QColor(0, 0, 0));

        QString str;
        str.append(QString::number(inputOffset)).append("/").append(QString::number(data->size()));
        painter.drawText(0, height()/2, str);
    }
}


void Waveform::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    resizeDisplayDataWidth();
    resample();
}


void Waveform::wheelEvent(QWheelEvent *event)
{
    if (!_zoom)
        return;

    QPoint p = this->mapFromGlobal(QCursor::pos()); // マウスの位置を取得
    double xOffset = qMax((double)p.x() / width(), 0.0);

    if (event->angleDelta().y() < 0)
    {
        int temp = inputSize;
        inputSize *= xScaleCoff;
        inputOffset += xOffset * (temp  - inputSize);
    }
    else
    {
        inputSize /= xScaleCoff;
        inputOffset += xOffset * inputSize;
    }

    // inputSize の値域
    if (data->size() <= inputSize)
        inputSize = data->size() - 1;
    else if(inputSize <= 0)
        inputSize = 1;

    // inputOffset の値域
    if (inputOffset < 0)
        inputOffset = 0;
    else
        inputOffset += qMin(data->size() - (inputSize + inputOffset), 0);

    resample();
    update();

    emit rangeChanged();
}


void Waveform::mouseMoveEvent(QMouseEvent *event)
{
    if (draggingWaveform && (event->buttons() & Qt::LeftButton))
    {
        int dx = _lastPoint.x() - QCursor::pos().x();
        inputOffset += dx * inputSize / width();
        inputOffset = qMax(0, qMin(inputOffset, data->size() - inputSize - 1));
        resample();
        update();
        _lastPoint = QCursor::pos();
    }
}


void Waveform::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        draggingWaveform = true;
        _lastPoint = QCursor::pos();
    }
}


void Waveform::mouseReleaseEvent(QMouseEvent *event)
{
    if (draggingWaveform && event->button() == Qt::LeftButton)
    {
        draggingWaveform = false;
    }
}


//
// private functions
//


void Waveform::resample()
{
    float t = 0.0;

    if (width() <= inputSize)
    {
        float dt = (float)inputSize / width();
        float min, max;
        for (int i = 0; i < width(); ++i)
        {
            t = i * dt + inputOffset;
            if (data->size() <= t) t = data->size() - 1;

            min = 1.0;
            max = -1.0;
            for (int j = t; j < t + dt; ++j)
            {
                if (data->size() <= j) break;
                min = qMin(min, data->at(j));
                max = qMax(max, data->at(j));
            }

            dataDecimated.replace(i, data->at(floor(t)));
            dataMin.replace(i, min);
            dataMax.replace(i, max);
            dataPoint.replace(i, false);
        }
    }
    else
    {
        int tt = inputOffset; // サンプル点用
        QVector<float> win(windowLength, 0.0);
        windowFunc(&win);

        data->insert(0, win.size() / 2, 0.0);
        for (int i = 0; i < width(); ++i)
        {
            t = (double)i * inputSize / width() + inputOffset;

            double y = 0.0;
            for (int n = 0; n < win.size(); ++n)
            {
                if (data->size() <= t + n) break;
                y += data->at(t + n) * sinc((t - floor(t)) - (n - win.size() / 2)) * win.at(n);
            }
            dataDecimated.replace(i, y);
            dataMin.replace(i, y);
            dataMax.replace(i, y);

            if (1.0 <= (t - tt))
            {
                dataPoint.replace(i, true);
                tt = floor(t);
            }
            else
            {
                dataPoint.replace(i, false);
            }
        }
        data->remove(0, win.size() / 2);
    }
}


void Waveform::resizeDisplayDataWidth()
{
    dataDecimated.resize(width());
    dataMin.resize(width());
    dataMax.resize(width());
    dataPoint.resize(width());
}


float Waveform::sinc(float input)
{
    double a = PI * input;
    if (fabs(a) < eps)
    {
        return 1;
    }
    return sin(a) / a;
}


void Waveform::windowFunc(QVector<float> * output)
{
    double omega = 0.0;
    //double m = (output->size() - 1) / 2.0; // Gaussian
    //double sigma = 0.2; // Gaussian
    for (int n = 0; n < output->size(); ++n)
    {
        omega = 2.0 * PI * n / (output->size() - 1);

        // Hanning
        //output->replace(n, 0.5 * (1.0 - cos(omega)));

        // Hamming
        //output->replace(n, 0.54 - 0.46 * cos(omega));

        // Blackman-Nuttall
        //output->replace(n, 0.3635819 - 0.4891775*cos(omega) + 0.1365995*cos(2.0*omega) - 0.0106411*cos(3.0*omega));

        // Cosine
        output->replace(n, sinf(omega / 2.0));

        // Gaussian
        //double nm = n-m;
        //double s = (sigma * output->size());
        //output->replace(n, exp(-nm*nm / (2.0 * s * s)));
    }
}


//
// public slots
//

void Waveform::refresh()
{
    inputSize = data->size();
    inputOffset = 0;
    resizeDisplayDataWidth();
    resample();
    this->update();
}
