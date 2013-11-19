#pragma once

#include <QtEntity/EntityManager>
#include <QtEntity/SimpleEntitySystem>
#include <QtEntity/DataTypes>
#include <QtCore/QObject>
#include <QtGui/QColor>
#include <QScriptable>

using namespace QtEntity;


class TestObj1 : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int propint READ propInt WRITE setPropInt USER true)
    qint32 _propint;
public:

     Q_INVOKABLE TestObj1() {}

    void setPropInt(qint32 v) { _propint = v; }
    qint32 propInt() const  { return _propint; }
    
};


class TestObj2 : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE TestObj2() {}
};

class Testing : public QtEntity::Component
{
    DECLARE_COMPONENT_TYPE(NOEXP)        

public:

    Testing() {}

    void setMyVec2(const Vec2d& v) { _myvec2 = v; }
    Vec2d myVec2() const  { return _myvec2; }

    void setMyInt(qint32 v) { _myint = v; }
    qint32 myInt() const  { return _myint; }

    void setMyVec3(const Vec3d& v) { _myvec3 = v; }
    Vec3d myVec3() const  { return _myvec3; }

    void setMyVec4(const Vec4d& v) { _myvec4 = v; }
    Vec4d myVec4() const  { return _myvec4; }

    void setMyColor(const QColor& v) { _mycolor = v; }
    QColor myColor() const  { return _mycolor; }

    void setMyObjects(const QVariantList& v) { _myobjects = v; }
    QVariantList myObjects() const  { return _myobjects; }

    Vec2d _myvec2;
    Vec3d _myvec3;
    Vec4d _myvec4;
    QColor _mycolor;
    qint32 _myint;
    QVariantList _myobjects;

};



class TestingSystem : public SimpleEntitySystem<Testing>
{     
public:
    TestingSystem(QtEntity::EntityManager* em)
        : SimpleEntitySystem<Testing>(em)
    {
        QtEntity::registerMetaTypes();
    }

    virtual QVariantMap properties(QtEntity::EntityId eid) override
    {
        QVariantMap m;
        Testing* t;
        if(component(eid, t))
        {
            m["myint"]     = t->myInt();
            m["myvec2"]    = QVariant::fromValue(t->myVec2());
            m["myvec3"]    = QVariant::fromValue(t->myVec3());
            m["myvec4"]    = QVariant::fromValue(t->myVec4());
            m["mycolor"]   = t->myColor();
            m["myobjects"] = t->myObjects();
        }
        return m;    
    }

    virtual void setProperties(QtEntity::EntityId eid, const QVariantMap& m) override
    {
        Testing* t;
        if(component(eid, t))
        {
            if(m.contains("myint"))     t->setMyInt(m["myint"].toInt());
            if(m.contains("myvec2"))    t->setMyVec2(m["myvec2"].value<QtEntity::Vec2d>());
            if(m.contains("myvec3"))    t->setMyVec3(m["myvec3"].value<QtEntity::Vec3d>());
            if(m.contains("myvec4"))    t->setMyVec4(m["myvec4"].value<QtEntity::Vec4d>());
            if(m.contains("mycolor"))   t->setMyColor(m["mycolor"].value<QColor>());
            if(m.contains("myobjects")) t->setMyObjects(m["myobjects"].toList());
        }
    }
    
};


class EntitySystemPrototype : public QObject, public QScriptable
{
    Q_OBJECT
public:
    EntitySystemPrototype(QObject *parent = 0);
    
    Q_INVOKABLE bool createComponent(QtEntity::EntityId id, const QVariantMap& params);
    Q_INVOKABLE bool destroyComponent(QtEntity::EntityId id);
    Q_INVOKABLE quint32 count() const;
};