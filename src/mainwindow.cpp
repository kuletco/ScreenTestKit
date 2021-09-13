#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
#include <QToolButton>
#include <QGuiApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionStartSelected->setVisible(true);//ToDo

    onScreensChanged();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onScreensChanged()
{
    ui->listWidgetScreens->clear();
    QList<QScreen*> screens = QGuiApplication::screens();

    for (int i = 0; i < screens.count(); ++i) {
        ui->listWidgetScreens->addItem(tr("Display %1").arg(i));
    }

    if (ui->listWidgetScreens->count() > 0 && ui->listWidgetScreens->currentRow() <= -1) {
        ui->listWidgetScreens->setCurrentRow(0);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    if (_testWindow) {
        _testWindow->close();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event)
    if (_testWindow && _testWindow->isVisible()) {
        _testWindow->keyPressEvent(event);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if (_testWindow && _testWindow->isVisible()) {
        _testWindow->mousePressEvent(event);
    }
}

void MainWindow::onTestWindowVisibleChanged(bool arg)
{
    if (!arg) {
        if (sender() == _testWindow) {
            delete _testWindow;
            _testWindow = nullptr;
            ui->actionStart->setEnabled(true);
            ui->actionStartSelected->setEnabled(true);
            ui->actionStopTest->setEnabled(false);

        } else {
            qWarning() << Q_FUNC_INFO << ": sender() != _testWindow";
        }
    }
}

void MainWindow::onTestChanged(int index, AbstractTest *test)
{
    Q_UNUSED(index)
    Q_UNUSED(test)
    ui->listWidgetTests->clear();
    for (int i = 0; i < _testWindow->tests().count(); ++i) {
        const AbstractTest* test = _testWindow->tests().at(i);
        ui->listWidgetTests->addItem(test->name());
    }

    ui->listWidgetTests->item(index)->setIcon(QIcon(":/resources/images/play.svg"));
    ui->listWidgetTests->scrollToItem(ui->listWidgetTests->item(index));
}

void MainWindow::on_actionStart_triggered()
{
    QList<QScreen*> screens = QGuiApplication::screens();
    int selectedScreen = 0;
    if (ui->listWidgetScreens->currentRow() >= 0 && ui->listWidgetScreens->currentRow() <= screens.count()) {
        selectedScreen = ui->listWidgetScreens->currentRow();
    }

    if (_testWindow) {
        _testWindow->close();
    }

    _testWindow = new TestWindow(screens[selectedScreen]);
    on_actionLoop_triggered();
    on_actionAutoNext_triggered();

    connect(_testWindow, &TestWindow::visibleChanged,     this, &MainWindow::onTestWindowVisibleChanged);
    connect(_testWindow, &TestWindow::currentTestChanged, this, &MainWindow::onTestChanged);

    onTestChanged(0, _testWindow->tests().at(0));
    _testWindow->showFullScreen();

    ui->actionStart->setEnabled(false);
    ui->actionStartSelected->setEnabled(false);
    ui->actionStopTest->setEnabled(true);
}

void MainWindow::on_actionStartSelected_triggered()
{
    on_actionStart_triggered();
}

void MainWindow::on_actionStopTest_triggered()
{
    if (_testWindow) {
        _testWindow->close();
    }
}

void MainWindow::on_actionLoop_triggered()
{
    _loop = ui->actionLoop->isChecked();
    if (_testWindow && _testWindow->isVisible()) {
        _testWindow->setLoop(_loop);
    }
}

void MainWindow::on_actionAutoNext_triggered()
{
    _autonext = ui->actionAutoNext->isChecked();
    if (_testWindow && _testWindow->isVisible()) {
        _testWindow->setAutoNext(_autonext);
    }
}
