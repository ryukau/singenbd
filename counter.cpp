#pragma once

#include "counter.h"
#include <QtWidgets>
#include <cmath>



const double y_move_sensi = 1.0 / 8.0; // 数値を1変更するために必要な移動量[pixel] の逆数[1.0/pixel]



//
// constructor
//

Counter::Counter(QWidget *parent) :
    QWidget(parent),
    _valueMin(0.0),
    _valueMax(1000.0),
    _lowerDigits(4),
    _upperDigits(4),
    changingValue(false),
    _value(0.0),
    _lastValue(0.0),
    _digitScale(0.0)
{
}


QString Counter::valueString()
{
    return QString("%1").arg(_value, _upperDigits + 1 /* 小数点 */ + _lowerDigits, 'f', _lowerDigits, '0'); // 数字の桁数指定
}



//
// getter
//

double Counter::value()
{
    return _value;
}


double Counter::minimum()
{
    return _valueMin;
}


double Counter::maximum()
{
    return _valueMax;
}


int Counter::lowerDigits()
{
    return _lowerDigits;
}


int Counter::upperDigits()
{
    return _upperDigits;
}



//
// setter
//

void Counter::setValue(double val)
{
    if (val < _valueMin)
    {
        _value = _valueMin;
    }
    else if (val > _valueMax)
    {
        _value = _valueMax;
    }
    else
    {
        _value = val;
    }

    update();
    emit valueChanged(_value);
}


void Counter::setMinimum(double min)
{
    _valueMin = min;

    emit rangeChanged(_valueMin, _valueMax);
}


void Counter::setMaximum(double max)
{
    _valueMax = max;

    emit rangeChanged(_valueMin, _valueMax);
}


void Counter::setLowerDigits(int digits)
{
    _lowerDigits = digits;
}


void Counter::setUpperDigits(int digits)
{
    _upperDigits = digits;
    //setMaximum(pow(10.0, digits - 1));
}


QSize Counter::sizeHint() const
{
    QFontMetrics fontMetrics(font());
    QString str = QString("%1").arg(_value, _upperDigits + 1 /* 小数点 */ + _lowerDigits, 'f', _lowerDigits, '0');
    return QSize(fontMetrics.width(str), fontMetrics.height());
}



//
// event
//

void Counter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // スタイルシートを描画
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    // 数値を描画
    painter.setFont(font());
    painter.setPen(QColor::QColor(0, 0, 0));

    QFontMetrics fontMetrics(font());
    painter.drawText(0, fontMetrics.height(), valueString());
}


void Counter::mouseMoveEvent(QMouseEvent *event)
{
    if (changingValue && (event->buttons() & Qt::LeftButton))
    {
        _mouseDeltaY += _lastPoint.y() - QCursor::pos().y();
        setValue(_lastValue + floor((double)(_mouseDeltaY) * y_move_sensi) * _digitScale);

        QCursor::setPos(_lastPoint);
        repaint();
    }

    if (_value != _lastValue)
    {
        emit valueChanged(_value);
    }
}


void Counter::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        changingValue = true;
        _lastValue = _value;
        _lastPoint = QCursor::pos();
        _mouseDeltaY = 0;


        // digitScaleを計算
        int index = _upperDigits - 1;
        QFontMetrics fontMetrics(font());
        QString str = valueString();
        for (int i = 1; i < str.size(); ++i)
        {
            if (event->pos().x() <= fontMetrics.width(str.left(i)))
                break;

            if (i == _upperDigits)
                ++i;

            --index;
        }

        _digitScale = pow(10.0, index); // 横移動で調整する桁を変更

        setCursor(Qt::BlankCursor);

        emit counterPressed();
    }
}


void Counter::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        changingValue = false;
        setCursor(Qt::ArrowCursor);

        emit counterReleased();
    }
}

