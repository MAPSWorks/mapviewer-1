#include "tilegenerator.h"
#include <QDebug>
#include <math.h>
#include <QPainter>
#include "quadtree.h"
TileGenerator::TileGenerator(QObject *parent) :
    QObject(parent)
{
}

void TileGenerator::generateTiles(QString origFile, QString targetFileName, QString targetDir)
{
    // TODO check targetDir, if not exists,create one
    QImage img(origFile);
    if (img.isNull()) {
        qDebug()<<"null image";
        return;
    }
    int width = img.width();
    if (img.width() != img.height() ) {
        qDebug()<<"image should be preprocessed to be square";
        return;
    }
    int targetSide = width;
    if ((width & (width-1)) != 0) {
        // pad the image to be power of 2;
        targetSide = pow(2,ceil(log2(width)));
        qDebug()<<targetSide;
        QImage newImg(targetSide,targetSide,img.format());
        newImg.fill(Qt::transparent);
        QPainter pt(&newImg);
        pt.drawImage((targetSide - width)/2, (targetSide - width)/2,img);
        img = newImg;
    }
    // should have big image to start.
    Q_ASSERT(width > 512);
    // change to use the quadtree structure,
    m_targetDir = targetDir;
    m_targetFileName = targetFileName;

    mapData = processTile(img, 512, QPoint(0,0), QPoint(targetSide,targetSide));

    //    DataNode::BFTTraverseTree(map,&test);
}

// should pass in a quadtree structure, and return one
DataNode *  TileGenerator::processTile(QImage img, int targetSide, QPoint topLeft, QPoint bottomRight)
{
    Q_ASSERT(img.width() == img.height());
    static int i = 0;

    DataNode * ret = new DataNode();
    //    if (img.width() > targetSide) {
    QImage t = img.scaledToWidth(targetSide,Qt::SmoothTransformation );
    ret->fileName  = (m_targetDir +"/" + m_targetFileName + "_" + QString::number(i) + ".jpg");
    qDebug()<<"save file: "<< ret->fileName;
    ret->topLeftPt = topLeft;
    ret->bottomRightPt = bottomRight;
    ret->scale = img.width()/targetSide;
    // remove this
    QPainter pt(&t);
    pt.setPen(Qt::white);
    pt.drawText(50,50,ret->fileName);

    // save file
    t.save(ret->fileName);
    i++;
    //    }
    if (img.width() > targetSide) {

        /***************************************************************
             *            1
             *  +---------+---------+
             *  |         |         |
             *  |         |         |
             *  |        3|         |
             * 2+---------+---------+4
             *  |         |         |
             *  |         |         |
             *  |         |         |
             *  +---------+---------+
             *            5
             *
             *We need to calculate those 5 points, as the corresponding topleft
             *and bottomRight points for sub-tiles
             *
             **************************************************************/
        QPoint pt1((topLeft.x()+ bottomRight.x())/2, topLeft.y());
        QPoint pt2(topLeft.x(), (topLeft.y() + bottomRight.y())/2);
        QPoint pt3(pt1.x(), pt2.y());
        QPoint pt4(bottomRight.x(), pt2.y());
        QPoint pt5(pt1.x(), bottomRight.y());

        ret->topLeft = processTile(img.copy(0,0, img.width()/2,img.height()/2),targetSide,topLeft, pt3);
        ret->topRight = processTile(img.copy(img.width()/2, 0, img.width()/2,img.height()/2),targetSide,pt1,pt4);
        ret->bottomLeft = processTile(img.copy(0,             img.height()/2, img.width()/2,img.height()/2),targetSide,pt2,pt5);
        ret->bottomRight = processTile(img.copy(img.width()/2, img.height()/2, img.width()/2,img.height()/2),targetSide,pt3,bottomRight);
    }

    return ret;
}
