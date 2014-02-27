#include "ActorSystem"

#include <osg/Geode>
#include <QStringList>
#include <QDebug>


osg::Vec3 toVec(const QVariant& m)
{
    QVariantMap map = m.toMap();
    return osg::Vec3d(map["x"].toDouble(), map["y"].toDouble(), map["z"].toDouble());
}

QVariantMap toVarMap(const osg::Vec3& v)
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


void Actor::setShapes(const QtEntityUtils::ItemList& shapes)
{
    _shapes = shapes;
    _geode->removeDrawables(0, _geode->getNumDrawables());

    for(auto i = shapes.begin(); i != shapes.end(); ++i)
    {
        QtEntityUtils::Item entry = *i;

        osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable();
        
        using namespace QtEntity;

        QVariantMap val = entry._value.toMap();

        if(entry._prototype == "Box")
        {
            osg::Vec3 hl = toVec(val["HalfLengths"]);
            osg::Vec3 c = toVec(val["Center"]);
            sd->setShape(new osg::Box(c, hl[0],hl[1],hl[2]));
            
        }
        else if(entry._prototype == "Sphere")
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


QVariantMap ActorSystem::toVariantMap(QtEntity::EntityId eid, int)
{
    QVariantMap m;
    Actor* a;
    if(component(eid, a))
    {
        m["name"]     = a->name();
        m["position"] = toVarMap(a->position());
        m["shapes"]   = QVariant::fromValue(a->shapes());
    }
    return m;    
}


void ActorSystem::fromVariantMap(QtEntity::EntityId eid, const QVariantMap& m, int)
{
    Actor* a;
    if(component(eid, a))
    {
        if(m.contains("name"))     a->setName(m["name"].toString());
        if(m.contains("position")) a->setPosition(toVec(m["position"]));
        if(m.contains("shapes"))   a->setShapes(m["shapes"].value<QtEntityUtils::ItemList>());
    }
}


QVariantMap ActorSystem::editingAttributes(int) const
{

    QVariantMap ret;
    {
        QVariantMap types;
        {
            QVariantMap sphere;
            {
                QVariantMap psphere;
                {
                    psphere["Radius"] = 0.5f;
                    psphere["Color"] = QColor(0,255,0,255);
                    psphere["Center"] = toVarMap(osg::Vec3(0.5f,0,0));
                }
                sphere["prototype"] = psphere;
            }
            types["Sphere"] = sphere;

            QVariantMap box;
            {
                QVariantMap pbox;
                {
                    pbox["HalfLengths"] = toVarMap(osg::Vec3(0.5f,0.5f,0.5f));
                    pbox["Color"] = QColor(255,0,0,255);
                    pbox["Center"] = toVarMap(osg::Vec3(-0.5f,0,0));
                }            
                box["prototype"] = pbox;
            }
            types["Box"] = box;
        }
        ret["__types"] = types;

        QVariantMap shapesattr;
        {
            QVariantList shapesproto;
            {
                shapesproto.push_back("Box");
                shapesproto.push_back("Sphere");
            }
            shapesattr["prototypes"] = shapesproto;
        }
        ret["shapes"] = shapesattr;
    }
    return ret;
}


void* ActorSystem::createComponent(QtEntity::EntityId id, const QVariantMap& propertyVals)
{
    void* obj = BaseClass::createComponent(id, propertyVals);
    if(obj != nullptr)
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



