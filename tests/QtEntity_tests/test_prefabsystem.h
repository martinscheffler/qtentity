#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QtEntity/PrefabSystem>
#include <QtEntity/EntityManager>

using namespace QtEntity;

class MyComponent : public QObject
{
    Q_OBJECT

public:

    Q_INVOKABLE MyComponent() {}
    Q_PROPERTY(int myint READ myInt WRITE setMyInt USER true)
    void setMyInt(qint32 v) { _myint = v; }
    qint32 myInt() const  { return _myint; }
    qint32 _myint;
};


class PrefabSystemTest: public QObject
{
    Q_OBJECT

private slots:

    void create()
    {
        EntityManager em;
        PrefabSystem* ps = new PrefabSystem();
        em.addEntitySystem(ps);

        EntitySystem* es = new EntitySystem(MyComponent::staticMetaObject);
        em.addEntitySystem(es);

        QVariantMap mycomponent;
        mycomponent["myint"] = 12345;

        QVariantMap components;
        QString cn = MyComponent::staticMetaObject.className();
        components[cn] = mycomponent;
        ps->addPrefab("bla.prefab", components);

        PrefabInstance* instance;

        QVariantMap props;
        props["path"] = "bla.prefab";
        em.createComponent(1, instance, props);


        auto test = em.getComponent<MyComponent>(1);
        QVERIFY(test != nullptr);
        QCOMPARE(12345, test->myInt());

    }



};
