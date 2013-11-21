#include "Renderer"
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QGraphicsPixmapItem>
#include <QQuickItem>
#include <QQuickView>
#include <QQmlComponent>
#include <QQmlProperty>


#if 0
// Use graphics View based renderer
class RendererImpl : public QGraphicsView
{
public:

    QGraphicsScene _scene;

    RendererImpl(QWidget* parent)
        : QGraphicsView(parent)
    {
        setScene(&_scene);
        _scene.setSceneRect(0, 0, 640, 480);
        setFocusPolicy(Qt::NoFocus);
    }

    // create a shape with given texture and transform, returns an identifier
    RenderHandle createShape(const QPixmap& pic, const QPointF& pos, int zindex)
    {
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

#else
// Use Qml based renderer

class RendererImpl : public QObject
{
public:

    RendererImpl(QObject* parent)
        : QObject(parent)
    {
    }

    QQuickView* _view;
    QQmlComponent* _shapeComponent;
};


Renderer::Renderer(QWidget* parent)
    : QWidget(parent)
{
    _impl = new RendererImpl(this);
    _impl->_view = new QQuickView();
    QWidget *container = QWidget::createWindowContainer(_impl->_view, this);
    container->setMinimumSize(640, 400);
    container->setMaximumSize(800, 600);
    container->setFocusPolicy(Qt::TabFocus);
    _impl->_view->setSource(QUrl("qrc:/assets/main.qml"));
    setLayout(new QHBoxLayout());
    layout()->addWidget(container);
    
    _impl->_shapeComponent = new QQmlComponent(_impl->_view->engine(), QUrl("qrc:/assets/shape.qml"), QQmlComponent::PreferSynchronous);

}


// create a shape with given texture and transform, returns an identifier
RenderHandle Renderer::createShape(const QPixmap& pic, const QPointF& pos, int zindex)
{
    
    QObject *object = _impl->_shapeComponent->create();
    QQmlProperty::write(object, "x", pos.x());
    QQmlProperty::write(object, "y", pos.y());
    QQmlProperty::write(object, "z", zindex);
    QQuickItem* item = qobject_cast<QQuickItem*>(object); 
    item->setParentItem(_impl->_view->rootObject());
    return reinterpret_cast<RenderHandle>(object);
}


// destroy shape previously created with createShape
void Renderer::destroyShape(RenderHandle handle)
{
    QObject* item = reinterpret_cast<QObject*>(handle);
    delete item;
}


// update transform of shape previously created with createShape
void Renderer::updateShape(RenderHandle handle, const QPointF& pos, int zindex)
{
   QObject* object = reinterpret_cast<QObject*>(handle);
   QQmlProperty::write(object, "x", pos.x());
    QQmlProperty::write(object, "y", pos.y());
    QQmlProperty::write(object, "z", zindex);
}
#endif