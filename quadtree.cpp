#include "quadtree.h"
#include <QQueue>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <math.h>
DataNode::DataNode(QString file, QPoint tl, QPoint br,int sc):
    topLeftPt(tl),
    bottomRightPt(br),
    fileName(file),
    scale(sc),
    topLeft(0),
    topRight(0),
    bottomLeft(0),
    bottomRight(0),
    parent(0),
    neighbors(8,0)
{}

QString DataNode::toString() const
{
    return QString("<file>%1</file><Point>%2,%3</Point><Point>%4,%5</Point>").arg(fileName)
            .arg(topLeftPt.x()).arg(topLeftPt.y()).arg(bottomRightPt.x()).arg(bottomRightPt.y());
}
DataNode::~DataNode() {
    if (topLeft) {
        delete topLeft;
        delete topRight;
        delete bottomLeft;
        delete bottomRight;
    }
}

const QList<QPoint> QuadTree::offsets = QList<QPoint>() << QPoint(-1,-1)<< QPoint(-1, 0)<< QPoint(-1, 1)
                                                        << QPoint(0, -1)<< QPoint(0,1)
                                                        << QPoint(1,-1) << QPoint(1,0) << QPoint(1,1);

QuadTree::QuadTree(DataNode *root):
    _root(root)
{}

QuadTree::~QuadTree()
{
    if (_root) {
        delete _root;
    }
}

void QuadTree::BFTTraverseTree(traversalFunction processFunc)
{
    if (_root) {
        QQueue<DataNode*> queue;
        queue.enqueue(_root);
        while(queue.size() > 0) {
            DataNode* n = queue.dequeue();
            if (n->topLeft) {
                queue.enqueue(n->topLeft);
                queue.enqueue(n->topRight);
                queue.enqueue(n->bottomLeft);
                queue.enqueue(n->bottomRight);
            }
            processFunc(n);
        }
    }
}

// the assumption is that the tree is always full
int QuadTree::treeHeight() const
{
    int ret = 0;
    DataNode * n = _root;
    while(n) {
        ret++;
        n = n->topLeft;
    }
    return ret;
}

int QuadTree::maxResolution() const
{
    return 1;
}

int QuadTree::minResolution() const
{
    return pow(2,treeHeight() -1 );
}

void QuadTree::saveTree(QString fileName)
{
    if (!_root) {
        return;
    }
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {

        QDataStream out(&file);
        // TODO use BFTTraverseTree function
        QQueue<DataNode*> queue;
        queue.enqueue(_root);
        while(queue.size() > 0) {
            DataNode* n = queue.dequeue();
            if (n->topLeft) {
                queue.enqueue(n->topLeft);
                queue.enqueue(n->topRight);
                queue.enqueue(n->bottomLeft);
                queue.enqueue(n->bottomRight);
            }
            //            processFunc(n);
            out<< n->fileName << n->topLeftPt<< n->bottomRightPt << n->scale;
        }


        file.close();
    }

}

DataNode * QuadTree::createNodeHelper(QDataStream& stream)
{
    QString file;
    QPoint tl, br;
    int scale;
    stream>> file >> tl>> br >> scale;
    DataNode * node = new DataNode(file, tl, br,scale);
    return node;
}

void QuadTree::loadTree(QString fileName)
{
    QFile file(fileName);
    if (!file.exists()) {
        qDebug()<<"no such file";
    }
    DataNode* root = 0;
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        QQueue<DataNode*> queue;
        if(!in.atEnd()) {
            // take the root node
            root = createNodeHelper(in);
        }

        DataNode * currentParent = root;
        int childrenCount = 0;
        while(!in.atEnd()) {
            DataNode * n =createNodeHelper(in);
            switch(childrenCount) {
            case 0:
                currentParent->topLeft = n;
                break;
            case 1:
                currentParent->topRight = n;
                break;
            case 2:
                currentParent->bottomLeft = n;
                break;
            case 3:
                currentParent->bottomRight = n;
                break;
            }
            n->parent = currentParent;
            childrenCount++;
            queue.enqueue(n);
            if (childrenCount == 4) {
                // we filled the currentParent, get the next one
                currentParent = queue.dequeue();
                childrenCount = 0;
            }
        }
    }
    file.close();
    _root = root;
    connectWithNeighbors();
}

