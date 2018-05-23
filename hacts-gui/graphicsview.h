#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QWheelEvent>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GraphicsView(QWidget *parent = nullptr);
    void scrollZoom(int numSteps);

protected:
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void scalingTime(qreal x);
    void animFinished();

private:
    int numScheduledScalings = 0;
};

#endif // GRAPHICSVIEW_H
