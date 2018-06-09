#pragma once

#include <QObject>
#include <QGraphicsPolygonItem>

class CarShape : public QGraphicsPolygonItem
{
    qint64 id;

public:
    CarShape(qint64 id, qreal width, qreal length, QGraphicsItem *parent = nullptr);

    qint64 getId() { return id; }
};
