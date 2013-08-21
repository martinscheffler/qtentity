#include "ActorSystem"
#include <osg/Geode>
#include <QStringList>
#include <QtEntity/MetaObjectRegistry>


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


void Actor::setShapes(const QtEntity::PropertyObjects& v)
{
    _shapes = v;
    _geode->removeDrawables(0, _geode->getNumDrawables());

    foreach(auto o, v)
    {
        ShapeDrawable* sd = static_cast<ShapeDrawable*>(o.data());
        _geode->addDrawable(sd->_shapeDrawable);
    }
}


ActorSystem::ActorSystem(osg::Group* rootnode)
    : SimpleEntitySystem(Actor::classTypeId())
    , _rootNode(rootnode)
{
    QtEntity::registerMetaObject(Box::staticMetaObject);
    QtEntity::registerMetaObject(Sphere::staticMetaObject);

    QTE_ADD_PROPERTY("name", QString, Actor, name, setName);
    QTE_ADD_PROPERTY("position", QVector3D, Actor, position, setPosition);
    QStringList sl;
    sl.push_back(Box::staticMetaObject.className());
    sl.push_back(Sphere::staticMetaObject.className());
    QVariantMap attribs;
    attribs["classnames"] = sl;
    QTE_ADD_PROPERTY_WITH_ATTRIBS("shapes", QtEntity::PropertyObjects, Actor, shapes, setShapes, attribs);
}


QtEntity::Component* ActorSystem::createComponent(QtEntity::EntityId id, const QVariantMap& propertyVals)
{
    QtEntity::Component* obj = SimpleEntitySystem::createComponent(id, propertyVals);
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

    return SimpleEntitySystem::destroyComponent(id);
}



const QVariantMap ActorSystem::attributesForProperty(const QString& name) const
{
    QVariantMap r;
    if(name == "shapes")
    {
        QStringList sl;
        sl.push_back(Box::staticMetaObject.className());
        sl.push_back(Sphere::staticMetaObject.className());
        r["classnames"] = sl;
    }
    return r;
}
