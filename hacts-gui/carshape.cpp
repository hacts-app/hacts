#include "carshape.h"
#include <QPolygonF>
#include <QPointF>
#include <QBrush>
#include <QColor>
#include <QPen>

CarShape::CarShape(QGraphicsItem *parent) :
    QGraphicsPolygonItem(parent)
{
    const QVector<QPointF> shape { {0, -2}, {-2, -3.3}, {0, 3.3}, {2, -3.3} };
    setPolygon(shape);

    setBrush(QBrush(QColor("#6495ED")));

    QPen pen;
    pen.setWidthF(0.1);
    setPen(pen);
}
