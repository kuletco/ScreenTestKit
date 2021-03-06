#ifndef TESTWINDOW_HPP
#define TESTWINDOW_HPP

#include <QRasterWindow>
#include <functional>
#include <QTimer>
#include <QTranslator>

class TestWindow;
class AbstractTest
{
public:
    AbstractTest(TestWindow* window, const QString& name, int duration) : _window(window), _name(name), _duration(duration) {
        assert(window != nullptr);
    }
    virtual ~AbstractTest() { _window = nullptr; }
    virtual void paintEvent(QPaintEvent *event) = 0;
    QString name()        const { return _name; }
    int duration()        const { return _duration; }
    bool needAutoUpdate() const { return _needAutoUpdate; }

protected:
    TestWindow* _window;
    QString _name;
    int _duration = 3000;
    bool _needAutoUpdate = false;
};

class MainWindow;
class TestWindow : public QRasterWindow
{
    Q_OBJECT
public:
    friend class MainWindow;
    explicit TestWindow(QScreen *targetScreen = nullptr, QWindow *parent = nullptr);
    ~TestWindow();
    void initTests();
    void setTestScreen(QScreen *screen);
    void startTests(int index = 0);
    void stopTest();

    QList<AbstractTest*> tests() const { return _tests; };
    int currentTestIndex() const { return _currentTestIndex; };
    void setTest(int index);

    void setLoop(bool loop) { _loop = loop; }
    void setAutoNext(bool autonext) {
        _autonext = autonext;
        if (_timerNextTest.isActive()) {
            if (_autonext) {
                _timerNextTest.start(_currentTest->duration());
            } else {
                _timerNextTest.stop();
            }
        }
    }

signals:
    void currentTestChanged(int index, const AbstractTest* test);
    void testStopped(int index);

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void onTestEnd();
    void onNeedUpdate();

private:
    bool nextTest();
    bool previousTest();

    bool _loop = false;
    bool _autonext = false;
    int _currentTestIndex = 0;
    AbstractTest* _currentTest;
    QList<AbstractTest*> _tests;
    QTimer _timerNextTest;

    qreal _refreshRate = 60;
    QTimer _timerRefrashImage;
};

class ContrastTest : public AbstractTest
{
public:
    ContrastTest(TestWindow * window, const QString &name) : AbstractTest(window, name, 3000) {}
    void paintEvent(QPaintEvent *event) override;
private:
};

class SolidColorTest : public AbstractTest
{
public:
    SolidColorTest(TestWindow* window, const QString& name, const QColor& color) : AbstractTest(window, name, 3000), _color(color) {}
    void paintEvent(QPaintEvent *event) override;
private:
    QColor _color = Qt::GlobalColor::red;
};

class BlinkTest : public AbstractTest
{
public:
    BlinkTest(TestWindow* window, const QString& name, const QList<QColor>& colors) : AbstractTest(window, name, 6000), _colors(colors) {
        _needAutoUpdate = true;
    }
    void paintEvent(QPaintEvent *event) override;
private:
    QList<QColor> _colors;
    int _currentColorIndex = 0;
};

class FadeTest : public AbstractTest
{
public:
    FadeTest(TestWindow* window, const QString& name, const QList<QColor>& colors, double blendSpeed) : AbstractTest(window, name, 30000), _colors(colors), _blendSpeed(blendSpeed) {
        _needAutoUpdate = true;
        _currentColor = colors.first();
    }
    void paintEvent(QPaintEvent *event) override;
private:
    QList<QColor> _colors;
    QColor _currentColor;
    double _blendFactor = 0;
    double _blendSpeed = 0.01;
    int _currentColorIndex = 0;
};

class StrokeTest : public AbstractTest
{
public:
    enum CapabilityAngle{
        Angle0,
        Angle45,
        Angle90,
        Angle135
    };
    StrokeTest(TestWindow* window, const QString& name, const QList<QColor>& colors, CapabilityAngle angle, bool moving) : AbstractTest(window, name, 10000), _colors(colors), _angle(angle), _moving(moving) {
        _needAutoUpdate = moving;
    }
    void paintEvent(QPaintEvent *event) override;
private:
    QList<QColor> _colors;
    int _currentColorIndex = 0;
    CapabilityAngle _angle = Angle0;
    bool _moving = false;
};

#endif // TESTWINDOW_HPP
