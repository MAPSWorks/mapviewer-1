#ifndef MAPITEM_H
#define MAPITEM_H

#include <QObject>
#include <QList>
#include "misc.h"
class QGraphicsItem;
class DataNode;
class MapItem : public QObject
{
    Q_OBJECT
public:
    explicit MapItem(DataNode* map, QObject *parent = 0);
    QList<QGraphicsItem*> items() const { return m_items;}
signals:
    
public slots:
    void onViewportChanged(const Rect viewPort, const int zoom);
private:
    QList<QGraphicsItem*> m_items;
//    int row;
//    int col;
    DataNode* _map;
};

#endif // MAPITEM_H
