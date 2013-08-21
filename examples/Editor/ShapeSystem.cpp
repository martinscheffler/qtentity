#include "ShapeSystem"

IMPLEMENT_COMPONENT_TYPE(Shape)


Shape::Shape()
    : _renderer(nullptr)
    , _handle(0)    
{
}


void Shape::buildShape()
{
    if(!_renderer) return;
    if(_handle != 0)
    {
        _renderer->destroyShape(_handle);
    }
    QPixmap pic(_path);
    QPixmap p = pic.copy(_subtex);
    _handle = _renderer->createShape(p, _position, _zindex);
}


void Shape::setPosition(const QPoint& p)
{
    _position = p;
    if(_renderer) _renderer->updateShape(_handle, _position, _zindex);
}


void Shape::setZIndex(int i)
{
    _zindex = i;
    if(_renderer) _renderer->updateShape(_handle, _position, _zindex);
}


ShapeSystem::ShapeSystem(Renderer* renderer)
    : SimpleEntitySystem(Shape::classTypeId())
    , _renderer(renderer)
{
    QTE_ADD_PROPERTY("position", QPoint, Shape, position, setPosition);
    QTE_ADD_PROPERTY("path", QString, Shape, path, setPath);
    QTE_ADD_PROPERTY("zIndex", int, Shape, zIndex, setZIndex);
    QTE_ADD_PROPERTY("subTex", QRect, Shape, subTex, setSubtex);
}


QObject* ShapeSystem::createComponent(QtEntity::EntityId id, const QVariantMap& properties)
{
    QObject* o = SimpleEntitySystem::createComponent(id, properties);
    qobject_cast<Shape*>(o)->_renderer = _renderer;
    qobject_cast<Shape*>(o)->buildShape();
    return o;
}

const QVariantMap ShapeSystem::attributesForProperty(const QString& name) const
{
    QVariantMap r;
    if(name == "path")
    {
        r["filter"] = "SVG files (*.svg)";
    }
    return r;
}
