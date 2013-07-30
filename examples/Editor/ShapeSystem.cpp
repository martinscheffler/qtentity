#include "ShapeSystem"


Shape::Shape(Renderer* renderer, const QPoint& pos, const QtEntityUtils::FilePath& path, int zIndex, const QRect& subtex)
    : _renderer(renderer)
    , _path(path)
    , _position(pos)
    , _subtex(subtex)
    , _zindex(zIndex)
    , _handle(0)    
{
    buildShape();
}


void Shape::buildShape()
{
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
    _renderer->updateShape(_handle, _position, _zindex);
}


void Shape::setZIndex(int i)
{
    _zindex = i;
    _renderer->updateShape(_handle, _position, _zindex);
}


ShapeSystem::ShapeSystem(Renderer* renderer)
    : SimpleEntitySystem(Shape::staticMetaObject)
    , _renderer(renderer)
{

}


QObject* ShapeSystem::createObjectInstance(QtEntity::EntityId id, const QVariantMap& propertyVals)
{

    QPoint pos = propertyVals["position"].value<QPoint>();
    QtEntityUtils::FilePath path = propertyVals["path"].value<QtEntityUtils::FilePath>();
    int zindex = propertyVals["zIndex"].toInt();
    QRect rect = propertyVals["subtex"].value<QRect>();
    return new Shape(_renderer, pos, path, zindex, rect);

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
