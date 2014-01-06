#include "Renderer"

#include <QHBoxLayout>
#include <QtGlobal>


#include <ShapeSystem>
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
    container->setMinimumSize(480, 600);
    container->setMaximumSize(480, 600);
    container->setFocusPolicy(Qt::TabFocus);
    _impl->_view->setSource(QUrl("qrc:/assets/main.qml"));
    setLayout(new QHBoxLayout());
    layout()->addWidget(container);
    
    _impl->_shapeComponent = new QQmlComponent(_impl->_view->engine(), QUrl("qrc:/assets/shape.qml"), QQmlComponent::PreferSynchronous);

    if (_impl->_shapeComponent->isError())
    {
        qWarning() << _impl->_shapeComponent->errors();
    }
}


void Renderer::installRendererEventFilter(QObject* o)
{
    _impl->_view->installEventFilter(o);
}


// create a shape with given texture and transform, returns an identifier
void Renderer::addShape(Shape* shape)
{
    QObject *object = _impl->_shapeComponent->create();

    Q_ASSERT(object);
    QQmlProperty::write(object, "path", "qrc" + shape->_path);

    qobject_cast<QQuickItem*>(object)->setParentItem(_impl->_view->rootObject());
    Q_ASSERT(shape->_handle == nullptr);
    shape->_handle = object;
    updateShape(shape);
}


// destroy shape previously created with createShape
void Renderer::removeShape(Shape* shape)
{
    QObject* item = reinterpret_cast<QObject*>(shape->_handle);
    delete item;
    shape->_handle = nullptr;
}


// update transform of shape previously created with createShape
void Renderer::updateShape(Shape* shape)
{
   QObject* object = reinterpret_cast<QObject*>(shape->_handle);
   QPoint pos = shape->_position;
   QQmlProperty::write(object, "x", pos.x());
   QQmlProperty::write(object, "y", pos.y());
   QQmlProperty::write(object, "z", shape->_zindex);
   QRect s = shape->_subtex;
   QQmlProperty::write(object, "clipX", s.x());
   QQmlProperty::write(object, "clipY", s.y());
   QQmlProperty::write(object, "width", s.width());
   QQmlProperty::write(object, "height", s.height());
}

