#include "testwindow.hpp"
#include <QPainter>
#include <QDebug>
#include <QKeyEvent>
#include <QScreen>
#include <cmath>

double deg2rad (double degrees) {
    return degrees * 4.0 * atan(1.0) / 180.0;
}

TestWindow::TestWindow(QScreen *targetScreen, QWindow *parent) : QRasterWindow(parent)
{
    if (targetScreen) {
        setScreen(targetScreen);
        _refreshRate = targetScreen->refreshRate();
    }

    connect(&_timerNextTest, &QTimer::timeout, this, &TestWindow::onTestEnd);

    _tests.append(new SolidColorTest(
                      this,
                      tr("Solid Black"),
                      QColor(0,   0,   0)));
    _tests.append(new SolidColorTest(
                      this,
                      tr("Solid Red"),
                      QColor(255, 0,   0)));
    _tests.append(new SolidColorTest(
                      this,
                      tr("Solid Green"),
                      QColor(0,   255, 0)));
    _tests.append(new SolidColorTest(
                      this,
                      tr("Solid Blue"),
                      QColor(0,   0, 255)));
    _tests.append(new SolidColorTest(
                      this,
                      tr("Solid Yellow"),
                      QColor(255, 255, 0)));
    _tests.append(new SolidColorTest(
                      this,
                      tr("Solid Magenta"),
                      QColor(255, 0,   255)));
    _tests.append(new SolidColorTest(
                      this,
                      tr("Solid Cyan"),
                      QColor(0,   255, 255)));
    _tests.append(new SolidColorTest(
                      this,
                      tr("Solid White"),
                      QColor(255, 255, 255)));
    _tests.append(new SolidColorTest(
                      this,
                      tr("Solid Gray"),
                      QColor(128, 128, 128)));

    _tests.append(new BlinkTest(
                      this,
                      tr("Blink All"),{
                          QColor(0,   0,   0),
                          QColor(255, 0,   0),
                          QColor(0,   255, 0),
                          QColor(0,   0, 255),
                          QColor(255, 255, 0),
                          QColor(255, 0,   255),
                          QColor(0,   255, 255),
                          QColor(255, 255, 255),
                          QColor(128, 128, 128)
                      }));

    _tests.append(new BlinkTest(
                      this,
                      tr("Blink RGB"),{
                          QColor(255, 0,   0),
                          QColor(0,   255, 0),
                          QColor(0,   0, 255),
                      }));
    _tests.append(new BlinkTest(
                      this,
                      tr("Blink Black/White"),{
                          QColor(255, 255, 255),
                          QColor(0,   0,   0),
                      }));
    _tests.append(new BlinkTest(
                      this,
                      tr("Blink Red/Blue"),{
                          QColor(255, 0, 0),
                          QColor(0,   0, 255),
                      }));
    _tests.append(new BlinkTest(
                      this,
                      tr("Blink Red/Green"),{
                          QColor(255, 0, 0),
                          QColor(0, 255, 0),
                      }));
    _tests.append(new BlinkTest(
                      this,
                      tr("Blink Green/Blue"),{
                          QColor(0, 255, 0),
                          QColor(0, 0, 255),
                      }));

    _tests.append(new FadeTest(
                      this,
                      tr("Fade All Slow"),{
                          QColor(0,   0,   0),
                          QColor(255, 0,   0),
                          QColor(0,   255, 0),
                          QColor(0,   0, 255),
                          QColor(255, 255, 0),
                          QColor(255, 0,   255),
                          QColor(0,   255, 255),
                          QColor(255, 255, 255),
                          QColor(128, 128, 128)
                      }, 0.02));

    _tests.append(new FadeTest(
                      this,
                      tr("Fade All Fast"),{
                          QColor(0,   0,   0),
                          QColor(255, 0,   0),
                          QColor(0,   255, 0),
                          QColor(0,   0, 255),
                          QColor(255, 255, 0),
                          QColor(255, 0,   255),
                          QColor(0,   255, 255),
                          QColor(255, 255, 255),
                          QColor(128, 128, 128)
                      }, 0.25));

    _tests.append(new StrokeTest(
                      this,
                      tr("Stroke All 0°"),{
                          QColor(0,   0,   0),
                          QColor(255, 0,   0),
                          QColor(0,   255, 0),
                          QColor(0,   0, 255),
                          QColor(255, 255, 0),
                          QColor(255, 0,   255),
                          QColor(0,   255, 255),
                          QColor(255, 255, 255),
                          QColor(128, 128, 128)
                      }, StrokeTest::Angle0, true));

    _tests.append(new StrokeTest(
                      this,
                      tr("Stroke All 45°"),{
                          QColor(0,   0,   0),
                          QColor(255, 0,   0),
                          QColor(0,   255, 0),
                          QColor(0,   0, 255),
                          QColor(255, 255, 0),
                          QColor(255, 0,   255),
                          QColor(0,   255, 255),
                          QColor(255, 255, 255),
                          QColor(128, 128, 128)
                      }, StrokeTest::Angle45, true));

    _tests.append(new StrokeTest(
                      this,
                      tr("Stroke All 90°"),{
                          QColor(0,   0,   0),
                          QColor(255, 0,   0),
                          QColor(0,   255, 0),
                          QColor(0,   0, 255),
                          QColor(255, 255, 0),
                          QColor(255, 0,   255),
                          QColor(0,   255, 255),
                          QColor(255, 255, 255),
                          QColor(128, 128, 128)
                      }, StrokeTest::Angle90, true));

    _tests.append(new StrokeTest(
                      this,
                      tr("Stroke Black/White 90°"),{
                          QColor(255, 255, 255),
                          QColor(0,   0,   0)
                      }, StrokeTest::Angle90, false));

    _tests.append(new StrokeTest(
                      this,
                      tr("Stroke White/Black 90°"),{
                          QColor(0,   0,   0),
                          QColor(255, 255, 255)
                      }, StrokeTest::Angle90, false));

    _currentTestIndex = 0;
    setTest(_currentTestIndex);

    connect(&_timerRefrashImage, &QTimer::timeout, this, &TestWindow::onNeedUpdate);
    _timerRefrashImage.setInterval(1000 / _refreshRate);
    _timerRefrashImage.setSingleShot(true);
    _timerRefrashImage.start();
}

