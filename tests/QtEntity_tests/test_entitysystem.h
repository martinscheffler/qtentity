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

    Q_PROPERTY(int myint READ myInt WRITE setMyInt USER true)
    Q_PROPERTY(QVector2D myvec2 READ myVec2 WRITE setMyVec2 USER true)

    void setMyVec2(const QVector2D& v) { _myvec2 = v; }
    QVector2D myVec2() const  { return _myvec2; }

    void setMyInt(qint32 v) { _myint = v; }
    qint32 myInt() const  { return _myint; }

    QVector2D _myvec2;
    qint32 _myint;

};


class EntitySystemTest: public QObject
{
    Q_OBJECT
private slots:

    void createAndFetch()
    {
        SimpleEntitySystem ts(Transform::staticMetaObject);

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
        SimpleEntitySystem ts(Transform::staticMetaObject);
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
        SimpleEntitySystem ts(Transform::staticMetaObject);
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
