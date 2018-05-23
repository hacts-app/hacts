#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QGraphicsScene>

#include "graphicsview.h"
#include "carshape.h"


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

    GraphicsView *view;
    QGraphicsScene *scene;

    CarShape *carShape;

    QProcess *process;
    void processLine(const QString &line);
};

#endif // MAINWINDOW_H
