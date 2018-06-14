#pragma once

#include <QStandardItem>

class CarTreeItem : public QStandardItem
{
public:
    // inherit constructors
    using QStandardItem::QStandardItem;

    void setCarID(qint64 carId) { _carID = carId; }
    qint64 carID() const { return _carID; }

private:
    qint64 _carID;
};
