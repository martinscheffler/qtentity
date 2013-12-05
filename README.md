QtEntity
========

QtEntity is a component entity system built on the Qt framework. It helps in the creation of games,
simulation systems or other applications that need to handle dynamic data in a flexible way.
Game objects are composed from components in a flexible and efficient way.

QtEntity provides a powerful property system based on the QVariant data type.
This can be used for component introspection, serialization and scripting.

QtEntity is easy to integrate with rendering systems and can be extended in various ways.

Entity Component System Basics
-------------
From  [http://entity-systems.wikidot.com/]
	Entity Systems (ES) are a technique/architecture for building complex extensible projects 
	(mostly: computer games) based around pluggable behaviours and lean, fast, modular data.
	This makes the programming process leaner and easier to extend, with advantages in:
        performance, extensibility, and game-design flexibility

An entity is a set of components. As an example, a spaceship entity may consist of a drawable component,
a physics component, a sound component and a position component. The way the spaceship entity
is composed from components depends on the application being developed and may be done
in a number of ways.

All components of one class are stored in one place. For each component class
there exists one object of the class EntitySystem which holds the component instances and is
responsible for creating, deleting and retrieving them.

Components are indexed by entity ids. An entity id is simply a unique integer
identifying an entity. To get a list of all components of an entity it is necessary to
iterate over all EntitySystems and to query for components indexed by the entity id.

Each entity system can only hold a single component for a given entity id.
To have an entity have lists of something you should not have multiple components of the same type,
instead you should have a single component handling a list of things.

All entity systems are stored in the EntityManager. Usually there is only one EntityManager
in an application. Access to the EntityManager gives access to all entity systems.
The EntityManager provides a number of convenience methods to create, retrieve or delete components
in its systems. It is also responsible for handing out entity ids.

A spaceship example
-------------
Creating a spaceship entity with two components, transform and sprite.

        QtEntity::EntityId spaceShipId = entityManager.createEntityId();
        Transform* transform;
        entityManager.createComponent(spaceShipId, transform);
        Sprite* sprite;
        entityManager.createComponent(spaceShipId, sprite, {"path", ":/assets/spaceArt.svg"}, {"subTex", QRect(374,360,106,90)});

The third parameter to createComponent accepts a QVariantMap with values for the
component properties - more on that later.

Components
-------------
All components in QtEntity derive from the abstract interface QtEntity::Component. It defines a
single method returning a type id for the component class.
There is a pair of macros which may be used to create a type id for the component and to implement
the necessary methods. In the header:

        class ExampleComponent : public QtEntity::Component
        {
            DECLARE_COMPONENT_TYPE(NOEXP)

        public:
            ExampleComponent() { }
        };

And in the cpp:

        IMPLEMENT_COMPONENT_TYPE(ExampleComponent)

Entity Systems
-------------
The entity system class is derived from QObject.
Entity systems are used to create, retrieve and delete components.



They also give access to the properties of its components: The properties() method returns
a QVariantMap containing properties of the component associated with the given id. The setProperties method can
be used to assign values to the properties of a specified component.

Properties can be used in editor tools for introspection and manipulation and also for serializing to JSON or other formats.
Component construction methods (EntitySystem::createComponent()) accept a QVariantMap which can hold initialization 
values for the properties. Entity system implementations are themselves responsible for fetching and applying these property maps.

Example of an entity system holding components with a single int value:

	
	#include <QtEntity/PooledEntitySystem>

	class Damage : public QtEntity::Component
	{
		DECLARE_COMPONENT_TYPE(NOEXP)    

	public:

		Damage() : _energy(0) { }

		void setEnergy(int v) { _energy = v; }
		int energy() const { return _energy; }

	private:

		int _energy;

	};


        class DamageSystem : public QtEntity::PooledEntitySystem<Damage>
        {

        public:
            typedef QtEntity::PooledEntitySystem<Damage> BaseClass;
            DamageSystem(QtEntity::EntityManager* em);

            virtual QVariantMap properties(QtEntity::EntityId eid) override;
            virtual void setProperties(QtEntity::EntityId eid, const QVariantMap& m) override;

            void step(int frameNumber, int totalTime, int delta);
        };

	// Source file

        DamageSystem::DamageSystem(QtEntity::EntityManager* em)
            : BaseClass(em)
        {
        }


        QVariantMap DamageSystem::properties(QtEntity::EntityId eid)
        {
            QVariantMap m;
            Damage* d;
            if(component(eid, d))
            {
                m["energy"] = d->energy();
            }
            return m;
        }


        void DamageSystem::setProperties(QtEntity::EntityId eid, const QVariantMap& m)
        {
            Damage* d;
            if(component(eid, d))
            {
                if(m.contains("energy")) d->setEnergy(m["energy"].toInt());
            }
        }


        void DamageSystem::step(int frameNumber, int totalTime, int delta)
        {
            for(auto i = begin(); i != end(); ++i)
            {
                Shape* shape;
                entityManager()->component(i->first, shape);
            }
        }


Now add this to an entity manager:


	QtEntity::EntityManager em;
        DamageSystem* ds = new DamageSystem(&em);

Now create an entity and add a damage component to it:

        QtEntity::EntityId eid = em.createEntityId();

        QVariantMap props;
        props["energy"] = 100;
        Damage* damage; em.createComponent(eid, damage, props);

	// C++11 alternative:
	// auto damage = em.createComponent<Damage>(eid);

You can retrieve components later by doing:

	Damage* damage; em.getComponent(eid, damage);

Included in the library are some examples and Qt widgets for managing and visualizing entities and their components.