// sorting helper function
bool yLessThan(const DataNode* n1, const DataNode* n2)
{
    return n1->topLeftPt.y() < n2->topLeftPt.y();
}
bool xLessThan(const DataNode* n1, const DataNode* n2)
{
    return n1->topLeftPt.x() < n2->topLeftPt.x();
}

bool QuadTree::validateRowAndCol(const int &row, const int &col,const int & size)
{
    return row>=0 && row < size && col >=0 && col < size;
}

void QuadTree::connectWithNeighbors()
{
    // the idea is to store the neighbour information in each node
    // so that in mapview it would be faster to query what is around the tile
    if (_root) {
        QQueue<DataNode*> q1;
        QQueue<DataNode*> q2;
        q1.enqueue(_root);
        QList<QList<DataNode*> > levels;
        while(q1.size()> 0 || q2.size() > 0) {
            QList<DataNode*> t;
            while(q1.size() > 0) {
                DataNode* n = q1.dequeue();
                t.append(n);
                if (n->topLeft) {
                    q2.enqueue(n->topLeft);
                    q2.enqueue(n->topRight);
                    q2.enqueue(n->bottomLeft);
                    q2.enqueue(n->bottomRight);
                }
                // process
            }
            levels.append(t);
            t.clear();
            while(q2.size()> 0) {
                DataNode* n = q2.dequeue();
                t.append(n);
                if (n->topLeft) {
                    q1.enqueue(n->topLeft);
                    q1.enqueue(n->topRight);
                    q1.enqueue(n->bottomLeft);
                    q1.enqueue(n->bottomRight);
                }
                // process
            }
            levels.append(t);
        }

        //process each level
        foreach(QList<DataNode*> level, levels) {
            if (level.size() > 1) {
                int rows = sqrt(level.size());
                qDebug()<<rows;
                Q_ASSERT(rows*rows == level.size());
                qSort(level.begin(), level.end(), yLessThan);
                QList<QList<DataNode*> > map;
                int c = 0;
                QList<DataNode*> t;
                while(level.size() > 0) {
                    t.append(level.takeFirst());
                    c++;
                    if (c== rows) {
                        qSort(t.begin(), t.end(), xLessThan);
                        map.append(t);
                        t.clear();
                        c = 0;
                    }
                }
                // now we have a 2d array, and contains all the tiles
                // in the level.
                // What we need to do is to set up the neighbours pointers

                // we also know that the map is of size rows*rows
                for (int i =0 ;i< rows; i++) {
                    for (int j = 0; j< rows; j++) {

                        DataNode * n = map[i][j];
                        // the magic number 8 here is the number of neighbours
                        // one tile has.
                        for (int k = 0; k< 8; k++) {
                            int r = i + offsets.at(k).x();
                            int c = j + offsets.at(k).y();
                            if (validateRowAndCol(r,c,rows)) {
                                n->neighbors[k] = map[r][c];
                            }
                        }
                    }
                }
            }
        }
    }
}

DataNode * QuadTree::findStartingTile(QPoint pt, int res) const
{
    DataNode * ret = 0;
    if (_root) {
        ret = _root;
        int levelRes = minResolution();
        do {
//            if (pt.x())
            if (levelRes <= res) {
                // found the resolution we are looking fore
                break;
            }else {
                // go next level
                if (ret->topLeft!=0) {
                    if (pt.x() < (ret->topLeftPt.x() + ret->bottomRightPt.x())/2) {
                        if (pt.y() < (ret->topLeftPt.y() + ret->bottomRightPt.y())/2) {
                            ret = ret->topLeft;
                        }else {
                            ret = ret->bottomLeft;
                        }
                    }else {
                        if (pt.y() < (ret->topLeftPt.y() + ret->bottomRightPt.y())/2) {
                            ret = ret->topRight;
                        }else {
                            ret = ret->bottomRight;
                        }
                    }
                    levelRes = levelRes / 2;
                }else {
                    // no more detail
                    break;
                }
            }
        }while(ret!=0);
    }
    return ret;
}









