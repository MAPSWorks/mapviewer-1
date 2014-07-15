#ifndef LAYERINTERFACE_H
#define LAYERINTERFACE_H
#include <QGraphicsItem>
#include "misc.h"

class Layer: public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    Layer(QGraphicsItem* parent = 0):QObject(0),QGraphicsItem(parent){}
    virtual ~Layer(){}
public slots:
    virtual void onViewportChanged(const Rect viewPort, const int scale) = 0;
//    virtual void onViewportSizeChanged(const Rect viewport) = 0;
    virtual void onViewSizeChanged(const QSize size) = 0;

};
#endif // LAYERINTERFACE_H

