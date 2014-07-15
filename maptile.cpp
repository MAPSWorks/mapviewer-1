#include "maptile.h"
#include <QString>
#include <QPixmap>
#include <QDebug>
#include <QThread>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include "quadtree.h"
#include "mutexlocker.h"
void ImageLoader::run()
{
//    qDebug() << "loading "<< m_file << QThread::currentThread();
    QImage img(m_file);
    emit finishedLoading(img,m_file);
}


MapTile::MapTile(QuadTree *map, QGraphicsItem *parent) :
    Layer(0),
    _map(map),
    m_boundingRect(0,0,0,0),
    m_taskId(0)
{
    m_imageManager = new TileImageManager(this);
    init();
}


QRectF MapTile::boundingRect() const
{
    return m_boundingRect;
}

void MapTile::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    painter->setClipRect(option->exposedRect);
//    mutex.lock();
    MutexLocker ml(&mutex);
    painter->drawPixmap(0,0,m_map);
//    mutex.unlock();
}

void MapTile::init()
{
}
void MapTile::drawFrame()
{
    QList<LoaderResult> updateList = createUpdateList();

    QThread* thread = new QThread;
    m_taskId++;
    if (m_taskId == 4096) {
        m_taskId = 0;
    }
    MapBuilder* worker = new MapBuilder(updateList,boundingRect().size().toSize(),m_taskId,m_imageManager);
    worker->moveToThread(thread);
    connect(worker, SIGNAL(finishedBuilding(QImage,int)), this, SLOT(onFinishedBuilding(QImage,int)));
    connect(thread, SIGNAL(started()), worker, SLOT(build()));
    connect(worker, SIGNAL(destroyed()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), thread, SLOT(deleteLater()));
    thread->start();
}

void MapTile::onViewportChanged(const Rect viewPort, const int scale)
{
    if (m_currentScale != scale || m_currentViewport != viewPort) {
        m_currentScale = scale;
        m_currentViewport = viewPort;
        drawFrame();
    }
}

void MapTile::onFinishedBuilding(QImage img, int id)
{
    if (id != m_taskId) {
        return;
    }
//    mutex.lock();
    MutexLocker ml(&mutex);
    m_map = QPixmap::fromImage(img);
//    mutex.unlock();
    update();
}

QList<LoaderResult> MapTile::createUpdateList()
{
    // find the correct tile to draw;

    DataNode * startNode = _map->findStartingTile(QPoint(m_currentViewport.x, m_currentViewport.y),m_currentScale);
//    qDebug()<<"starting from " <<startPt <<" " << startNode->fileName;
//    qDebug()<<" sale " << startNode->scale;
    // in map coordinate
    QPoint endPt(m_currentViewport.x + m_currentViewport.width,
                 m_currentViewport.y + m_currentViewport.height);

    DataNode * t = startNode;
    DataNode * p = startNode;

    QPoint tp(m_currentViewport.x, m_currentViewport.y);

    QPoint startPt (startNode->topLeftPt);
    QList<LoaderResult> list;

    while (startPt.y() <= endPt.y() ) {

        while (startPt.x()<= endPt.x() ) {
            if(p){
//                dispatchImgLoader(p->fileName,p->topLeftPt.x() - tp.x(),p->topLeftPt.y()- tp.y(),m_taskId);
                LoaderResult result =  { p->topLeftPt.x() - tp.x(),p->topLeftPt.y()- tp.y(),p->scale,m_currentScale, p->fileName};
//                qDebug()<<"tp " << tp << "  "<<result.toString();
                list.append(result);
                startPt.rx() = p->bottomRightPt.x();
                p = p->neighbors[4];
//                 qDebug()<<"x " << startPt.x() << " " << endPt.x();
            }else {
                break;
            }

        }
        startPt.rx() = startNode->topLeftPt.x();

        if (t) {
            startPt.ry() = t->bottomRightPt.y();
            t = t->neighbors[6];
            p = t;
        }else {
            break;
        }

    }
//    qDebug()<<"\n";
    return list;
}

void MapTile::onViewSizeChanged(const QSize size)
{
    m_boundingRect.setSize(size);
//    mutex.lock();
    MutexLocker ml(&mutex);
    m_map= QPixmap((int)boundingRect().width(), (int)boundingRect().height());
    drawFrame();
//    mutex.unlock();
}

