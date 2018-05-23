#pragma once

#include <QObject>
#include <QGraphicsPolygonItem>

class CarShape : public QGraphicsPolygonItem
{
public:
    CarShape(QGraphicsItem *parent = nullptr);
};
