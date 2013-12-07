#include "EnemySystem"

#include "ShapeSystem"
#include <QtEntity/EntityManager>

IMPLEMENT_COMPONENT_TYPE(Enemy)

EnemySystem::EnemySystem(QtEntity::EntityManager* em)
   : BaseClass(em)
{
}


QVariantMap EnemySystem::toVariantMap(QtEntity::EntityId eid)
{
    QVariantMap m;
    Enemy* e;
    if(component(eid, e))
    {
        m["lifetime"] = e->lifetime();
    }
    return m;    
}


void EnemySystem::fromVariantMap(QtEntity::EntityId eid, const QVariantMap& m)
{
    Enemy* e;
    if(component(eid, e))
    {
        if(m.contains("lifetime")) e->setLifetime(m["lifetime"].toInt());
    }
}


void EnemySystem::step(int frameNumber, int totalTime, int delta)
{
    float t = float(totalTime) / 1000.0f;

    for(auto i = begin(); i != end(); ++i)
    {
        Enemy* enemy = static_cast<Enemy*>(i->second);
        float lifetime = float(enemy->lifetime()) / 1000.0f;
        enemy->setLifetime(enemy->lifetime() + delta);
        Shape* shape;
        bool found = entityManager()->component(i->first, shape);
        Q_ASSERT(found);
        int x = sin(t / 10.0f + lifetime) * 300 + 280;
        int y = float(lifetime * 3.0f) + cos(t / 20.0f) * 10 - 50;
        shape->setPosition(QPoint(x, y));
    }
}
