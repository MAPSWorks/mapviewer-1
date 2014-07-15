#ifndef MAPTILE_H
#define MAPTILE_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QRunnable>
#include <QImage>
#include <QList>
#include <QPixmap>
#include <QMap>
#include <QCache>
#include <QMutex>
#include <QSet>
#include <QThreadPool>
#include "layerinterface.h"
class QuadTree;
struct LoaderResult{
    int x;
    int y;
    int imgScale;
    int targetScale;
    QString img;
    QString toString() const {
        return QString("%1,%2,%3,%4,%5").arg(x).arg(y).arg(imgScale).arg(targetScale).arg(img);
    }
};
struct ImageCacheWrapper {
    QImage image;
    ImageCacheWrapper(QImage img):
        image(img)
    {}
};

class TileImageManager;
class MapTile : public Layer
{
    Q_OBJECT
public:
    explicit MapTile(QuadTree * map,QGraphicsItem *parent = 0);
    QRectF boundingRect() const ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
signals:
    
public slots:
    void init();
    void onViewportChanged(const Rect viewPort, const int scale);
    void onViewSizeChanged(const QSize size);
private slots:
    void onFinishedBuilding(QImage img, int id);
private:
    QList<LoaderResult> createUpdateList();
    void drawFrame();
private:
    QuadTree * _map;
    QRectF m_boundingRect;
    Rect m_currentViewport;
    int m_currentScale;
    QPixmap m_map;
    QMutex mutex;
    TileImageManager * m_imageManager;
    unsigned int m_taskId;
};

class MapBuilder: public QObject {
    Q_OBJECT
public:
    MapBuilder(QList<LoaderResult> buildlist,QSize size, int id,TileImageManager* manager);

public slots:
    void build();
    void onFinishedLoadingFile(QString fileName);

signals:
    void finishedBuilding(QImage img,int id);
    void finished();
private:
    void drawImageHelper(QImage img, int x, int y,int imageScale, int targetScale);

private:
    QList<LoaderResult> m_list;
    int m_id;
    TileImageManager* m_manager;
    QImage m_image;
    QPainter* m_pt;
    QMutex m_mutex;
};

class TileImageManager: public QObject {
    Q_OBJECT
public:
    TileImageManager(QObject* parent = 0);

public slots:
    void onLoadFinished(QImage img,QString file);
    QImage getCachedImage(const QString & fileName);
    void loadImage(const QString & fileName);
private:
    void startImageLoaderHelper(const QString & fileName);
signals:
    void finishedLoadingFile(QString fileName);
private:
    QCache<QString, ImageCacheWrapper> m_cache;
    QSet<QString> m_loadingNames;
    QMutex m_mutex;
    QThreadPool *m_loaderThreadPool;
};


class ImageLoader:public QObject ,public QRunnable {
    Q_OBJECT
public:
    ImageLoader(QString fileName,int x = 0, int y = 0,int id = 0):QRunnable(),
        m_file(fileName),
        m_x(x), m_y(y), m_id(id){}
    void run();
signals:
    void finished(QImage img,QString file, int x, int y,int id);
    void finishedLoading(QImage img, QString fileName);
private:
    QString m_file;
    int m_x;
    int m_y;
    int m_id;
};

#endif // MAPTILE_H
