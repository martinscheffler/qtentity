#include <QtTest/QtTest>
#include <QtCore/QObject>

#include <QTEntity/EntityManager>

using namespace qte;

/*class MocEntitySystem : public EntitySystem
{
    Q_OBJECT
public:
    MocEntitySystem();
    virtual Component* get(EntityId) const { return NULL;}
    virtual Component* create(EntityId) { return NULL; }
    virtual bool destroy(EntityId) { return false; }
};*/

class EntityManagerTest: public QObject
{
    Q_OBJECT
private slots:

    /*void getEntitySystem()
    {
        EntityManager em;
        MocEntitySystem es;
        em.addEntitySystem(&es);
        MocEntitySystem* es2;
        em.getES(es2);
        QVERIFY(&es == es2);
    }*/

    void createEntity()
    {
        EntityManager em;
        EntityId eid = em.createEntity();
        QVERIFY(1 == eid);
        EntityId eid2 = em.createEntity();
        QVERIFY(2 == eid2);
    }


};
