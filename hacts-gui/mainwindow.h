#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots: // to dodane
    void processReadyRead(); // to dodane

private:
    Ui::MainWindow *ui;

    QProcess *process; //to dodane
    void processLine(const QString &line); // i to dodane xd
};

#endif // MAINWINDOW_H
