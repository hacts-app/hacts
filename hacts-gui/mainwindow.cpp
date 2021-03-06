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
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValueRef>

#include "carshape.h"
#include "cartreeitem.h"

static qreal carwidth = 1.7, carheight = 4.02;

template <typename T>
static inline T myClamp(T a, T b, T c) {
    if(a < b)
        return b;
    if(a > c)
        return c;
    return a;
}

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
            //send(QString("setacceleration %1 0\n").arg(selectedCarID).toUtf8());
            setAcceleration(selectedCarID, 0);

    if(ev->key() == Qt::Key_A || ev->key() == Qt::Key_D)
        if(!heldKeys.contains(Qt::Key_A) && !heldKeys.contains(Qt::Key_D))
            //send(QString("setacceleration %1 0\n").arg(selectedCarID).toUtf8());
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

        if(carShape->isMovedByHand())
            return;

        double lastx = carShape->x();
        double lasty = carShape->y();
        double x = commandParts.value(2).toDouble();
        double y = commandParts.value(3).toDouble();
        double angle = commandParts.value(4).toDouble();

        if(!qFuzzyCompare(x, lastx) || !qFuzzyCompare(y, lasty)) {
            scene->addLine(lastx, lasty, x, y, pen);
        }

        carShape->setPos(x, y);
        carShape->setRotation(-90 + angle);

        displayCarAngleIfNeeded(id, -90 + angle);
    }
}

void MainWindow::drawLine(const QJsonArray &coords)
{
    for(int i = 2; i < coords.count(); i += 2) {
        scene->addLine(coords[i-2].toDouble(), coords[i-1].toDouble(), coords[i].toDouble(), coords[i+1].toDouble());
    }
}

void MainWindow::loadRoad()
{
    QFile file("./data.json");
    if(! file.open(QIODevice::ReadOnly)) {
        qWarning() << file.errorString();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    for(const QJsonValueRef line : doc["ways"].toArray()) {
        drawLine(line.toArray());
    }

    /*
    waQTextStream stream(&file);
    QString line;
    while(stream.readLineInto(&line)) {
        QStringList splitStr = line.split('\t');
        QStringList coords = splitStr.value(3, "").split(",");
        for(int i = 2; i < coords.length(); i += 2) {
            scene->addLine(coords[i-2].toDouble(), coords[i-1].toDouble(), coords[i].toDouble(), coords[i+1].toDouble());
        }
    }
    */
}

void MainWindow::displayOptionsForCar(CarID id, const QString &name)
{
    if(selectedCarID != id && (heldKeys.contains(Qt::Key_W) || heldKeys.contains(Qt::Key_S))) {
        setAcceleration(id, _oldacceleration);
        setAcceleration(selectedCarID, 0);
    }

    if(isAnyCarSelected) {
        // went from a selected car to another selected car

        sendAutopilot(selectedCarID, true); // the old car should drive itself now
    }

    isAnyCarSelected = true;
    selectedCarID = id;
    ui->carOptionsGroupBox->setTitle(name);
    ui->carOptionsGroupBox->show();

    ui->steeringWheelDial->setValue(carTurn.value(id, 5000));
    ui->autoPilotCheckBox->setChecked(true);
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
    //send(QString("rotatecar %1 %2\n").arg(selectedCarID).arg(angle).toUtf8());
}

void MainWindow::setAcceleration(CarID id, int direction)
{
    ensureNotAutopilotOnSelected();

    //qDebug() << QString("setacceleration %1 %2\n").arg(id).arg(direction).toUtf8();
    send(QString("setacceleration %1 %2\n").arg(id).arg(direction).toUtf8());
    _oldacceleration = direction;
}

void MainWindow::setTurning(int direction)
{
    ensureNotAutopilotOnSelected();

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
            connect(turningTimer, &QTimer::timeout, this, &MainWindow::turningTimerFired);
            turningTimer->start(30);
        }
    }

}

