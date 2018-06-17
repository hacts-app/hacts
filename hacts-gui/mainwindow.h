#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QGraphicsScene>
#include <QMap>
#include <QStandardItemModel>
#include <QItemSelection>
#include <QSet>

#include "graphicsview.h"
#include "carshape.h"

using CarID = qint64;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;

private slots:
    void processReadyRead();

    void on_zoomInButton_clicked();

    void on_zoomOutButton_clicked();

    void on_pauseButton_toggled(bool checked);

    void on_createCarButton_clicked();

    void treeSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void on_deleteSelected_clicked();

    void on_dial_sliderMoved(int position);

    void on_dial_sliderPressed();

    void on_dial_sliderReleased();

    void turningTimerFired();

    void on_steeringWheelDial_valueChanged(int position);

    void carPutDownByUser(CarID id);

    void on_autoPilotCheckBox_stateChanged(int checked);

private:
    bool paused = false;
    int newCarId = 1; // how many cars does ai create?

    bool sliderPressed = false;

    QSet<Qt::Key> heldKeys;

    CarID selectedCarID;
    bool isAnyCarSelected = false;

    Ui::MainWindow *ui;
    QStandardItemModel *treeModel;

    GraphicsView *view;
    QGraphicsScene *scene;

    QMap<CarID, CarShape *> cars;
    CarShape *getCarById(CarID id);

    QProcess *process;
    void processLine(const QString &line);

    void loadRoad();

    void displayOptionsForCar(CarID id, const QString &name);
    void focusCarOnTree(CarID id);

    void displayCarAngleIfNeeded(CarID id, double angleInDegrees);

    void setAcceleration(CarID id, int direction);

    int _oldacceleration = 0;

    void setTurning(int direction);

    int _turningNow = 0;
    QMap<CarID, double> carTurn;
    QTimer *turningTimer = nullptr;
    void updateSteeringDial(int value);
    void sendTurn(CarID id, double turn);
    void sendAutopilot(CarID id, bool autopilot);
    void ensureNotAutopilotOnSelected();

    void send(const QByteArray &data);
};

#endif // MAINWINDOW_H
