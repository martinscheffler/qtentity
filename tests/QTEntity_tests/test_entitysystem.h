#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QtGui/QVector3D>
#include <QTEntity/EntityManager>

using namespace qte;

class Transform : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE Transform() {}
};


class TransformSystem : public qte::EntitySystem
{
    Q_OBJECT
public:
    TransformSystem() : qte::EntitySystem(Transform::staticMetaObject) {}
};


class EntitySystemTest: public QObject
{
    Q_OBJECT
private slots:

    void createAndFetch()
    {
        TransformSystem ts;
        QObject* c = ts.createComponent(1);
        QObject* c2 = ts.getComponent(1);
        QVERIFY(c == c2);
        QVERIFY(ts.hasComponent(1));

        QObject* c3 = ts.getComponent(2);
        QVERIFY(c3 == nullptr);
    }

    void destruct()
    {
        TransformSystem ts;
        QObject* c = ts.createComponent(1);
        QObject* c2 = ts.getComponent(1);
        QVERIFY(c == c2);
        QVERIFY(ts.hasComponent(1));
        ts.destructComponent(1);
        QVERIFY(!ts.hasComponent(1));
        QObject* c3 = ts.getComponent(1);
        QVERIFY(c3 == nullptr);
    }

};
