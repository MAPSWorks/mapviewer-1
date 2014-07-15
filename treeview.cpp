#include "treeview.h"

TreeView::TreeView(QuadTree *map, QWidget *parent):
    QGraphicsView(parent),
    m_map(map)
{

}
