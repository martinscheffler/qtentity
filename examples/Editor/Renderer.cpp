#include "Renderer"
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QGraphicsPixmapItem>

class RendererImpl : public QGraphicsView
{
public:

    QGraphicsScene _scene;

    RendererImpl(QWidget* parent)
        : QGraphicsView(parent)
    {
        setMaximumSize(640, 480);
        resize(640, 480);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setScene(&_scene);
        _scene.setSceneRect(0, 0, 640, 480);
    }

    // create a shape with given texture and transform, returns an identifier
    RenderHandle createShape(const QPixmap& pic, const QPointF& pos, int zindex)
    {
        //QPixmap pm(texpath);
        //QPixmap sub = pm.copy(uv);
        QGraphicsPixmapItem* item = _scene.addPixmap(pic);
        item->setPos(pos);
        item->setZValue(zindex);
        return reinterpret_cast<RenderHandle>(item);
    }


    void destroyShape(RenderHandle handle)
    {
        QGraphicsPixmapItem* item = reinterpret_cast<QGraphicsPixmapItem*>(handle);
        _scene.removeItem(item);
        delete item;
    }


    void updateShape(RenderHandle handle, const QPointF& pos, int zindex)
    {
        QGraphicsPixmapItem* item = reinterpret_cast<QGraphicsPixmapItem*>(handle);
        item->setPos(pos);
        item->setZValue(zindex);
    }

};


Renderer::Renderer(QWidget* parent)
    : QWidget(parent)
{

    _impl = new RendererImpl(this);
    QHBoxLayout* layout = new QHBoxLayout();
    setLayout(layout);
    layout->addWidget(_impl);
}


// create a shape with given texture and transform, returns an identifier
RenderHandle Renderer::createShape(const QPixmap& pic, const QPointF& pos, int zindex)
{
    return _impl->createShape(pic, pos, zindex);
}


// destroy shape previously created with createShape
void Renderer::destroyShape(RenderHandle idx)
{
    _impl->destroyShape(idx);
}


// update transform of shape previously created with createShape
void Renderer::updateShape(RenderHandle idx, const QPointF& pos, int zindex)
{
    _impl->updateShape(idx, pos, zindex);
}
