#pragma once

#include <QObject>
#include <QGraphicsPolygonItem>
#include <QPointF>
#include <QGraphicsObject>

class CarShape : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT

public:
    CarShape(qint64 id, qreal width, qreal length, QGraphicsItem *parent = nullptr);

    qint64 getId() { return id; }

    bool isMovedByHand();

signals:
    void putDownByUser(qint64 id);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    bool isBeingMovedByHand = false;

    qint64 id;
};
