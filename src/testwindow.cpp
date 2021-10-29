#include "testwindow.hpp"
#include <QPainter>
#include <QDebug>
#include <QKeyEvent>
#include <QScreen>
#include <cmath>

#define QCOLOR_000000   QColor(0,   0,   0)     // Black
#define QCOLOR_FFFFFF   QColor(255, 255, 255)   // White
#define QCOLOR_808080   QColor(128, 128, 128)   // Gray
#define QCOLOR_FF0000   QColor(255, 0,   0)     // Red
#define QCOLOR_00FF00   QColor(0,   255, 0)     // Green
#define QCOLOR_0000FF   QColor(0,   0,   255)   // Blue
#define QCOLOR_FFFF00   QColor(255, 255, 0)     // Yellow
#define QCOLOR_FF00FF   QColor(255, 0,   255)   // Magenta
#define QCOLOR_00FFFF   QColor(0,   255, 255)   // Cyan

double deg2rad (double degrees) {
    return degrees * 4.0 * atan(1.0) / 180.0;
}

TestWindow::TestWindow(QScreen *targetScreen, QWindow *parent) : QRasterWindow(parent)
{
    if (targetScreen) {
        setScreen(targetScreen);
        _refreshRate = targetScreen->refreshRate();
    }

    initTests();

    _currentTestIndex = 0;

    connect(&_timerNextTest, &QTimer::timeout, this, &TestWindow::onTestEnd);

    connect(&_timerRefrashImage, &QTimer::timeout, this, &TestWindow::onNeedUpdate);
    _timerRefrashImage.setInterval(1000 / _refreshRate);
    _timerRefrashImage.setSingleShot(true);
}

TestWindow::~TestWindow()
{
    if (_timerNextTest.isActive() || this->isVisible()) {
        stopTest();
    }
    while (!_tests.isEmpty()) {
        delete _tests.takeFirst();
    }
    _tests.clear();
}

void TestWindow::initTests()
{
    // Solid
    _tests.append(new SolidColorTest(this, tr("Solid Black"),   QCOLOR_000000));
    _tests.append(new SolidColorTest(this, tr("Solid White"),   QCOLOR_FFFFFF));
    _tests.append(new SolidColorTest(this, tr("Solid Gray"),    QCOLOR_808080));
    _tests.append(new SolidColorTest(this, tr("Solid Red"),     QCOLOR_FF0000));
    _tests.append(new SolidColorTest(this, tr("Solid Green"),   QCOLOR_00FF00));
    _tests.append(new SolidColorTest(this, tr("Solid Blue"),    QCOLOR_0000FF));
    _tests.append(new SolidColorTest(this, tr("Solid Yellow"),  QCOLOR_FFFF00));
    _tests.append(new SolidColorTest(this, tr("Solid Magenta"), QCOLOR_FF00FF));
    _tests.append(new SolidColorTest(this, tr("Solid Cyan"),    QCOLOR_00FFFF));

    // Blink
    _tests.append(new BlinkTest(this, tr("Blink All"),         { QCOLOR_000000, QCOLOR_FF0000, QCOLOR_00FF00, QCOLOR_0000FF, QCOLOR_FFFF00, QCOLOR_FF00FF, QCOLOR_00FFFF, QCOLOR_FFFFFF, QCOLOR_808080 }));
    _tests.append(new BlinkTest(this, tr("Blink RGB"),         { QCOLOR_FF0000, QCOLOR_00FF00, QCOLOR_0000FF, }));
    _tests.append(new BlinkTest(this, tr("Blink Black/White"), { QCOLOR_FFFFFF, QCOLOR_000000, }));
    _tests.append(new BlinkTest(this, tr("Blink Red/Blue"),    { QCOLOR_FF0000, QCOLOR_0000FF, }));
    _tests.append(new BlinkTest(this, tr("Blink Red/Green"),   { QCOLOR_FF0000, QCOLOR_00FF00, }));
    _tests.append(new BlinkTest(this, tr("Blink Green/Blue"),  { QCOLOR_00FF00, QCOLOR_0000FF, }));

    // Fade
    _tests.append(new FadeTest(this, tr("Fade All Slow"), { QCOLOR_000000, QCOLOR_FF0000, QCOLOR_00FF00, QCOLOR_0000FF, QCOLOR_FFFF00, QCOLOR_FF00FF, QCOLOR_00FFFF, QCOLOR_FFFFFF, QCOLOR_808080 }, 0.02));
    _tests.append(new FadeTest(this, tr("Fade All Fast"), { QCOLOR_000000, QCOLOR_FF0000, QCOLOR_00FF00, QCOLOR_0000FF, QCOLOR_FFFF00, QCOLOR_FF00FF, QCOLOR_00FFFF, QCOLOR_FFFFFF, QCOLOR_808080 }, 0.25));

    // Stroke
    _tests.append(new StrokeTest(this, tr("Stroke All 0°"),  { QCOLOR_000000, QCOLOR_FF0000, QCOLOR_00FF00, QCOLOR_0000FF, QCOLOR_FFFF00, QCOLOR_FF00FF, QCOLOR_00FFFF, QCOLOR_FFFFFF, QCOLOR_808080 }, StrokeTest::Angle0, true));
    _tests.append(new StrokeTest(this, tr("Stroke All 45°"), { QCOLOR_000000, QCOLOR_FF0000, QCOLOR_00FF00, QCOLOR_0000FF, QCOLOR_FFFF00, QCOLOR_FF00FF, QCOLOR_00FFFF, QCOLOR_FFFFFF, QCOLOR_808080 }, StrokeTest::Angle45, true));
    _tests.append(new StrokeTest(this, tr("Stroke All 90°"), { QCOLOR_000000, QCOLOR_FF0000, QCOLOR_00FF00, QCOLOR_0000FF, QCOLOR_FFFF00, QCOLOR_FF00FF, QCOLOR_00FFFF, QCOLOR_FFFFFF, QCOLOR_808080 }, StrokeTest::Angle90, true));
    _tests.append(new StrokeTest(this, tr("Stroke Black/White 90°"), { QCOLOR_FFFFFF, QCOLOR_000000 }, StrokeTest::Angle90, false));
    _tests.append(new StrokeTest(this, tr("Stroke White/Black 90°"), { QCOLOR_000000, QCOLOR_FFFFFF }, StrokeTest::Angle90, false));

    qDebug().noquote() << "Tests Count:" << _tests.count();
}

