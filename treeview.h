#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QGraphicsView>
#include "quadtree.h"
class TreeView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TreeView(QuadTree * map, QWidget *parent = 0);
    
signals:
    
public slots:
    
private:
    QuadTree * m_map
};

#endif // TREEVIEW_H
