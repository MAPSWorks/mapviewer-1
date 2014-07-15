#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include "layerinterface.h"
#include <QGraphicsProxyWidget>
class Controllers :public Layer
{
    Q_OBJECT
public:
    explicit Controllers(QGraphicsItem *parent = 0);
    QRectF boundingRect() const { return m_boundingRect;}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
signals:
    
public slots:
    void onViewportChanged(const Rect viewPort, const int scale);
    void onViewSizeChanged(const QSize size);

    void onPlusPressed();
    void onMinusPressed();

private:
    QGraphicsTextItem* textItem;
    QGraphicsProxyWidget* plusButtomItem;
    QGraphicsProxyWidget* minusButtomItem;
    QRectF m_boundingRect;
};

#endif // CONTROLLERS_H
