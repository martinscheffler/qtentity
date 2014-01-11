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
public:

    Testing() {}

    void setMyInt(qint32 v) { _myint = v; }
    qint32 myInt() const  { return _myint; }

    void setMyColor(const QColor& v) { _mycolor = v; }
    QColor myColor() const  { return _mycolor; }

    void setMyObjects(const QVariantList& v) { _myobjects = v; }
    QVariantList myObjects() const  { return _myobjects; }

    QColor _mycolor;
    qint32 _myint;
    QVariantList _myobjects;

};

Q_DECLARE_METATYPE(Testing)

class TestingSystem : public SimpleEntitySystem<Testing>
{     
public:
    TestingSystem(QtEntity::EntityManager* em)
        : SimpleEntitySystem<Testing>(em)
    {
        QtEntity::registerMetaTypes();
    }

    virtual QVariantMap toVariantMap(QtEntity::EntityId eid,
                                     int context = 0) override
    {
        Q_UNUSED(context)
        QVariantMap m;
        Testing* t;
        if(component(eid, t))
        {
            m["myint"]     = t->myInt();
            m["mycolor"]   = t->myColor();
            m["myobjects"] = t->myObjects();
        }
        return m;    
    }

    virtual void fromVariantMap(QtEntity::EntityId eid,
                                const QVariantMap& m,
                                int context = 0) override
    {
        Q_UNUSED(context)
        Testing* t;
        if(component(eid, t))
        {
            if(m.contains("myint"))     t->setMyInt(m["myint"].toInt());
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
