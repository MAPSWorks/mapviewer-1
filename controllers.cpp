#include "controllers.h"
#include "widget.h"
#include <QPushButton>
#include <QDebug>
Controllers::Controllers(QGraphicsItem *parent) :
    Layer(parent),
    m_boundingRect(0,0,0,0)
{
    setFlag(QGraphicsItem::ItemHasNoContents);
    textItem = new QGraphicsTextItem(this);
    textItem->setPos(0,0);
    textItem->setDefaultTextColor(Qt::green);
    textItem->setZValue(1000);

    QPushButton * btPlus = new QPushButton("+");
    QPushButton * btMinus = new QPushButton("-");
    btPlus->resize(50,50);
    btMinus->resize(50,50);
    plusButtomItem =  new QGraphicsProxyWidget(this);
    minusButtomItem = new QGraphicsProxyWidget(this);
    plusButtomItem->setWidget(btPlus);
    minusButtomItem->setWidget(btMinus);

    plusButtomItem->setPos(10, 50);
    minusButtomItem->setPos(10,110);

    connect(btPlus, SIGNAL(clicked()), this, SLOT(onPlusPressed()));
    connect(btMinus, SIGNAL(clicked()), this, SLOT(onMinusPressed()));
}

void Controllers::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

void Controllers::onViewSizeChanged(const QSize size)
{
    m_boundingRect.setSize(size);
}

void Controllers::onViewportChanged(const Rect viewPort, const int scale)
{
    textItem->setPlainText(QString("%1  %2 meter/pixel").arg(viewPort.toString()).arg(scale) );
}

void Controllers::onPlusPressed()
{
    // hack to get the view item.
    if (this->scene()) {
        Widget * w = dynamic_cast<Widget*>(scene()->views().at(0));
        w->setScale(w->currentScale()+1);
    }
}

void Controllers::onMinusPressed()
{
    // hack to get the view item.
    if (this->scene()) {
        Widget * w = dynamic_cast<Widget*>(scene()->views().at(0));
        w->setScale(w->currentScale()-1);
    }
}
