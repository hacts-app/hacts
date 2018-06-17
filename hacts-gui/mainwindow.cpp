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
#include <QModelIndexList>
#include <QKeyEvent>
#include <QTimer>

#include "carshape.h"
#include "cartreeitem.h"

static qreal carwidth = 1.7, carheight = 4.02;

static inline double steeringDialToValue(int position) {
   return (position - 5000) / 10000.0 * 2.0;
}

static inline int steeringValueToDial(double position) {
   return position * 10000.0 / 2.0 + 5000;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->carOptionsGroupBox->hide(); // no way to do this from the designer

    treeModel = new QStandardItemModel(this);
    treeModel->invisibleRootItem()->appendRow(new QStandardItem("Samochody"));
    ui->treeView->setModel(treeModel);
    ui->treeView->setExpanded(treeModel->index(0, 0), true);

    QItemSelectionModel *selectionModel = ui->treeView->selectionModel();
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(treeSelectionChanged(QItemSelection,QItemSelection)));

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

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if(ev->isAutoRepeat() || ev->type() != QEvent::KeyPress) {
        QMainWindow::keyPressEvent(ev);
        return;
    }

    switch(ev->key()) {
        case Qt::Key_W:
        case Qt::Key_S:
        case Qt::Key_A:
        case Qt::Key_D:
            heldKeys |= static_cast<Qt::Key>(ev->key());
            break;

        default:
            // didn't handle it, pass to qt;
            QMainWindow::keyPressEvent(ev);
            return;
    }

    if(ev->key() == Qt::Key_W && isAnyCarSelected)
        setAcceleration(selectedCarID, 1);
    else if(ev->key() == Qt::Key_S && isAnyCarSelected)
        setAcceleration(selectedCarID, -1);
    else if(ev->key() == Qt::Key_A)
        setTurning(-1);
    else if(ev->key() == Qt::Key_D)
        setTurning(1);
}