void TestWindow::setTestScreen(QScreen *screen)
{
    if (screen) {
        setScreen(screen);
        _refreshRate = screen->refreshRate();
    }
}

void TestWindow::startTests(int index)
{
    //qDebug().noquote() << "StartAt:" << index;
    setTest(index);
    _timerRefrashImage.start();
}

void TestWindow::stopTest()
{
    if (_timerNextTest.isActive()) {
        _timerNextTest.stop();
    }
    if (_timerRefrashImage.isActive()) {
        _timerRefrashImage.stop();
    }
    hide();
    //qDebug().noquote() << "StopAt:" << _currentTestIndex;
    emit testStopped(_currentTestIndex);
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
        stopTest();
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

void TestWindow::mousePressEvent(QMouseEvent *event)
{
    switch (event->button()) {
    case Qt::MouseButton::LeftButton:
        nextTest();
        event->accept();
        break;
    default:
        break;
    }
}

void TestWindow::onTestEnd()
{
    if (_autonext) {
        nextTest();
    }
}

void TestWindow::onNeedUpdate()
{
    if (_currentTest->needAutoUpdate()) {
        update();
    }
}

void TestWindow::setTest(int index)
{
    if (index < 0 || index >= _tests.count()) {
        return;
    }
    _currentTestIndex = index;
    _currentTest      = _tests[index];
    update();
    if (_autonext) {
        _timerNextTest.start(_currentTest->duration());
    }
    //qDebug().noquote() << "CurrentTest:" << _currentTestIndex;
    emit currentTestChanged(_currentTestIndex, _currentTest);
}

bool TestWindow::nextTest()
{
    _currentTestIndex++;

    if (_currentTestIndex > _tests.count() - 1) {
        _currentTestIndex = 0;
        if (!_loop) {
            stopTest();
            return false;
        }
    }

    setTest(_currentTestIndex);
    return true;
}

bool TestWindow::previousTest()
{
    _currentTestIndex--;

    if (_currentTestIndex < 0) {
        _currentTestIndex = _tests.count() - 1;
        if (!_loop) {
            stopTest();
            return false;
        }
    }

    setTest(_currentTestIndex);
    return true;
}

void ContrastTest::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
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
