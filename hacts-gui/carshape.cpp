#include "carshape.h"
#include <QPolygonF>
#include <QPointF>
#include <QBrush>
#include <QColor>
#include <QPen>

CarShape::CarShape(qint64 id, qreal width, qreal length, QGraphicsItem *parent) :
    QGraphicsPolygonItem(parent),
    id(id)
{
    const qreal halfwidth = width/2, halflength = length/2;

    const QVector<QPointF> shape {
        {halfwidth, -halflength}, {halfwidth, halflength}, {-halfwidth, halflength}, {-halfwidth, -halflength}
    };
    setPolygon(shape);

    setBrush(QBrush(QColor("#6495ED")));

    QPen pen;
    pen.setWidthF(0.1);
    setPen(pen);

    QGraphicsPolygonItem *arrow = new QGraphicsPolygonItem(this);
    arrow->setBrush(QBrush(QColor("#FF6666")));
    const QVector<QPointF> arrowShape {
        {0, -.6}, {-.6, -1.0}, {0, 1.0}, {.6, -1.0}
    };
    arrow->setPolygon(arrowShape);
    arrow->setPen(pen);
}
