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

private slots:
    void processReadyRead();

    void on_zoomInButton_clicked();

private:
    Ui::MainWindow *ui;

    QProcess *process;
    void processLine(const QString &line);
};

#endif // MAINWINDOW_H
