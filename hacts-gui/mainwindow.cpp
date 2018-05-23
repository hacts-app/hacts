#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsView>
#include <QDir>
#include <QDebug>
#include <QGridLayout>
#include <QPen>
#include <QFile>
#include <QTextStream>

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
    view->addBorder(10000);

    carShape = new CarShape();
    scene->addItem(carShape);

    connect(process, SIGNAL(readyRead()), this, SLOT(processReadyRead()));

    loadRoad();
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
    //qDebug() << line;
    QStringList commandParts = line.split(" ");

    if(commandParts.value(0) == "movecar") {
        double x = commandParts.value(2).toDouble();
        double y = commandParts.value(3).toDouble();
        double angle = commandParts.value(4).toDouble();
        carShape->setX(x);
        carShape->setY(y);
        carShape->setRotation(-90 + angle);
    }
}

void MainWindow::loadRoad()
{
    QFile file("./data.txt");
    if(! file.open(QIODevice::ReadOnly)) {
        qWarning() << file.errorString();
        return;
    }
    QTextStream stream(&file);
    QString line;
    while(stream.readLineInto(&line)) {
        QStringList splitStr = line.split('\t');
        QStringList coords = splitStr.value(3, "").split(",");
        for(int i = 2; i < coords.length(); i += 2) {
            scene->addLine(coords[i-2].toDouble(), coords[i-1].toDouble(), coords[i].toDouble(), coords[i+1].toDouble());
        }
    }
}

void MainWindow::on_zoomInButton_clicked()
{
    view->scrollZoom(10);
}

void MainWindow::on_zoomOutButton_clicked()
{
    view->scrollZoom(-10);
}
