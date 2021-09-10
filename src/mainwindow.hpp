#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
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
    void onScreensChanged();
    void onTestWindowVisibleChanged(bool arg);
    void onTestChanged(int index, AbstractTest* test);

    void on_actionStart_triggered();
    void on_actionStartSelected_triggered();
    void on_actionStopTest_triggered();

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::MainWindow *ui;
    TestWindow* _testWindow = nullptr;
};
#endif // MAINWINDOW_HPP
