#include "ActorSystem"

#include <osg/Geode>
#include <QStringList>

IMPLEMENT_COMPONENT_TYPE(Actor);

Actor::Actor()
    : _id(0)
    , _geode(new osg::Geode())
    , _transform(new osg::PositionAttitudeTransform())
    , _system(nullptr)
{
    _transform->addChild(_geode);    
}


void Actor::setName(const QString& name)
{
    if(_name != name)
    {
        _name = name;
        if(_system) _system->entityChanged(_id, _name);
    }
}


void Actor::setPosition(const QVector3D& p)
{
    _transform->setPosition(osg::Vec3d(p.x(), p.y(), p.z()));
}


QVector3D Actor::position() const
{
    const osg::Vec3d& p = _transform->getPosition();
    return QVector3D(p[0], p[1], p[2]);
}


void Actor::setShapes(const QVariantList& shapes)
{
    _shapes = shapes;
    _geode->removeDrawables(0, _geode->getNumDrawables());

    for(auto i = shapes.begin(); i != shapes.end(); ++i)
    {
        QVariantMap val = i->toMap();
        if(!val.contains("classname"))
        {
            qDebug() << "Shape contains no classname!?!";
            continue;
        }
        QString classname = val["classname"].toString();

        osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable();
        
        if(classname == "Box")
        {
            QVector3D hl = val["HalfLengths"].value<QVector3D>();
            QVector3D c = val["Center"].value<QVector3D>();
            sd->setShape(new osg::Box(osg::Vec3(c.x(), c.y(), c.z()), hl.x(), hl.y(), hl.z()));
            
        }
        else if(classname == "Sphere")
        {
            QVector3D c = val["Center"].value<QVector3D>();
            float radius = val["Radius"].toFloat();
            sd->setShape(new osg::Sphere(osg::Vec3(c.x(), c.y(), c.z()), radius));
        }
        QColor co = val["Color"].value<QColor>();
        sd->setColor(osg::Vec4(co.redF(), co.greenF(), co.blueF(), co.alphaF()));
        _geode->addDrawable(sd);

    }
}


ActorSystem::ActorSystem(QtEntity::EntityManager* em, osg::Group* rootnode)
    : BaseClass(em)
    , _rootNode(rootnode)
{   
}


QVariantMap ActorSystem::propertyValues(QtEntity::EntityId eid)
{
    QVariantMap m;
    Actor* a;
    if(component(eid, a))
    {
        m["name"]     = a->name();
        m["position"] = a->position();
        m["shapes"]   = a->shapes();
    }
    return m;    
}


void ActorSystem::applyPropertyValues(QtEntity::EntityId eid, const QVariantMap& m)
{
    Actor* a;
    if(component(eid, a))
    {
        if(m.contains("name"))     a->setName(m["name"].toString());
        if(m.contains("position")) a->setPosition(m["position"].value<QVector3D>());
        if(m.contains("shapes"))   a->setShapes(m["shapes"].toList());
    }
}


QVariantMap ActorSystem::propertyAttributes()
{
    QVariantMap box, sphere, radius, center, halflengths, color;
    radius["__value__"] = 1.0f;
    radius["__type__"] =  qMetaTypeId<float>();
    sphere["Radius"] = radius;

    color["__value__"] = QColor(255,0,0,255);
    color["__type__"] =  qMetaTypeId<QColor>();
    sphere["Color"] = color;

    center["__value__"] = QVector3D(0,0,0);
    center["__type__"] =  qMetaTypeId<QVector3D>();
    sphere["Center"] = center;

    halflengths["__value__"] = QVector3D(0.5f,0.5f,0.5f);
    halflengths["__type__"] =  qMetaTypeId<QVector3D>();
    box["HalfLengths"] = halflengths;

    box["Center"] = center;
    box["Color"] = color;

    QVariantMap classes;
    classes["Box"] = box;
    classes["Sphere"] = sphere;
    
    QVariantMap attribs;
    attribs["classes"] = classes;

    QVariantMap shapes;
    shapes["shapes"] = attribs;
    return shapes;
}


QtEntity::Component* ActorSystem::createComponent(QtEntity::EntityId id, const QVariantMap& propertyVals)
{
    QtEntity::Component* obj = BaseClass::createComponent(id, propertyVals);
    if(obj != NULL)
    {
        Actor* entry = static_cast<Actor*>(obj);
        entry->_id = id;
        entry->_system = this;
        _rootNode->addChild(entry->_transform);
        if(this->component(id, entry))
        {
            emit entityAdded(id, entry->name());
        }
    }
    return obj;
}


bool ActorSystem::destroyComponent(QtEntity::EntityId id)
{
    Actor* entry;
    if(this->component(id, entry))
    {
        emit entityRemoved(id);
    }

    return BaseClass::destroyComponent(id);
}



