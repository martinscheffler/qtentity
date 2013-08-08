#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QtGui/QVector2D>
#include <QtEntity/EntityManager>
#include <QtEntity/PooledEntitySystem>

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

    QVector2D _myvec2;
    qint32 _myint;

};


class PooledEntitySystemTest: public QObject
{
    Q_OBJECT
private slots:

    void createAndFetch()
    {
        PooledEntitySystem<XTransform> ts;

		QVariantMap m;
		m["myint"] = 666;
		m["myvec2"] =  QVector2D(77.0,88.0);
        QObject* c = ts.createComponent(1, m);

        QObject* c2 = ts.getComponent(1);
        QVERIFY(c == c2);
        QVERIFY(ts.getComponent(1) != nullptr);

        QObject* c3 = ts.getComponent(2);
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
		QObject* c1 = ts.createComponent(1, m);
        m["myint"] = 2;
        QObject* c2 = ts.createComponent(2, m);
        m["myint"] = 3;
        QObject* c3 = ts.createComponent(3, m);
        
        QCOMPARE(ts.size(), (size_t)3);
        ts.destroyComponent(2);
        QCOMPARE(ts.size(), (size_t)2);

        QCOMPARE(static_cast<XTransform*>(ts.getComponent(1))->myInt(), 1);
        QCOMPARE(static_cast<XTransform*>(ts.getComponent(3))->myInt(), 3);
        
    }
};
