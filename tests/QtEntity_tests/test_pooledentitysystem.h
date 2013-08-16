#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QtGui/QVector2D>
#include <QtEntity/EntityManager>
#include <QtEntity/SimpleEntitySystem>
#include <QtEntity/PooledEntitySystem>
#include <QElapsedTimer>

using namespace QtEntity;

class XTransform : public QObject
{
    Q_OBJECT

public:

    Q_INVOKABLE XTransform()
    : _myint(123)
    , _myvec2(QVector2D(-45, 45))
    {
    }

    Q_PROPERTY(int myint READ myInt WRITE setMyInt USER true)
    Q_PROPERTY(QVector2D myvec2 READ myVec2 WRITE setMyVec2 USER true)

    void setMyVec2(const QVector2D& v) { _myvec2 = v; }
    QVector2D myVec2() const  { return _myvec2; }

    void setMyInt(qint32 v) { _myint = v; }
    qint32 myInt() const 
    {
        return _myint; 
    }

    qint32 _myint;
    QVector2D _myvec2;


};


class PooledEntitySystemTest: public QObject
{
    Q_OBJECT


#define NUM_COMPONENTS 500
#define NUM_ITERATIONS 10000

private:
    template <typename T>
    int speed1(T& es)
    {
        QElapsedTimer timer;
        timer.start();
        QtEntity::EntityId id = 1;
        for(int i = 1; i <= NUM_COMPONENTS; ++i)
        {
            es.createComponent(id++);
        }

        for(int i = 1; i <= NUM_COMPONENTS; i += 4)
        {
            es.destroyComponent(i);
        }

        for(int i = 1; i <= NUM_COMPONENTS; ++i)
        {
            es.createComponent(id++);
        }

        for(int i = 0; i <= NUM_ITERATIONS; ++i)
        {
            auto end = es.end();
            for(auto j = es.begin(); j != end; ++j)
            {
                QObject* c = *j;
                XTransform* t = qobject_cast<XTransform*>(c);
                t->setMyInt(t->myInt() + 1);
            }
        }

        return timer.elapsed();
    }

    int speed2(EntitySystem& es)
    {

        QElapsedTimer timer;
        timer.start();

        QtEntity::EntityId id = 1;
        for(int i = 1; i <= NUM_COMPONENTS; ++i)
        {
            es.createComponent(id++);
        }

        for(int i = 1; i <= NUM_COMPONENTS; i += 4)
        {
            es.destroyComponent(i);
        }

        for(int i = 1; i <= NUM_COMPONENTS; ++i)
        {
            es.createComponent(id++);
        }

        for(int i = 0; i <= NUM_ITERATIONS; ++i)
        {
            auto end = es.pend();
            for(auto j = es.pbegin(); j != end; ++j)
            {
                QObject* c = *j;
                XTransform* t = qobject_cast<XTransform*>(c);
                t->setMyInt(t->myInt() + 1);
            }
        }

        return timer.elapsed();
    }

private slots:

    void createAndFetch()
    {
        PooledEntitySystem<XTransform> ts;

		QVariantMap m;
		m["myint"] = 666;
		m["myvec2"] =  QVector2D(77.0,88.0);
        QObject* c = ts.createComponent(1, m);

        QObject* c2 = ts.component(1);
        QVERIFY(c == c2);
        QVERIFY(ts.component(1) != nullptr);

        QObject* c3 = ts.component(2);
        QVERIFY(c3 == nullptr);

        XTransform* tr = qobject_cast<XTransform*>(c);
        QVERIFY(tr != nullptr);
        QCOMPARE(tr->myInt(), 666);
        QCOMPARE(tr->myVec2().x(), 77.0);
        QCOMPARE(tr->myVec2().y(), 88.0);
     

    }

    void reserve()
    {
        PooledEntitySystem<XTransform> ts(0, 2);
		for(int i = 0; i < 3; ++i) ts.createComponent(i + 1);
        QCOMPARE(ts.size(), (size_t)3);
        QCOMPARE(ts.capacity(), (size_t)4);
    }

