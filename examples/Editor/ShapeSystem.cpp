#include "ShapeSystem"

#include <QtEntity/PropertyAccessor>

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


ShapeSystem::ShapeSystem(QtEntity::EntityManager* em, Renderer* renderer)
    : BaseClass(em)
    , _renderer(renderer)
{
}


QtEntity::Component* ShapeSystem::createComponent(QtEntity::EntityId id, const QVariantMap& properties)
{
    Shape* shp = static_cast<Shape*>(PooledEntitySystem::createComponent(id, properties));
    shp->_renderer = _renderer;
    shp->buildShape();
    return shp;
}

QVariantMap ShapeSystem::propertyValues(QtEntity::EntityId eid)
{
    QVariantMap m;
    Shape* s;
    if(component(eid, s))
    {
        m["position"] = s->position();
        m["path"] = s->path();
        m["zIndex"] = s->zIndex();
        m["subTex"] = s->subTex();
    }
    return m;
    
}


QVariantMap ShapeSystem::propertyAttributes()
{
    QVariantMap path;
    path["filter"] = "SVG files (*.svg)";  
    QVariantMap r;
    r["path"] = path;
    return r;
}


void ShapeSystem::applyPropertyValues(QtEntity::EntityId eid, const QVariantMap& m)
{
    Shape* s;
    if(component(eid, s))
    {
        if(m.contains("position")) s->setPosition(m["position"].toPoint());
        if(m.contains("path")) s->setPath(m["path"].toString());
        if(m.contains("zIndex")) s->setZIndex(m["zIndex"].toInt());
        if(m.contains("subTex")) s->setSubtex(m["subTex"].toRect());
    }
}