void MainWindow::updateSteeringDial(int value)
{
    ui->steeringWheelDial->setValue(
                myClamp(ui->steeringWheelDial->value() + value, 0, 10000));
}

void MainWindow::sendTurn(CarID id, double turn)
{
    ensureNotAutopilotOnSelected();
    send(QString("setsteeringangle %1 %2\n").arg(id).arg(turn, 0, 'g', 15).toUtf8());
}

void MainWindow::sendAutopilot(CarID id, bool autopilot)
{
    send(QString("setautopilot %1 %2\n").arg(id).arg(autopilot ? "1" : "0").toUtf8());
}

void MainWindow::ensureNotAutopilotOnSelected()
{
    if(!isAnyCarSelected)
        return;

    if(ui->autoPilotCheckBox->checkState() != Qt::Unchecked) {
        ui->autoPilotCheckBox->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::send(const QByteArray &data)
{
    process->write(data);
    qDebug() << "sent" << data;
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
    connect(carShape, &CarShape::putDownByUser, this, &MainWindow::carPutDownByUser);

    scene->addItem(carShape);

    CarTreeItem *item = new CarTreeItem(QString("Samochód ") + QString::number(id));
    item->setCarID(id);
    treeModel->item(0, 0)->appendRow(item);

    return carShape;
}

void MainWindow::on_pauseButton_toggled(bool checked)
{
    if(checked)
        send("pause\n");
    else
        send("resume\n");

    paused = !checked;
}

void MainWindow::on_createCarButton_clicked()
{
    if(!paused)
        send("pause\n");

    QPointF viewCenter = view->mapToScene(view->viewport()->rect()).boundingRect().center();

    send(QString("newcar %1\n").arg(newCarId).toUtf8());
    send(QString("movecar %1 %2 %3\n")
                   .arg(newCarId)
                   .arg(viewCenter.x())
                   .arg(viewCenter.y()).toUtf8());

    getCarById(newCarId);
    focusCarOnTree(newCarId);

    newCarId++;

    if(!paused)
        send("resume\n");
}

void MainWindow::treeSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected) // don't need no warnings

    QModelIndexList selectedIndexes = selected.indexes();
    if(selectedIndexes.empty() || selectedIndexes.first().parent() != treeModel->index(0, 0)) {
        // selected a non-car
        ui->carOptionsGroupBox->hide();
        if(isAnyCarSelected) {
            // from a selected car
            sendAutopilot(selectedCarID, false);
        }
        isAnyCarSelected = false;
        return;
    }

    QStandardItem *carItem = treeModel->itemFromIndex(selectedIndexes.first());
    CarID id = dynamic_cast<CarTreeItem *>(carItem)->carID();

    displayOptionsForCar(id, carItem->text());
}

void MainWindow::on_deleteSelected_clicked()
{
    send(QString("deletecar %1\n").arg(selectedCarID).toUtf8());
}

void MainWindow::on_dial_sliderMoved(int position)
{
    ensureNotAutopilotOnSelected();
    double angle = -qDegreesToRadians(position * 0.1) - 0.5*M_PI;
    send(QString("rotatecar %1 %2\n").arg(selectedCarID).arg(angle, 0, 'g', 15).toUtf8());
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

    ensureNotAutopilotOnSelected();

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

void MainWindow::carPutDownByUser(CarID id)
{
    QPointF pos = cars[id]->pos();
    send(QString("movecar %1 %2 %3\n").arg(id).arg(pos.x(), 0, 'g', 15).arg(pos.y(), 0, 'g', 15).toUtf8());
}

void MainWindow::on_autoPilotCheckBox_stateChanged(int checked)
{
    if(!isAnyCarSelected)
        return;
    sendAutopilot(selectedCarID, !!checked);

    if(!!checked) {
        ui->steeringWheelDial->setValue(5000); // reset the steering wheel as autopilot takes control
    }
}
