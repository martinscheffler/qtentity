#include <QTEntity/Component>
#include <QTEntity/DataTypes>
#include <QTEntity/EntityManager>
#include <QTEntity/EntitySystem>

#include <QHash>
#include <QVector3D>

class Transform : public qte::Component
{
    Q_PROPERTY(QVector3D position READ position WRITE setPosition USER true)
public:
    void setPosition(const QVector3D& v) { _position = v; }
    QVector3D getPosition() const  { return _position; }
private:
    QVector3D _position;
};


template <typename T>
class EntitySystemImpl : public qte::EntitySystem
{
public:
    typedef QHash<qte::EntityId, T*> ComponentStore;

    bool getComponent(qte::EntityId id, T*& component) const
    {
        typename ComponentStore::const_iterator i = _components.find(id);
        if(i == _components.end())
        {
            component = NULL;
            return false;
        }
        component = i.value();
        return true;
    }

    virtual qte::Component* get(qte::EntityId id) const
    {
        T* c;
        if(getComponent(id, c)) return c;
        return NULL;
    }

    virtual qte::Component* create(qte::EntityId id)
    {
        if(get(id) != NULL) return NULL;
        T* t = new T();
        _components[id] = t;
        return t;
    }

    virtual bool destroy(qte::EntityId id)
    {
        typename ComponentStore::iterator i = _components.find(id);
        if(i == _components.end()) return false;
        delete i.value();
        _components.erase(i);
        return true;
    }

private:

    ComponentStore _components;
};

class TransformSystem : public EntitySystemImpl<Transform>
{
public:

};

int main(int argc, char *argv[])
{
    qte::EntityManager em;
    TransformSystem* ts = new TransformSystem();
    em.addEntitySystem(ts);
    ts->create(1);
    ts->get(1);

    return 0;
}
