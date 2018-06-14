#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QGraphicsScene>
#include <QMap>
#include <QStandardItemModel>
#include <QItemSelection>

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

private slots:
    void processReadyRead();

    void on_zoomInButton_clicked();

    void on_zoomOutButton_clicked();

    void on_pauseButton_toggled(bool checked);

    void on_createCarButton_clicked();

    void treeSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void on_deleteSelected_clicked();

    void on_dial_sliderMoved(int position);

private:
    bool paused = false;
    int newCarId = 1; // how many cars does ai create?

    CarID selectedCarID;

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
};

#endif // MAINWINDOW_H
