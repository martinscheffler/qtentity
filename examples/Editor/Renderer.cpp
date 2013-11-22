#include "Renderer"

#include <QHBoxLayout>
#include <QtGlobal>

#if 0
// Use graphics View based renderer

#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>


class RendererImpl : public QObject
{
public:

    RendererImpl(QObject* parent)
        : QObject(parent)
    {
    }

    QGraphicsView _view;
    QGraphicsScene _scene;
};


Renderer::Renderer(QWidget* parent)
    : QWidget(parent)
{

    _impl = new RendererImpl(this);
    _impl->_view.setScene(&_impl->_scene);
    _impl->_scene.setSceneRect(0, 0, 640, 480);
    setFocusPolicy(Qt::NoFocus);
    QHBoxLayout* layout = new QHBoxLayout();
    setLayout(layout);
    layout->addWidget(&_impl->_view);
}


// create a shape with given texture and transform, returns an identifier
RenderHandle Renderer::createShape(const QString& path, const QRect& rect, const QPointF& pos, int zindex)
{
    QPixmap pic(path);
    QPixmap p = pic.copy(rect);
    QGraphicsPixmapItem* item = _impl->_scene.addPixmap(p);
    item->setPos(pos);
    item->setZValue(zindex);
    return reinterpret_cast<RenderHandle>(item);
}


// destroy shape previously created with createShape
void Renderer::destroyShape(RenderHandle handle)
{
    QGraphicsPixmapItem* item = reinterpret_cast<QGraphicsPixmapItem*>(handle);
    _impl->_scene.removeItem(item);
    delete item;
}


// update transform of shape previously created with createShape
void Renderer::updateShape(RenderHandle handle, const QPointF& pos, int zindex)
{
    QGraphicsPixmapItem* item = reinterpret_cast<QGraphicsPixmapItem*>(handle);
    item->setPos(pos);
    item->setZValue(zindex);
}

#else

#include <QQuickItem>
#include <QQuickView>
#include <QQmlComponent>
#include <QQmlProperty>

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
RenderHandle Renderer::createShape(const QString& path, const QRect& rect, const QPointF& pos, int zindex)
{

    QObject *object = _impl->_shapeComponent->create();
    QQmlProperty::write(object, "x", pos.x());
    QQmlProperty::write(object, "y", pos.y());
    QQmlProperty::write(object, "z", zindex);
    QQmlProperty::write(object, "width", rect.width());
    QQmlProperty::write(object, "height", rect.height());

    Q_ASSERT(object->children().size() == 1);
    QObject* img = object->children().front();
    Q_ASSERT(img);
    QQmlProperty::write(img, "source", "qrc" + path);
    QQmlProperty::write(img, "x", rect.x() * -1);
    QQmlProperty::write(img, "y", rect.y() * -1);

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
