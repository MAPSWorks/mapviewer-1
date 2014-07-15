#include "mapitem.h"
#include "maptile.h"
#include <QDebug>
MapItem::MapItem(DataNode *map, QObject *parent) :
    QObject(parent),
    _map(map)
{
//    for (int i = 0; i< row; i++) {
//        for (int j = 0; j< col; j++) {
//            MapTile * tile = new MapTile(i, j);
//            tile->setPos(i * 3000, j * 3000);
//            m_items.append(tile);
//        }
//    }
    MapTile * tile = new MapTile(_map,0);

    m_items.append(tile);
}


void MapItem::onViewportChanged(const Rect viewPort, const int zoom)
{
    MapTile * m = qgraphicsitem_cast<MapTile*>(m_items[0]);

    m->loadImage(viewPort,zoom);
}
