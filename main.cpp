#include "widget.h"
#include "tilegenerator.h"
#include "quadtree.h"
#include <QApplication>
#include <QDebug>
#include <QDataStream>
#include <QByteArray>
#include <QPoint>
//#include "mapitem.h"
#include "maptile.h"
#include "controllers.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // generate the tiles and save tree to a file.
    //    TileGenerator generator;
    //    qDebug()<<"generating..";
    //    generator.generateTiles("earth.jpg","earth_tile","tiles");
    //    QuadTree t(generator.mapData);
    //   t.saveTree("map.data");

    Widget w;
//    Widget w1;
//    Widget w2;
//    Widget w3;

    QuadTree tree;
    tree.loadTree("map.data");

    MapTile* map = new MapTile(&tree);
//    MapTile* map1 = new MapTile(&tree);
//    MapTile* map2 = new MapTile(&tree);
//    MapTile* map3 = new MapTile(&tree);

    Controllers  controller;
//    Controllers  controller1;
//    Controllers  controller2;
//    Controllers  controller3;

    w.addLayer(map);
    w.addLayer(&controller);
    w.resize(800,600);

//    w1.addLayer(map1);
//    w1.addLayer(&controller1);
//    w1.resize(800,600);

//    w2.addLayer(map2);
//    w2.addLayer(&controller2);
//    w2.resize(800,600);

//    w3.addLayer(map3);
//    w3.addLayer(&controller3);
//    w3.resize(800,600);

    w.show();
//    w1.show();
//    w2.show();
//    w3.show();


//    QObject::connect(&w, SIGNAL(viewportChanged(Rect,int)), &w1, SLOT(synchronizeViewport(Rect,int)));
//    QObject::connect(&w, SIGNAL(viewportChanged(Rect,int)), &w2, SLOT(synchronizeViewport(Rect,int)));
//    QObject::connect(&w, SIGNAL(viewportChanged(Rect,int)), &w3, SLOT(synchronizeViewport(Rect,int)));
    w.moveViewportToCenterAt(8000,8000);

    //    qDebug()<<DataNode::treeHeight(map);
    //    return 0;
    int ret =  a.exec();
    //    delete mapdata;
    delete map;
    return ret;
}
