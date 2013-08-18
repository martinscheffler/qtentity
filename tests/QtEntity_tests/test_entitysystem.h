#include <QtTest/QtTest>
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
        QTE_ADD_PROPERTY("myint", &TransformSystem::myInt, &TransformSystem::setMyInt);
        QTE_ADD_PROPERTY("myvec2", &TransformSystem::myVec2, &TransformSystem::setMyVec2);
    }

    QVariant myInt(QtEntity::EntityId id) const { return static_cast<Transform*>(component(id))->myInt(); }
    void setMyInt(QtEntity::EntityId id, const QVariant& v) const { static_cast<Transform*>(component(id))->setMyInt(v.toInt()); }

    QVariant myVec2(QtEntity::EntityId id) const { return static_cast<Transform*>(component(id))->myVec2(); }
    void setMyVec2(QtEntity::EntityId id, const QVariant& v) const { static_cast<Transform*>(component(id))->setMyVec2(v.value<QVector2D>()); }
};


class EntitySystemTest: public QObject
{
    Q_OBJECT
private slots:

    void propertyReadWrite()
    {

    }

    void createAndFetch()
    {
        TransformSystem ts;

		QVariantMap m;
		m["myint"] = 666;
		m["myvec2"] =  QVector2D(77.0,88.0);
        QObject* c = ts.createComponent(1, m);

        QObject* c2 = ts.component(1);
        QVERIFY(c == c2);
        QVERIFY(ts.hasComponent(1));

        QObject* c3 = ts.component(2);
        QVERIFY(c3 == nullptr);

        Transform* tr = qobject_cast<Transform*>(c);
        QVERIFY(tr != nullptr);
        QCOMPARE(tr->myInt(), 666);
        QCOMPARE(tr->myVec2().x(), 77.0);
        QCOMPARE(tr->myVec2().y(), 88.0);
     

    }

    void destruct()
    {
        TransformSystem ts;
        QObject* c = ts.createComponent(1);
        QObject* c2 = ts.component(1);
        QVERIFY(c == c2);
        QVERIFY(ts.hasComponent(1));
        ts.destroyComponent(1);
        QVERIFY(!ts.hasComponent(1));
        QObject* c3 = ts.component(1);
        QVERIFY(c3 == nullptr);
    }

    void iteratorTest1()
    {
        SimpleEntitySystem ts(Transform::staticMetaObject);
        ts.createComponent(1);
        auto i = ts.pbegin();
        QObject* o = *i;
        QObject* o2 = ts.component(1);
        QCOMPARE(o, o2);

    }

    void iteratorTest2()
    {
        TransformSystem ts;
        QVariantMap m;
        for(int i = 1; i <= 5; ++i)
        {
            m["myint"] = i;
            ts.createComponent(i, m);
        }
        int sum = 0;
        auto end = ts.pend();
        for(auto i = ts.pbegin(); i != end; ++i)
        {
            QObject* o = *i;
            Transform* t = static_cast<Transform*>(o);
            sum += t->myInt();
        }

        QCOMPARE(sum, 15);
    }
};
