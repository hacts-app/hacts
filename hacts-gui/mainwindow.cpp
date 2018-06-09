#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsView>
#include <QDir>
#include <QDebug>
#include <QGridLayout>
#include <QPen>
#include <QFile>
#include <QTextStream>
#include <QtMath>
#include <QStandardItem>

#include "carshape.h"

static qreal carwidth = 3.23, carheight = 4.02;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    treeModel = new QStandardItemModel(this);
    treeModel->invisibleRootItem()->appendRow(new QStandardItem("Samochody"));
    ui->treeView->setModel(treeModel);
    ui->treeView->setExpanded(treeModel->index(0, 0), true);

    process = new QProcess(this);

#ifdef Q_OS_WIN
    process->start("hacts-ai.exe");
#else
    process->start("./hacts-ai");
#endif


    view = new GraphicsView(this);
    ui->gridLayout->replaceWidget(ui->graphicsViewWidget, view);

    scene = new QGraphicsScene(this);
    view->setScene(scene);
    view->addBorder(10000);

    connect(process, SIGNAL(readyRead()), this, SLOT(processReadyRead()));

    loadRoad();
}

MainWindow::~MainWindow()
{
    process->close();

    delete ui;
}


void MainWindow::processReadyRead()
{
    while(process->canReadLine()) {
        QString line(process->readLine());
        processLine(line);
        //        qDebug() << line;
    }
}

void MainWindow::processLine(const QString &line)
{
    QPen pen;
    pen.setWidthF(.25);

    //qDebug() << line;
    QStringList commandParts = line.split(" ");

    if(commandParts.value(0) == "movecar") {
        CarID id = commandParts.value(1).toLongLong();
        CarShape *carShape = getCarById(id);

        double lastx = carShape->x();
        double lasty = carShape->y();
        double x = commandParts.value(2).toDouble();
        double y = commandParts.value(3).toDouble();
        double angle = commandParts.value(4).toDouble();

        if(!qFuzzyCompare(x, lastx) || !qFuzzyCompare(y, lasty)) {
            scene->addLine(lastx, lasty, x, y, pen);
        }

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

CarShape *MainWindow::getCarById(CarID id)
{
    if(cars.contains(id)) {
        return cars.value(id);
    }

    // this is the first time we've seen this car.
    CarShape *carShape = new CarShape(id, carwidth, carheight);
    cars[id] = carShape;

    scene->addItem(carShape);

    treeModel->item(0, 0)->appendRow(
        new QStandardItem(
            QString("Samochód ") + QString::number(id)
        )
    );

    return carShape;
}
