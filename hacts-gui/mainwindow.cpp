#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsView>
#include <QDir>
#include <QDebug>
#include <QGridLayout>
#include <QPen>

#include "carshape.h"

static QPen pen;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    pen.setWidthF(0.1);

    ui->setupUi(this);

    process = new QProcess(this);
    //process->setWorkingDirectory(r("../hacts-ai/"));

#ifdef Q_OS_WIN
    process->start("./hacts-ai.exe");
#else
    process->start("./hacts-ai");
#endif


    view = new GraphicsView(this);
    ui->gridLayout->replaceWidget(ui->graphicsViewWidget, view);

    scene = new QGraphicsScene(this);
    view->setScene(scene);
    scene->addLine(0, 5, 10, 100, pen);
    scene->addLine(-3, 300, 100, 150, pen);


    carShape = new CarShape();
    scene->addItem(carShape);

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
    qDebug() << line;
    QStringList commandParts = line.split(" ");

    double x = commandParts.value(1).toDouble();
    double y = commandParts.value(2).toDouble();
    double angle = commandParts.value(3).toDouble();
    carShape->setX(x);
    carShape->setY(y);
    carShape->setRotation(-90 + angle);
}

void MainWindow::on_zoomInButton_clicked()
{
    view->scrollZoom(10);
}

void MainWindow::on_zoomOutButton_clicked()
{
    view->scrollZoom(-10);
}
