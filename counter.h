#ifndef COUNTER_H
#define COUNTER_H

#include <QWidget>

class Counter : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(double Value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double Minimum READ minimum WRITE setMinimum NOTIFY rangeChanged)
    Q_PROPERTY(double Maximum READ maximum WRITE setMaximum NOTIFY rangeChanged)
    Q_PROPERTY(int lowerDigits READ lowerDigits WRITE setLowerDigits NOTIFY lowerDigitsChanged)
    Q_PROPERTY(int upperDigits READ upperDigits WRITE setUpperDigits NOTIFY upperDigitsChanged)

public:
   Counter(QWidget *parent = 0);

   double value();
   double minimum();
   double maximum();
   int lowerDigits();
   int upperDigits();

   QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

private:
    double _valueMin;
    double _valueMax;
    int _lowerDigits;
    int _upperDigits;

    bool changingValue;
    double _value;

    double _lastValue;
    int _mouseDeltaY;
    QPoint _lastPoint;
    double _digitScale;

    QString valueString();


signals:
    void counterPressed();
    void counterReleased();
    void valueChanged(double value);
    void rangeChanged(double min, double max);
    void lowerDigitsChanged();
    void upperDigitsChanged();

public slots:
    void setValue(double val);
    void setMinimum(double min);
    void setMaximum(double max);
    void setLowerDigits(int digits);
    void setUpperDigits(int digits);
};

#endif // COUNTER_H
