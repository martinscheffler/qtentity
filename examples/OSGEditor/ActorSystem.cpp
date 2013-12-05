#include "ActorSystem"

#include <osg/Geode>
#include <QStringList>

IMPLEMENT_COMPONENT_TYPE(Actor);

osg::Vec3 toVec(const QVariant& m)
{
    QVariantMap map = m.toMap();
    return osg::Vec3d(map["x"].toDouble(), map["y"].toDouble(), map["z"].toDouble());
}

QVariantMap toVariantMap(const osg::Vec3& v)
{
    QVariantMap m;
    m["x"] = v[0];
    m["y"] = v[1];
    m["z"] = v[2];
    return m;
}

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


void Actor::setPosition(const osg::Vec3d& p)
{
    _transform->setPosition(p);
}


osg::Vec3d Actor::position() const
{
    return _transform->getPosition();
}


void Actor::setShapes(const QVariantList& shapes)
{
    _shapes = shapes;
    _geode->removeDrawables(0, _geode->getNumDrawables());

    for(auto i = shapes.begin(); i != shapes.end(); ++i)
    {
        QVariantMap entry = i->toMap();
        if(!entry.contains("prototype"))
        {
            qDebug() << "Shape contains no prototype name!?!";
            continue;
        }
        QString prototype = entry["prototype"].toString();
        QVariantMap val = entry["value"].toMap();

        osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable();
        
        using namespace QtEntity;

        if(prototype == "Box")
        {
            osg::Vec3 hl = toVec(val["HalfLengths"]);
            osg::Vec3 c = toVec(val["Center"]);
            sd->setShape(new osg::Box(c, hl[0],hl[1],hl[2]));
            
        }
        else if(prototype == "Sphere")
        {
            osg::Vec3 c = toVec(val["Center"]);
            float radius = val["Radius"].toFloat();
            sd->setShape(new osg::Sphere(c, radius));
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


QVariantMap ActorSystem::properties(QtEntity::EntityId eid)
{
    QVariantMap m;
    Actor* a;
    if(component(eid, a))
    {
        m["name"]     = a->name();
        m["position"] = toVariantMap(a->position());
        m["shapes"]   = a->shapes();
    }
    return m;    
}


void ActorSystem::setProperties(QtEntity::EntityId eid, const QVariantMap& m)
{
    Actor* a;
    if(component(eid, a))
    {
        if(m.contains("name"))     a->setName(m["name"].toString());
        if(m.contains("position")) a->setPosition(toVec(m["position"]));
        if(m.contains("shapes"))   a->setShapes(m["shapes"].toList());
    }
}


QVariantMap ActorSystem::propertyAttributes() const
{

    QVariantMap sphere;
    sphere["Radius"] = 1.0f;
    sphere["Color"] = QColor(255,0,0,255);
    sphere["Center"] = toVariantMap(osg::Vec3(0,0,0));

    QVariantMap box;
    box["HalfLengths"] = toVariantMap(osg::Vec3(0.5f,0.5f,0.5f));
    box["Color"] = QColor(255,0,0,255);
    box["Center"] = toVariantMap(osg::Vec3(0,0,0));

    QVariantMap prototypes;
    prototypes["Box"] = box;
    prototypes["Sphere"] = sphere;
    
    QVariantMap attribs;
    attribs["prototypes"] = prototypes;

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



