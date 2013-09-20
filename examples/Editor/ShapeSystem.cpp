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


ShapeSystem::ShapeSystem(QtEntity::EntityManager* em, Renderer* renderer)
    : BaseClass(em)
    , _renderer(renderer)
{
    QTE_ADD_PROPERTY("position", QPoint, Shape, position, setPosition);

    QVariantMap r;
    r["filter"] = "SVG files (*.svg)";    

    QTE_ADD_PROPERTY_WITH_ATTRIBS("path", QString, Shape, path, setPath, r);
    QTE_ADD_PROPERTY("zIndex", int, Shape, zIndex, setZIndex);
    QTE_ADD_PROPERTY("subTex", QRect, Shape, subTex, setSubtex);
}


QtEntity::Component* ShapeSystem::createComponent(QtEntity::EntityId id, const QVariantMap& properties)
{
    Shape* shp = static_cast<Shape*>(PooledEntitySystem::createComponent(id, properties));
    shp->_renderer = _renderer;
    shp->buildShape();
    return shp;
}

