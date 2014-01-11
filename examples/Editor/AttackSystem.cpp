#include "AttackSystem"

#include <QtEntity/EntityManager>
#include <ShapeSystem>


Attack::Attack()
    : _attackMode(ATTACK_NONE)
    , _speed(10.0f)
{
}


AttackSystem::AttackSystem(QtEntity::EntityManager* em)
    : BaseClass(em)
    , _target(0)
{
}


QVariantMap AttackSystem::toVariantMap(QtEntity::EntityId eid, int)
{
    QVariantMap m;
    Attack* a;
    if(component(eid, a))
    {
        m["attackMode"] = QVariant::fromValue(attackMode(eid));
        m["speed"] = a->_speed;
    }
    return m;
}


void AttackSystem::fromVariantMap(QtEntity::EntityId eid, const QVariantMap& m, int)
{
    Attack* a;
    if(component(eid, a))
    {
        if(m.contains("attackMode"))
        {
            setAttackMode(eid, static_cast<AttackMode>(m["attackMode"].toInt()));           
        }
        if(m.contains("speed"))
        {
            a->_speed = m["speed"].toFloat();
        }
    }
}


QVariantMap AttackSystem::editingAttributes(int) const
{
    QStringList t;
    t << "ATTACK_NONE" << "ATTACK_STRAIGHT" << "ATTACK_CIRCLE";
    QVariantMap p;
    p["enumNames"] = t;
    QVariantMap m;
    m["attackMode"] = p;
    return m;
}


AttackMode AttackSystem::attackMode(QtEntity::EntityId eid) const
{
    Attack* a;
    if(component(eid, a))
    {
        return a->attackMode();
    }
    return ATTACK_NONE;
}


void AttackSystem::setAttackMode(QtEntity::EntityId eid, AttackMode v)
{
    Attack* a;
    if(component(eid, a))
    {
        a->setAttackMode(v);
    }
}


void AttackSystem::tick(int frameNumber, int totalTime, float delta)
{
    Q_UNUSED(frameNumber)
    Q_UNUSED(totalTime)

    ShapeSystem* shapesys = static_cast<ShapeSystem*>(entityManager()->system(qMetaTypeId<Shape>()));

    QVector2D targetPos = shapesys->position(_target);

    for(auto i = begin(); i != end(); ++i)
    {
        Attack* attack = i->second;

        if(attack->attackMode() == ATTACK_NONE)
        {
            continue;
        }

        QVector2D attackerPos = shapesys->position(i->first);
        QVector2D toTarget = targetPos - attackerPos;
        toTarget.normalize();
        switch(attack->attackMode())
        {
        case ATTACK_NONE: break;
        case ATTACK_STRAIGHT:
            attackerPos += toTarget * delta * attack->_speed;
            break;
        case ATTACK_CIRCLE:
            attackerPos += QVector2D(toTarget.y(), -toTarget.x()) * delta * attack->_speed;
            break;
        }
        shapesys->setPosition(i->first, attackerPos);
    }
}

