#pragma once

#include <QtCore/QObject>
#include <QtGui/QVector2D>
#include <QtEntity/EntityManager>
#include <QtEntity/SimpleEntitySystem>

using namespace QtEntity;

class Transform : public QObject
{
    Q_OBJECT

public:

    Q_INVOKABLE Transform() {}

    void setMyVec2(const QVector2D& v) { _myvec2 = v; }
    QVector2D myVec2() const  { return _myvec2; }

    void setMyInt(qint32 v) { _myint = v; }
    qint32 myInt() const  { return _myint; }

    QVector2D _myvec2;
    qint32 _myint;

};


class TransformSystem : public SimpleEntitySystem
{
public:
    TransformSystem()
     : SimpleEntitySystem(Transform::staticMetaObject)
    {
        QTE_ADD_PROPERTY("myint", int, Transform, myInt, setMyInt);
        QTE_ADD_PROPERTY("myvec2", QVector2D, Transform, myVec2, setMyVec2);
    }
};
