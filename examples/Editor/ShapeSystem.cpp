#include "ShapeSystem"


IMPLEMENT_COMPONENT_TYPE(Shape)


Shape::Shape()
    : _renderer(nullptr)
    , _handle(0)    
{
    test["x"] = 123;
    test["y"] = 456.0f;
    test["z"] = 789.0;
    test["bla"] = "Hallo Welt";
    testList.push_back(122.43);
    testList.push_back("Hallo Welt");
}


void Shape::buildShape()
{
    if(!_renderer) return;
    if(_handle != 0)
    {
        _renderer->destroyShape(_handle);
    }

    _handle = _renderer->createShape(_path, _position, _subtex, _zindex);
}


void Shape::setPosition(const QPoint& p)
{
    _position = p;
    if(_renderer) _renderer->updateShape(_handle, _position, _subtex, _zindex);
}


void Shape::setZIndex(int i)
{
    _zindex = i;
    if(_renderer) _renderer->updateShape(_handle, _position, _subtex, _zindex);
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

QVariantMap ShapeSystem::properties(QtEntity::EntityId eid)
{
    QVariantMap m;
    Shape* s;
    if(component(eid, s))
    {
        QVariantMap pos;
        pos["x"] = s->position().x();
        pos["y"] = s->position().y();
        m["position"] = pos;
        //m["position"] = s->position();
        m["path"]     = QVariant::fromValue(s->path());
        m["zIndex"]   = s->zIndex();
        m["subTex"]   = s->subTex();

        m["test"] = s->test;
        m["testList"] = s->testList;
    }
    return m;
    
}


QVariantMap ShapeSystem::propertyAttributes() const
{

    QVariantMap path;
    path["filter"] = "SVG files (*.svg)";  
    QVariantMap r;
    r["path"] = path;


    QVariantMap sphere;
    sphere["Radius"] = 1.0f;
    sphere["Color"] = QColor(255,0,0,255);

    QVariantMap box;
    box["Color"] = QColor(255,0,0,255);

    QVariantMap prototypes;
    prototypes["Box"] = box;
    prototypes["Sphere"] = sphere;

    QVariantMap attribs;
    attribs["prototypes"] = prototypes;

    r["testList"] = attribs;

    return r;
}


void ShapeSystem::setProperties(QtEntity::EntityId eid, const QVariantMap& m)
{
    Shape* s;
    if(component(eid, s))
    {
        if(m.contains("position")) s->setPosition(m["position"].toPoint());
        if(m.contains("path"))     s->setPath(m["path"].toString());
        if(m.contains("zIndex"))   s->setZIndex(m["zIndex"].toInt());
        if(m.contains("subTex"))   s->setSubtex(m["subTex"].toRect());
        if(m.contains("test"))     s->test = m["test"].toMap();
        if(m.contains("testList"))
        {
            s->testList = m["testList"].toList();
        }
        s->setSubtex(m["subTex"].toRect());
    }
}
