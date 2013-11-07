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
        QTE_ADD_PROPERTY("myint", int, Testing, myInt, setMyInt);        
        QTE_ADD_PROPERTY("myvec2", Vec2d, Testing, myVec2, setMyVec2);
        QTE_ADD_PROPERTY("myvec3", Vec3d, Testing, myVec3, setMyVec3);
        QTE_ADD_PROPERTY("myvec4", Vec4d, Testing, myVec4, setMyVec4);
        QTE_ADD_PROPERTY("mycolor", QColor, Testing, myColor, setMyColor);
        QTE_ADD_PROPERTY("myobjects", QVariantList, Testing, myObjects, setMyObjects);

        QtEntity::registerMetaTypes();
    }
};


class EntitySystemPrototype : public QObject, public QScriptable
{
    Q_OBJECT
public:
    EntitySystemPrototype(QObject *parent = 0);


    Q_INVOKABLE bool createComponent(QtEntity::EntityId id, const QVariantMap& params);
    Q_INVOKABLE quint32 count() const;
    Q_INVOKABLE QtEntity::PropertyAccessor property(const QString& name) const;

};