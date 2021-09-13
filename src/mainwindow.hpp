#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QPointer>
#include "testwindow.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void UpdateTestList();
    void onScreensChanged();
    void onTestWindowVisibleChanged(bool arg);
    void onTestChanged(int index, const AbstractTest *test);
    void onTestStopped(int index);

    void on_actionStart_triggered();
    void on_actionStartSelected_triggered();
    void on_actionStopTest_triggered();
    void on_actionLoop_triggered();
    void on_actionAutoNext_triggered();

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::MainWindow *ui;
    TestWindow* _testWindow;
    int _selectedScreen = 0;
    bool _loop = false;
    bool _autonext = false;
};
#endif // MAINWINDOW_HPP
