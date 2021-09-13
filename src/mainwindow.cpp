
#include <QGuiApplication>
#include <QToolButton>
#include <QKeyEvent>
#include <QScreen>
#include <QPointer>
#include <QDebug>

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionStartSelected->setVisible(true);//ToDo

    _testWindow = new TestWindow();
    connect(_testWindow, &TestWindow::visibleChanged,     this, &MainWindow::onTestWindowVisibleChanged);
    connect(_testWindow, &TestWindow::currentTestChanged, this, &MainWindow::onTestChanged);
    connect(_testWindow, &TestWindow::testStopped,        this, &MainWindow::onTestStopped);
    on_actionLoop_triggered();
    on_actionAutoNext_triggered();

    UpdateTestList();
    onScreensChanged();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _testWindow;
}

void MainWindow::UpdateTestList()
{
    ui->listWidgetTests->clear();
    const QList<AbstractTest*> tests = _testWindow->tests();
    for (const AbstractTest *test : tests) {
        ui->listWidgetTests->addItem(test->name());
    }
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

    if (_testWindow) {
        _testWindow->hide();
    }

    int selectedScreen = 0;
    if (ui->listWidgetScreens->currentRow() >= 0 && ui->listWidgetScreens->currentRow() <= screens.count()) {
        selectedScreen = ui->listWidgetScreens->currentRow();
    }
    if (_testWindow) {
        //_testWindow->hide();
        _testWindow->setTestScreen(screens[selectedScreen]);
    }
}

void MainWindow::onTestWindowVisibleChanged(bool arg)
{
    if (!arg) {
        if (sender() == _testWindow) {
            ui->actionStart->setEnabled(true);
            ui->actionStartSelected->setEnabled(true);
            ui->actionStopTest->setEnabled(false);

        } else {
            qWarning() << Q_FUNC_INFO << ": sender() != _testWindow";
        }
    }
}

void MainWindow::onTestChanged(int index, const AbstractTest* test)
{
    Q_UNUSED(test)

    UpdateTestList();

    ui->listWidgetTests->item(index)->setIcon(QIcon(":/resources/images/play.svg"));
    ui->listWidgetTests->scrollToItem(ui->listWidgetTests->item(index));
}

void MainWindow::onTestStopped(int index)
{
    ui->listWidgetTests->item(index)->setIcon(QIcon());
    ui->listWidgetTests->scrollToItem(ui->listWidgetTests->item(0));
}

void MainWindow::on_actionStart_triggered()
{
    QList<QScreen*> screens = QGuiApplication::screens();
    int selectedScreen = 0;
    if (ui->listWidgetScreens->currentRow() >= 0 && ui->listWidgetScreens->currentRow() < screens.count()) {
        selectedScreen = ui->listWidgetScreens->currentRow();
    }
    _testWindow->setTestScreen(screens[selectedScreen]);


//    onTestChanged(0, _testWindow->tests().at(0));
    _testWindow->showFullScreen();

    ui->actionStart->setEnabled(false);
    ui->actionStartSelected->setEnabled(false);
    ui->actionStopTest->setEnabled(true);

    int selectedTest = 0;
    if (ui->listWidgetTests->currentRow() >= 0 && ui->listWidgetTests->currentRow() < _testWindow->tests().count()) {
        selectedTest = ui->listWidgetTests->currentRow();
    }
    _testWindow->startTests(selectedTest);
}

void MainWindow::on_actionStartSelected_triggered()
{
    on_actionStart_triggered();
}

void MainWindow::on_actionStopTest_triggered()
{
    if (_testWindow) {
        _testWindow->stopTest();
    }
}

void MainWindow::on_actionLoop_triggered()
{
    _loop = ui->actionLoop->isChecked();
    if (_testWindow) {
        _testWindow->setLoop(_loop);
    }
}

void MainWindow::on_actionAutoNext_triggered()
{
    _autonext = ui->actionAutoNext->isChecked();
    if (_testWindow) {
        _testWindow->setAutoNext(_autonext);
    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    if (_testWindow) {
        _testWindow->hide();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (_testWindow && _testWindow->isVisible()) {
        _testWindow->keyPressEvent(event);
    } else {
        if (event->key() == Qt::Key::Key_Escape) {
            qApp->exit();
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (_testWindow && _testWindow->isVisible()) {
        _testWindow->mousePressEvent(event);
    }
}