    void destroyOne()
    {
        PooledEntitySystem<XTransform> ts(0, 2);
		ts.createComponent(1);
        ts.destroyComponent(1);
        QCOMPARE(ts.size(), (size_t)0);
        QCOMPARE(ts.capacity(), (size_t)2);
    }


     void destroyMore()
    {
        PooledEntitySystem<XTransform> ts(0, 2);
        QVariantMap m;
		m["myint"] = 1;		
        ts.createComponent(1, m);
        m["myint"] = 2;
        ts.createComponent(2, m);
        m["myint"] = 3;
        ts.createComponent(3, m);
        
        QCOMPARE(ts.size(), (size_t)3);
        ts.destroyComponent(2);
        QCOMPARE(ts.size(), (size_t)2);

        QCOMPARE(static_cast<XTransform*>(ts.component(1))->myInt(), 1);
        QCOMPARE(static_cast<XTransform*>(ts.component(3))->myInt(), 3);
        
    }

    
    void speedTest()
    {
        {
            PooledEntitySystem<XTransform> pooled(0, 8);
            SimpleEntitySystem simple(XTransform::staticMetaObject);

            float timepooled = float(speed1(pooled)) / 1000.0f;
            float timesimple = float(speed1(simple)) / 1000.0f;
            qDebug() << "Time pooled1: " << timepooled;
            qDebug() << "Time simple1: " << timesimple;
        }

        {
            PooledEntitySystem<XTransform> pooled(0, 8);
            SimpleEntitySystem simple(XTransform::staticMetaObject);

            float timepooled = float(speed2(pooled)) / 1000.0f;
            float timesimple = float(speed2(simple)) / 1000.0f;
            qDebug() << "Time pooled1: " << timepooled;
             qDebug() << "Time simple1: " << timesimple;
        }
    }

    void iteratorTest1()
    {
        PooledEntitySystem<XTransform> pooled(0, 8);
        QVERIFY(pooled.begin() == pooled.end());

        QVariantMap m;
		m["myint"] = 1; pooled.createComponent(1, m);
        m["myint"] = 2; pooled.createComponent(2, m);
        m["myint"] = 3; pooled.createComponent(3, m);
        PooledEntitySystem<XTransform>::Iterator i = pooled.begin();
        XTransform* t = *i;
        QCOMPARE(t, static_cast<XTransform*>(pooled.component(1)));
        QCOMPARE(*++i, static_cast<XTransform*>(pooled.component(2)));
        QCOMPARE(*++i, static_cast<XTransform*>(pooled.component(3)));
        PooledEntitySystem<XTransform>::Iterator j = ++i;
        PooledEntitySystem<XTransform>::Iterator end = pooled.end();
        QCOMPARE(j, end);
    }

    void iteratorTest2()
    {
        PooledEntitySystem<XTransform> pooled(0, 8);
        QVariantMap m;
        for(int i = 1; i <= 10; ++i)
        {
            m["myint"] = i;
            pooled.createComponent(i, m);
        }
        int count = 1;
        for(PooledEntitySystem<XTransform>::Iterator i = pooled.begin(); i != pooled.end(); ++i)
        {
            XTransform* o = *i;
            QCOMPARE(o, static_cast<XTransform*>(pooled.component(count++)));
        }
    }

    void iteratorTest3()
    {
        PooledEntitySystem<XTransform> pooled(0, 8);
        QVariantMap m;
        for(int i = 1; i <= 10; ++i)
        {
            m["myint"] = i;
            pooled.createComponent(i, m);
        }
        int count = 1;
        for(EntitySystem::Iterator i = pooled.pbegin(); i != pooled.pend(); ++i)
        {
            XTransform* o = static_cast<XTransform*>(*i);
            QCOMPARE(o, static_cast<XTransform*>(pooled.component(count++)));
        }
    }
};
