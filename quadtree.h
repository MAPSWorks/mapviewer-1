#ifndef QUADTREE_H
#define QUADTREE_H

#include <QString>
#include <QPoint>
//#include <QList>
//template <class T>
#include <QSharedPointer>
#include <QList>
#include <QVector>
struct DataNode{
//    T data;
public:
    QPoint topLeftPt;
    QPoint bottomRightPt;
    QString fileName;
    int scale;
    DataNode * topLeft;
    DataNode * topRight;
    DataNode * bottomLeft;
    DataNode * bottomRight;
    DataNode * parent;

    QVector<DataNode*> neighbors;
//    DataNode();
    DataNode(QString file = QString(),
             QPoint tl = QPoint(),
             QPoint br = QPoint(),
             int sc = 0);
    QString toString() const;

//    QString itemToString() const;
    ~DataNode();
//    static void printTree(DataNode* root);



};

class QuadTree {

public:
    QuadTree(DataNode * root = 0);
    ~QuadTree();
    DataNode* root() {return _root;}

    typedef void(*traversalFunction)(DataNode*);

    void BFTTraverseTree(traversalFunction processFunc);

    int treeHeight() const;

    void saveTree(QString fileName);

    void loadTree(QString fileName);

    int maxResolution() const;

    int minResolution() const;

    DataNode * findStartingTile(QPoint pt, int res) const;
protected:
    DataNode* createNodeHelper(QDataStream& stream);
    void connectWithNeighbors();
    static bool validateRowAndCol(const int &row, const int &col,const int & size);
private:
    DataNode* _root;

    static const QList<QPoint> offsets;
};






#endif // QUADTREE_H