TileImageManager::TileImageManager(QObject *parent):
    QObject(parent),
    m_cache(50)
{
//    m_builderThreadPool = new QThreadPool(this);
    m_loaderThreadPool = new QThreadPool(this);
//    m_builderThreadPool->setMaxThreadCount(8);
}
void TileImageManager::startImageLoaderHelper(const QString &fileName)
{
//    m_mutex.lock();
    MutexLocker ml(&m_mutex);
    m_loadingNames.insert(fileName);
    ImageLoader * imgLoader = new ImageLoader(fileName);
    connect(imgLoader,SIGNAL(finishedLoading(QImage,QString)), this, SLOT(onLoadFinished(QImage,QString)));
    m_loaderThreadPool->start(imgLoader);
//    m_mutex.unlock();
}

void TileImageManager::loadImage(const QString &fileName)
{
    if (!m_cache.contains(fileName) && !m_loadingNames.contains(fileName)) {
        startImageLoaderHelper(fileName);
    }
}

void TileImageManager::onLoadFinished(QImage img, QString file)
{
    ImageCacheWrapper *wrapper = new ImageCacheWrapper(img);
//    m_mutex.lock();
    MutexLocker ml(&m_mutex);
    m_loadingNames.remove(file);
    m_cache.insert(file, wrapper);
    emit finishedLoadingFile(file);
//    m_mutex.unlock();
}

QImage TileImageManager::getCachedImage(const QString &fileName)
{
//    m_mutex.lock();
    MutexLocker ml(&m_mutex);
    if (m_cache.contains(fileName)) {
//        m_mutex.unlock();
        return m_cache.object(fileName)->image;
    }
//    m_mutex.unlock();
    return QImage();
}

MapBuilder::MapBuilder(QList<LoaderResult> buildlist,QSize size, int id, TileImageManager *manager):
    QObject(0),
    m_list(buildlist),
    m_id(id),
    m_manager(manager),
    m_image(size.width(), size.height(), QImage::Format_RGB32)
{
//    m_pt = new QPainter(&m_image);
//    m_pt->setClipRect(0,0,size.width(), size.height());
    m_image.fill(Qt::black);
    connect(m_manager, SIGNAL(finishedLoadingFile(QString)),
            this, SLOT(onFinishedLoadingFile(QString)));
}

void MapBuilder::onFinishedLoadingFile(QString fileName)
{
//    m_mutex.lock();
    MutexLocker ml(&m_mutex);
//    qDebug()<<"finished loading";
    for (int i = m_list.size()-1; i>=0 ; i--) {
        if (fileName == m_list.at(i).img) {
            //
            drawImageHelper(m_manager->getCachedImage(m_list.at(i).img),
                            m_list.at(i).x, m_list.at(i).y,m_list.at(i).imgScale,m_list.at(i).targetScale);
            m_list.removeAt(i);
        }
    }
    if (m_list.size() == 0) {
        emit finishedBuilding(m_image,m_id);
        emit finished();
    }
//    m_mutex.unlock();
}

void MapBuilder::drawImageHelper(QImage img, int x, int y, int imageScale, int targetScale)
{
    if (m_image.isNull()){
        return;
    }
    QPainter pt(&m_image);
    if (imageScale == targetScale) {
        pt.drawImage(x/imageScale,y/imageScale, img);
    }else{
        double is = imageScale;
        double ts = targetScale;
        double ratio = is/ts;
        pt.drawImage(QRectF(x/imageScale*ratio,y/imageScale*ratio,img.width()*ratio, img.height()*ratio),img);
    }
}

void MapBuilder::build()
{
//    m_mutex.lock();
    MutexLocker ml(&m_mutex);
    for (int i = m_list.size()-1; i>=0 ; i--) {
        QImage img(m_manager->getCachedImage(m_list.at(i).img));
        if (img.isNull()) {
            // load the image and connect the signals
            m_manager->loadImage(m_list.at(i).img);

        }else {
            // draw the image
            drawImageHelper(img,m_list.at(i).x,m_list.at(i).y,m_list.at(i).imgScale,m_list.at(i).targetScale);

            //remove the item from list
            m_list.removeAt(i);
        }
    }

    if (m_list.size() == 0) {

        emit finishedBuilding(m_image,m_id);
        emit finished();
    }
//    m_mutex.unlock();
}

