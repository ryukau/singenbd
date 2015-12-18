#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QWidget>
#include <QColor>
#include <QPoint>

class Waveform : public QWidget
{
    Q_OBJECT

    // 色はスタイルシートで変更するようにできないか
    Q_PROPERTY(QColor colorWave READ colorWave WRITE setColorWave)
    Q_PROPERTY(QColor colorAxis READ colorAxis WRITE setColorAxis)
    Q_PROPERTY(bool displaySampleNumber READ displaySampleNumber WRITE setDisplaySampleNumber)
    Q_PROPERTY(bool zoom READ zoom WRITE setZoom)
    Q_PROPERTY(bool upperHalf READ upperHalf WRITE setUpperHalf)

public:
    explicit Waveform(QWidget *parent = 0);
    explicit Waveform(QVector<float> * wave, QWidget *parent = 0);
    ~Waveform();

    QColor colorWave();
    QColor colorAxis();
    bool displaySampleNumber();
    bool zoom();
    bool upperHalf();
    void setColorWave(QColor color);
    void setColorAxis(QColor color);
    void setDisplaySampleNumber(bool display);
    void setZoom(bool z);
    void setUpperHalf(bool uh);
    void setWave(QVector<float> * wave);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent * event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

private:
    bool draggingWaveform;
    QPoint _lastPoint;

    QColor _colorWave;
    QColor _colorAxis;

    QVector<float> dummyData;
    QVector<float> * data;
    QVector<float> dataDecimated;
    QVector<float> dataMin;
    QVector<float> dataMax;
    QVector<bool> dataPoint;

    bool _displaySampleNumber;
    bool _zoom;
    bool _upperHalf;

    int inputSize;
    int inputOffset;

    void resample();
    void resizeDisplayDataWidth();

    float sinc(float input);
    void windowFunc(QVector<float> *output);

signals:
    void rangeChanged();

public slots:
    void refresh();
};

#endif // WAVEFORM_H
