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

    QVariantMap entry1;
    entry1["prototype"] = "Box";
    QVariantMap v;
    v["Color"] = QColor(0,255,0);
    v["Test0.1"] = 0.1;
    v["Test0.5"] = 0.5;
    entry1["value"] = v;
    testList.push_back(entry1);
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

QVariantMap ShapeSystem::toVariantMap(QtEntity::EntityId eid, int)
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


QVariantMap ShapeSystem::editingAttributes() const
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
    box["Test0.1"] = 3.1;
    box["Test0.5"] = 3.5;

    QVariantMap prototypes;
    prototypes["Box"] = box;
    prototypes["Sphere"] = sphere;

    QVariantMap attribs;
    attribs["prototypes"] = prototypes;

    QVariantMap test01;
    test01["singleStep"] = 0.1;
    QVariantMap test05;
    test05["singleStep"] = 0.5;

    QVariantMap boxattrs;
    boxattrs["Test0.1"] = test01;
    boxattrs["Test0.5"] = test05;
    attribs["Box"] = boxattrs;

    r["testList"] = attribs;

    return r;
}


void ShapeSystem::fromVariantMap(QtEntity::EntityId eid, const QVariantMap& m, int)
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
