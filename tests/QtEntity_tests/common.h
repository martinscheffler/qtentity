#pragma once

#include <QtEntity/EntityManager>
#include <QtEntity/SimpleEntitySystem>
#include <QtEntity/MetaObjectRegistry>
#include <QtCore/QObject>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>
#include <QtGui/QColor>


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

    void setMyVec2(const QVector2D& v) { _myvec2 = v; }
    QVector2D myVec2() const  { return _myvec2; }

    void setMyInt(qint32 v) { _myint = v; }
    qint32 myInt() const  { return _myint; }

    void setMyVec3(const QVector3D& v) { _myvec3 = v; }
    QVector3D myVec3() const  { return _myvec3; }

    void setMyVec4(const QVector4D& v) { _myvec4 = v; }
    QVector4D myVec4() const  { return _myvec4; }

    void setMyColor(const QColor& v) { _mycolor = v; }
    QColor myColor() const  { return _mycolor; }

    void setMyObjects(const PropertyObjects& v) { _myobjects = v; }
    PropertyObjects myObjects() const  { return _myobjects; }

    QVector2D _myvec2;
    QVector3D _myvec3;
    QVector4D _myvec4;
    QColor _mycolor;
    qint32 _myint;
    PropertyObjects _myobjects;

};



class TestingSystem : public SimpleEntitySystem
{
    Q_OBJECT 
public:
    TestingSystem() : SimpleEntitySystem(Testing::classTypeId())
    {
        QTE_ADD_PROPERTY("myint", int, Testing, myInt, setMyInt);        
        QTE_ADD_PROPERTY("myvec2", QVector2D, Testing, myVec2, setMyVec2);
        QTE_ADD_PROPERTY("myvec3", QVector3D, Testing, myVec3, setMyVec3);
        QTE_ADD_PROPERTY("myvec4", QVector4D, Testing, myVec4, setMyVec4);
        QTE_ADD_PROPERTY("mycolor", QColor, Testing, myColor, setMyColor);
        QTE_ADD_PROPERTY("myobjects", QtEntity::PropertyObjects, Testing, myObjects, setMyObjects);

        QtEntity::registerMetaTypes();
        registerMetaObject(TestObj1::staticMetaObject);
        registerMetaObject(TestObj2::staticMetaObject);
    }
};