void MainWindow::keyReleaseEvent(QKeyEvent *ev)
{
    if(ev->isAutoRepeat() || ev->type() != QEvent::KeyRelease) {
        // didn't handle it, pass to qt;
        QMainWindow::keyPressEvent(ev);
        return;
    }
    switch(ev->key()) {
        case Qt::Key_W:
        case Qt::Key_S:
        case Qt::Key_A:
        case Qt::Key_D:
            heldKeys.remove(static_cast<Qt::Key>(ev->key()));
            break;

        default:
            QMainWindow::keyPressEvent(ev);
            return;
    }

    if(ev->key() == Qt::Key_W || ev->key() == Qt::Key_S)
        if(!heldKeys.contains(Qt::Key_W) && !heldKeys.contains(Qt::Key_S))
            //process->write(QString("setacceleration %1 0\n").arg(selectedCarID).toUtf8());
            setAcceleration(selectedCarID, 0);

    if(ev->key() == Qt::Key_A || ev->key() == Qt::Key_D)
        if(!heldKeys.contains(Qt::Key_A) && !heldKeys.contains(Qt::Key_D))
            //process->write(QString("setacceleration %1 0\n").arg(selectedCarID).toUtf8());
            setTurning(0);
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

        displayCarAngleIfNeeded(id, -90 + angle);
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

void MainWindow::displayOptionsForCar(CarID id, const QString &name)
{
    if(selectedCarID != id && (heldKeys.contains(Qt::Key_W) || heldKeys.contains(Qt::Key_S))) {
        setAcceleration(id, _oldacceleration);
        setAcceleration(selectedCarID, 0);
    }
    ui->steeringWheelDial->setValue(carTurn.value(selectedCarID, 5000));

    isAnyCarSelected = true;
    selectedCarID = id;
    ui->carOptionsGroupBox->setTitle(name);
    ui->carOptionsGroupBox->show();
}

void MainWindow::focusCarOnTree(CarID id)
{
    QStandardItem *samochodyItem = treeModel->item(0, 0);
    for(int i = samochodyItem->rowCount() - 1; i >= 0; i--) {
        CarTreeItem *item = dynamic_cast<CarTreeItem *>(samochodyItem->child(i, 0));
        if(item->carID() == id) {
            ui->treeView->selectionModel()->setCurrentIndex(item->index(), QItemSelectionModel::ClearAndSelect);
            return;
        }
    }
}

void MainWindow::displayCarAngleIfNeeded(CarID id, double angleInDegrees)
{
    if(!isAnyCarSelected || selectedCarID != id)
        return;

    if(sliderPressed)
        return;

    ui->dial->setValue(10 * -(180 + angleInDegrees));

    //double angle = -qDegreesToRadians(position * 0.1) - 0.5*M_PI;
    //process->write(QString("rotatecar %1 %2\n").arg(selectedCarID).arg(angle).toUtf8());
}

void MainWindow::setAcceleration(CarID id, int direction)
{
    qDebug() << QString("setacceleration %1 %2\n").arg(id).arg(direction).toUtf8();
    _oldacceleration = direction;
}

void MainWindow::setTurning(int direction)
{
    _turningNow = direction;
    if(direction == 0) {
        if(turningTimer != nullptr) {
            turningTimer->stop();
            turningTimer->deleteLater();
            turningTimer = nullptr;
        }
    } else {
        if(turningTimer == nullptr) {
            turningTimer = new QTimer(this);
            connect(turningTimer, SIGNAL(timeout()), this, SLOT(turningTimerFired()));
            turningTimer->start(30);
        }
    }

}

void MainWindow::updateSteeringDial(int value)
{
    ui->steeringWheelDial->setValue(
                clamp(ui->steeringWheelDial->value() + value, 0, 10000));

}

void MainWindow::sendTurn(CarID id, double turn)
{
    qDebug() << QString("setsteeringangle %1 %2\n").arg(id).arg(turn, 0, 'g', 15).toUtf8();
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

    CarTreeItem *item = new CarTreeItem(QString("Samochód ") + QString::number(id));
    item->setCarID(id);
    treeModel->item(0, 0)->appendRow(item);

    return carShape;
}

void MainWindow::on_pauseButton_toggled(bool checked)
{
    if(checked)
        process->write("pause\n");
    else
        process->write("resume\n");

    paused = !checked;
}

void MainWindow::on_createCarButton_clicked()
{
    if(!paused)
        process->write("pause\n");

    QPointF viewCenter = view->mapToScene(view->viewport()->rect()).boundingRect().center();

    process->write(QString("newcar %1\n").arg(newCarId).toUtf8());
    process->write(QString("movecar %1 %2 %3\n")
                   .arg(newCarId)
                   .arg(viewCenter.x())
                   .arg(viewCenter.y()).toUtf8());

    getCarById(newCarId);
    focusCarOnTree(newCarId);

    newCarId++;

    if(!paused)
        process->write("resume\n");
}

void MainWindow::treeSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected) // don't need no warnings

    QModelIndexList selectedIndexes = selected.indexes();
    if(selectedIndexes.empty() || selectedIndexes.first().parent() != treeModel->index(0, 0)) {
        ui->carOptionsGroupBox->hide();
        isAnyCarSelected = false;
        return;
    }

    QStandardItem *carItem = treeModel->itemFromIndex(selectedIndexes.first());
    CarID id = dynamic_cast<CarTreeItem *>(carItem)->carID();

    displayOptionsForCar(id, carItem->text());
}

void MainWindow::on_deleteSelected_clicked()
{
    process->write(QString("deletecar %1\n").arg(selectedCarID).toUtf8());
}

void MainWindow::on_dial_sliderMoved(int position)
{
    double angle = -qDegreesToRadians(position * 0.1) - 0.5*M_PI;
    process->write(QString("rotatecar %1 %2\n").arg(selectedCarID).arg(angle, 0, 'g', 15).toUtf8());
}

void MainWindow::on_dial_sliderPressed()
{
    sliderPressed = true;
    on_dial_sliderMoved(ui->dial->value());
}

void MainWindow::on_dial_sliderReleased()
{
    sliderPressed = false;
}

void MainWindow::turningTimerFired()
{
    if(!isAnyCarSelected)
        return;

    if(! carTurn.contains(selectedCarID))
        carTurn[selectedCarID] = 0;

    updateSteeringDial(200 * _turningNow);

    //sendTurn(selectedCarID, carTurn[selectedCarID]);
}
void MainWindow::on_steeringWheelDial_valueChanged(int position)
{
    carTurn[selectedCarID] = position;
    //carTurn[selectedCarID] = (position - 5000) / 10000.0 / 1.75;

    sendTurn(selectedCarID, steeringDialToValue(position));
}
