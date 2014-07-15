#include "widget.h"
//#include "mapitem.h"
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QGraphicsView(parent)
{

    QGraphicsScene * scene = new QGraphicsScene(this);
    //    setViewport(new QGLWidget(this));
    setScene(scene);
    setupContext();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scene->setSceneRect(0,0, width(),height());
    //    text->setPos(0,0);

    //    setDragMode(QGraphicsView::ScrollHandDrag);
}

Widget::~Widget()
{
}

//void Widget::setMapData(DataNode *map)
//{
//    _map = map;
//}

void Widget::addLayer(QObject *layer)
{
    QGraphicsItem* gItem = dynamic_cast<QGraphicsItem*>(layer);
    if (gItem){
        scene()->addItem(gItem);
        //        addItems(gItem);

        connect(this, SIGNAL(viewportChanged(const Rect, const int)), layer, SLOT(onViewportChanged(const Rect, const int)));
        connect(this, SIGNAL(viewSizeChanged(QSize)), layer, SLOT(onViewSizeChanged(const QSize)));
    }
    emit viewportChanged(m_viewportRect, m_metersPerPixel);
}

//void Widget::refreshText()
//{
//    text->setPlainText(m_viewportRect.toString());
//    text->setPos(0,0);
//}

void Widget::setupContext()
{
    m_areaRect = (Rect){0,0,16348,16348};
    m_viewportRect = (Rect){0,0,512,512};
    m_metersPerPixel = 1;
}

// TODO remove this.
void Widget::addItems(QList<QGraphicsItem *> list)
{
    foreach(QGraphicsItem * item, list)
    {
        scene()->addItem(item);
    }
}

void Widget::updateCurrentViewport()
{
    // calculate the viewport according to view size and scale.

    m_viewportRect.width =qMin( size().width()* m_metersPerPixel,m_areaRect.width);
    m_viewportRect.height = qMin(size().height()*m_metersPerPixel,m_areaRect.height);
    emit viewportChanged(m_viewportRect,m_metersPerPixel);
}

void Widget::setScale(int mpp)
{
    if (m_metersPerPixel != mpp && mpp > 0) {
        m_metersPerPixel = mpp;
        updateCurrentViewport();
    }
}

void Widget::moveViewportToCenterAt(int x, int y)
{
    if (x < m_areaRect.width  && y < m_areaRect.height) {
        m_viewportRect.x = x - m_viewportRect.width/2;
        m_viewportRect.y = y - m_viewportRect.height/2;
        emit viewportChanged(m_viewportRect, m_metersPerPixel);
    }
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if (event->isAccepted()) {
        return;
    }
    m_lastPos =QPoint(m_viewportRect.x,m_viewportRect.y);
    m_mousePressPos = event->pos();
    event->accept();
}

void Widget::resizeEvent(QResizeEvent *event)
{
    //    qDebug()<<"size changed";
    //    m_viewportRect.width = event->size().width()* m_metersPerPixel;
    //    m_viewportRect.height = event->size().height()* m_metersPerPixel;
    scene()->setSceneRect(0,0,event->size().width(),event->size().height());
    updateCurrentViewport();
    emit viewSizeChanged(event->size());
}

QPoint Widget::getValidRectTopLeft(QPoint pt)
{
    pt.rx() = qMax(m_areaRect.x, qMin(pt.x(), m_areaRect.x + m_areaRect.width - m_viewportRect.width));
    pt.ry() = qMax(m_areaRect.y, qMin(pt.y(), m_areaRect.y + m_areaRect.height - m_viewportRect.height));
    return pt;
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    static QPoint zero(0,0);
    if (event->buttons() & Qt::LeftButton) {
        QPoint d = event->pos() - m_mousePressPos;
        if (d != zero) {
            d.rx()*=m_metersPerPixel;
            d.ry()*=m_metersPerPixel;
            QPoint t = getValidRectTopLeft(m_lastPos - d);

            if (m_viewportRect.x != t.x() || m_viewportRect.y != t.y()) {
                m_viewportRect.x = t.x();
                m_viewportRect.y = t.y();
                emit viewportChanged(m_viewportRect, m_metersPerPixel);
            }
        }
        event->accept();
    }
}

void Widget::synchronizeViewport(const Rect viewport, const int zoom)
{
    if (m_viewportRect != viewport || m_metersPerPixel != zoom) {
        m_viewportRect = viewport;
        m_metersPerPixel = zoom;
        emit viewportChanged(m_viewportRect, m_metersPerPixel);
    }
}
