#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    process = new QProcess(this);

#ifdef Q_OS_WIN
    process->start("../hacts-ai/hacts-ai.exe");
#else
    process->start("../hacts-ai/hacts-ai");
#endif
    connect(process, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::processReadyRead()
{
    while(process->canReadLine()) {
        QString line(process->readLine());
        processLine(line);
    }
}

void MainWindow::processLine(const QString &line)
{
    QStringList commandParts = line.split(" ");
    qDebug() << commandParts[0];
}
