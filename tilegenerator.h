#ifndef TILEGENERATOR_H
#define TILEGENERATOR_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QSharedPointer>


class DataNode;
class TileGenerator : public QObject
{
    Q_OBJECT
public:
    explicit TileGenerator(QObject *parent = 0);

    // put here just for convenience.
    DataNode * mapData;
signals:
    
public slots:
    void generateTiles(QString origFile,
                       QString targetFileName,
                      QString targetDir = "images");
protected:
    DataNode *  processTile(QImage img, int targetSide, QPoint topLeft, QPoint bottomRight);
private:
    // the original file name
//    QString m_originalFile;
    // the target dir where tiles are stored
    QString m_targetDir;
    QString m_targetFileName;
};

#endif // TILEGENERATOR_H
