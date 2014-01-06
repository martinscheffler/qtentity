#include "ShapeSystem"


IMPLEMENT_COMPONENT_TYPE(Shape)


Shape::Shape()
    : _zindex(0)
    , _rotation(0)
    , _handle(nullptr)
{
}


ShapeSystem::ShapeSystem(QtEntity::EntityManager* em, Renderer* renderer)
    : BaseClass(em)
    , _renderer(renderer)
{
}


QtEntity::Component* ShapeSystem::createComponent(QtEntity::EntityId id, const QVariantMap& properties)
{
    Shape* shape = static_cast<Shape*>(PooledEntitySystem::createComponent(id, properties));
    Q_ASSERT(_renderer);
    _renderer->addShape(shape);
    return shape;
}


bool ShapeSystem::destroyComponent(QtEntity::EntityId id)
{
    Shape* shape;
    if(component(id, shape))
    {
        _renderer->removeShape(shape);
    }
    return PooledEntitySystem::destroyComponent(id);
}


QVariantMap ShapeSystem::toVariantMap(QtEntity::EntityId eid, int)
{
    QVariantMap m;
    Shape* s;
    if(component(eid, s))
    {
        m["position"] = s->_position;
        m["path"]     = QVariant::fromValue(s->_path);
        m["zIndex"]   = s->_zindex;
        m["subTex"]   = s->_subtex;
    }
    return m;
    
}


void ShapeSystem::fromVariantMap(QtEntity::EntityId eid, const QVariantMap& m, int)
{
    Shape* s;
    if(component(eid, s))
    {
        if(m.contains("path"))
        {
            QString p = m["path"].toString();
            if(s->_path != p)
            {
                setPath(eid, p);
            }
        }
        if(m.contains("position")) s->_position = m["position"].toPoint();
        if(m.contains("zIndex"))   s->_zindex = m["zIndex"].toInt();
        if(m.contains("subTex"))   s->_subtex = m["subTex"].toRect();
        _renderer->updateShape(s);
    }
}


QVariantMap ShapeSystem::editingAttributes(int) const
{

    QVariantMap path;
    path["filter"] = "SVG files (*.svg)";
    QVariantMap r;
    r["path"] = path;
    return r;
}


QPoint ShapeSystem::position(QtEntity::EntityId eid) const
{
    Shape* s; if(!component(eid, s)) { return QPoint(); }
    return s->_position;
}


void ShapeSystem::setPosition(QtEntity::EntityId eid, const QPoint& p)
{
    Shape* s; if(!component(eid, s)) { return; }
    s->_position = p;
    _renderer->updateShape(s);
}


int ShapeSystem::zIndex(QtEntity::EntityId eid) const
{
    Shape* s; if(!component(eid, s)) { return 0; }
    return s->_zindex;
}


void ShapeSystem::setZIndex(QtEntity::EntityId eid, int i)
{
    Shape* s; if(!component(eid, s)) { return; }
    s->_zindex = i;
    _renderer->updateShape(s);
}


int ShapeSystem::rotation(QtEntity::EntityId eid) const
{
    Shape* s; if(!component(eid, s)) { return 0; }
    return s->_rotation;
}


void ShapeSystem::setRotation(QtEntity::EntityId eid, int i)
{
    Shape* s; if(!component(eid, s)) { return; }
    s->_rotation = i;
    _renderer->updateShape(s);
}


QRect ShapeSystem::subTex(QtEntity::EntityId eid) const
{
    Shape* s; if(!component(eid, s)) { return QRect(); }
    return s->_subtex;
}


void ShapeSystem::setSubtex(QtEntity::EntityId eid, const QRect& v)
{
    Shape* s; if(!component(eid, s)) { return; }
    s->_subtex = v;
    _renderer->updateShape(s);
}


QtEntityUtils::FilePath ShapeSystem::path(QtEntity::EntityId eid) const
{
    Shape* s; if(!component(eid, s)) { return QtEntityUtils::FilePath(); }
    return s->_path;
}


void ShapeSystem::setPath(QtEntity::EntityId eid, const QtEntityUtils::FilePath& path)
{
    Shape* s; if(!component(eid, s)) { return; }
    if(s->_handle)
    {
        _renderer->removeShape(s);
        s->_path = path;
        _renderer->addShape(s);
    }
    else
    {
        s->_path = path;
    }
}

