#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QPoint>
#include "misc.h"

class QGraphicsItem;
class QMouseEvent;
//class MapItem;
class DataNode;
class Widget : public QGraphicsView
{
    Q_OBJECT
    
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
public:
    Widget(QWidget *parent = 0);
    ~Widget();

    void addItems(QList<QGraphicsItem*> list);

    void setupContext();

    // make an interface
    void addLayer(QObject* layer);
//    void setMapData(DataNode* map);
    int currentScale() const { return m_metersPerPixel;}
public slots:
    void setScale(int mpp);
    void moveViewportToCenterAt(int x, int y);
    //temp
    void synchronizeViewport(const Rect viewport, const int zoom);
signals:
    void viewportChanged(const Rect viewPort, const int zoom);
//    void viewportSizeChanged(const Rect size);
    void viewSizeChanged(const QSize size);
private:
    QPoint getValidRectTopLeft(QPoint pt);
    void updateCurrentViewport();
    // temp
//    void refreshText();
private:
//    QPoint m_topLeft;
//    QPoint m_bottomRight;
//    QPoint m_topoTopLeft;
//    QPoint m_topoBottomRight;
    Rect m_areaRect;
    Rect m_viewportRect;
//    QGraphicsTextItem * text;
    QPoint m_lastPos;
    QPoint m_mousePressPos;
    DataNode * _map;
    int m_metersPerPixel;
};

#endif // WIDGET_H
