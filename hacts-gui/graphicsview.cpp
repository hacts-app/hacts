#include "graphicsview.h"
#include <QTimeLine>
#include <QBrush>

GraphicsView::GraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);

    setBackgroundBrush(QBrush("#282B2A", Qt::Dense7Pattern));
}

// based on Qt wiki

void GraphicsView::wheelEvent(QWheelEvent *event) {
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15; // see QWheelEvent documentation
    scrollZoom(numSteps);
}

void GraphicsView::scalingTime(qreal x) {
    Q_UNUSED(x)
    qreal factor = 1.0+ qreal(numScheduledScalings) / 300.0;
    scale(factor, factor);
}

void GraphicsView::animFinished() {
    if (numScheduledScalings > 0)
        numScheduledScalings--;
    else
        numScheduledScalings++;
    sender()->deleteLater();
}

void GraphicsView::scrollZoom(int numSteps) {
    numScheduledScalings += numSteps;
    if (numScheduledScalings * numSteps < 0) // if user moved the wheel in another direction, we reset previously scheduled scalings
        numScheduledScalings = numSteps;

    QTimeLine *anim = new QTimeLine(350, this);
    anim->setUpdateInterval(20);

    connect(anim, SIGNAL(valueChanged(qreal)), this, SLOT(scalingTime(qreal)));
    connect(anim, SIGNAL(finished()), this, SLOT(animFinished()));
    anim->start();
}