TestWindow::~TestWindow()
{
    for (AbstractTest* test : _tests)
    {
        delete test;
    }
    _tests.clear();
}

void TestWindow::paintEvent(QPaintEvent *event)
{
    _currentTest->paintEvent(event);
    _timerRefrashImage.start();
}

void TestWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key::Key_Escape:
        close();
        event->accept();
        break;
    case Qt::Key::Key_Right:
        nextTest();
        event->accept();
        break;
    case Qt::Key::Key_Left:
        previousTest();
        event->accept();
        break;
    default:
        break;
    }
}
        event->accept();
    }
}

void TestWindow::onTestEnd()
{
    nextTest();
}

void TestWindow::onNeedUpdate()
{
    if (_currentTest->needAutoUpdate()) {
        update();
    }
}

void TestWindow::setTest(int index)
{
    _currentTestIndex = index;
    _currentTest      = _tests[index];
    update();
    _timerNextTest.start(_currentTest->duration());
    emit currentTestChanged(_currentTestIndex, _currentTest);
}

void TestWindow::nextTest()
{
    _currentTestIndex++;

    if (_currentTestIndex > _tests.count() - 1) {
        _currentTestIndex = 0;
    }

    setTest(_currentTestIndex);
}

void TestWindow::previousTest()
{
    _currentTestIndex--;

    if (_currentTestIndex < 0) {
        _currentTestIndex = _tests.count() - 1;
    }

    setTest(_currentTestIndex);
}

int TestWindow::currentTestIndex() const
{
    return _currentTestIndex;
}

QList<AbstractTest *> TestWindow::tests() const
{
    return _tests;
}

void SolidColorTest::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(_window);
    painter.setPen(QPen(Qt::PenStyle::NoPen));
    painter.setBrush(QBrush(_color, Qt::BrushStyle::SolidPattern));
    painter.drawRect(0, 0, _window->size().width(), _window->size().height());
}

void BlinkTest::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(_window);
    painter.setPen(QPen(Qt::PenStyle::NoPen));
    painter.setBrush(QBrush(_colors[_currentColorIndex], Qt::BrushStyle::SolidPattern));
    painter.drawRect(0, 0, _window->size().width(), _window->size().height());

    _currentColorIndex++;
    if (_currentColorIndex > _colors.count() - 1) {
        _currentColorIndex = 0;
    }
}

void FadeTest::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    int nextColorIndex = _currentColorIndex + 1;
    if (nextColorIndex > _colors.count() - 1) {
        nextColorIndex = 0;
    }

    _blendFactor += _blendSpeed;

    const QColor& currentColor = _colors[_currentColorIndex];
    const QColor& nextColor = _colors[nextColorIndex];
    const QColor& color = QColor(
                currentColor.red()   * (1.0 - _blendFactor) + nextColor.red()   * _blendFactor,
                currentColor.green() * (1.0 - _blendFactor) + nextColor.green() * _blendFactor,
                currentColor.blue()  * (1.0 - _blendFactor) + nextColor.blue()  * _blendFactor
                );

    QPainter painter(_window);
    painter.setPen(QPen(Qt::PenStyle::NoPen));
    painter.setBrush(QBrush(color, Qt::BrushStyle::SolidPattern));
    painter.drawRect(0, 0, _window->size().width(), _window->size().height());

    if (_blendFactor >= 1.0) {
        _currentColorIndex++;
        if (_currentColorIndex > _colors.count() - 1) {
            _currentColorIndex = 0;
        }

        _blendFactor = 0.0f;
    }
}

void StrokeTest::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    float maxSide = fmax(_window->width(), _window->height());

    QPainter painter(_window);
    if (_angle == Angle0) {
        for (int i = 0; i < _window->height(); ++i) {
            painter.setPen(QPen(_colors[_currentColorIndex]));

            painter.drawLine(0, i, _window->width(), i);

            _currentColorIndex++;
            if (_currentColorIndex > _colors.count() - 1) {
                _currentColorIndex = 0;
            }
        }
    }
    if (_angle == Angle45) {
        for (int i = -maxSide; i < maxSide; ++i) {
            painter.setPen(QPen(_colors[_currentColorIndex]));

            painter.drawLine(i, 0, maxSide + i, maxSide);

            _currentColorIndex++;
            if (_currentColorIndex > _colors.count() - 1) {
                _currentColorIndex = 0;
            }
        }
    } else if (_angle == Angle90) {
        for (int i = 0; i < _window->width(); ++i) {
            painter.setPen(QPen(_colors[_currentColorIndex]));

            painter.drawLine(i, 0, i, _window->height());

            _currentColorIndex++;
            if (_currentColorIndex > _colors.count() - 1) {
                _currentColorIndex = 0;
            }
        }
    } else if (_angle == Angle135) {
        //ToDo
    }

    if (!_moving) {
        _currentColorIndex = 0;
    }
}
