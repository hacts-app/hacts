#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QGraphicsScene>
#include <QMap>
#include <QStandardItemModel>

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

private:
    Ui::MainWindow *ui;
    QStandardItemModel *treeModel;

    GraphicsView *view;
    QGraphicsScene *scene;

    QMap<CarID, CarShape *> cars;
    CarShape *getCarById(CarID id);

    QProcess *process;
    void processLine(const QString &line);

    void loadRoad();

};

#endif // MAINWINDOW_H
