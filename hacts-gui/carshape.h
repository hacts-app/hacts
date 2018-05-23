#pragma once

#include <QObject>
#include <QGraphicsPolygonItem>

class CarShape : public QGraphicsPolygonItem
{
public:
    CarShape(qreal width, qreal length, QGraphicsItem *parent = nullptr);
};
