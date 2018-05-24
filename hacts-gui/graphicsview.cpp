#include "graphicsview.h"
#include <QTimeLine>
#include <QBrush>
#include <QLineF>
#include <QPointF>

GraphicsView::GraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);

    // flip the y axis
    scale(1, -1);

    setBackgroundBrush(QBrush("#e8eBeA", Qt::Dense1Pattern));

    setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);
}


void GraphicsView::addBorder(qreal diameter)
{
    QPen pen;
    pen.setWidthF(20);

    qreal h = diameter/2;
    const QPointF topLeft {-h, -h}, topRight {-h, h};
    const QPointF bottomLeft {h, -h}, bottomRight {h, h};
    scene()->addLine(QLineF(topLeft, topRight), pen);
    scene()->addLine(QLineF(topRight, bottomRight), pen);
    scene()->addLine(QLineF(bottomRight, bottomLeft), pen);
    scene()->addLine(QLineF(bottomLeft, topLeft), pen);
}


// based on Qt wiki:

void GraphicsView::wheelEvent(QWheelEvent *event) {
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15; // see QWheelEvent documentation
    scrollZoom(numSteps);
}

void GraphicsView::scalingTime(qreal x) {
    Q_UNUSED(x)
    qreal factor = 1.0 + qreal(numScheduledScalings) / 300.0;
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
