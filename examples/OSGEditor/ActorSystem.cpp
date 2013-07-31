#include "ActorSystem"
#include <osg/Geode>
#include <QStringList>
#include <QtEntity/MetaObjectRegistry>

Actor::Actor(QtEntity::EntityId id, osg::Group* parent, const QString& name, const QVector3D& pos, ActorSystem* as)
    : _id(id)
    , _name(name)
    , _geode(new osg::Geode())
    , _transform(new osg::PositionAttitudeTransform())
    , _system(as)
{
    parent->addChild(_transform);
    _transform->addChild(_geode);
    setPosition(pos);
}


void Actor::setName(const QString& name)
{
    if(_name != name)
    {
        _name = name;
        _system->entityChanged(_id, _name);
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
    : SimpleEntitySystem(Actor::staticMetaObject)
    , _rootNode(rootnode)
{
    QtEntity::registerMetaObject(Box::staticMetaObject);
    QtEntity::registerMetaObject(Sphere::staticMetaObject);
}


QObject* ActorSystem::createObjectInstance(QtEntity::EntityId id, const QVariantMap& propertyVals)
{

    QVector3D pos = propertyVals["position"].value<QVector3D>();
    QString name = propertyVals["name"].toString();
    return new Actor(id, _rootNode, name, pos, this);

}


QObject* ActorSystem::createComponent(QtEntity::EntityId id, const QVariantMap& propertyVals)
{
    QObject* obj = SimpleEntitySystem::createComponent(id, propertyVals);
    if(obj != NULL)
    {
        Actor* entry;
        if(this->getComponent(id, entry))
        {
            emit entityAdded(id, entry->name());
        }
    }
    return obj;
}


bool ActorSystem::destroyComponent(QtEntity::EntityId id)
{
    Actor* entry;
    if(this->getComponent(id, entry))
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
