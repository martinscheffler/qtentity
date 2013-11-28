QtEntity
========

QtEntity is a component entity system built on the Qt framework. It can aid in the creation of games,
simulation systems or other applications that need to handle lots of state in a flexible way.
Functionality of game or simulation objects can be separated into components and composed 
during runtime in a flexible and efficient way. 

QtEntity provides a property system based on the QVariant data type. 
This can be used for component introspection, serialization and scripting. 

Entity Component System Basics
-------------
From  [http://entity-systems.wikidot.com/]
	Entity Systems (ES) are a technique/architecture for building complex extensible projects 
	(mostly: computer games) based around pluggable behaviours and lean, fast, modular data.
	This makes the programming process leaner and easier to extend, with advantages in:
        performance, extensibility, and game-design flexibility

Entities are collections of components. A spaceship entity may consist of a drawable component, 
a physics component, a sound component and a position component. The way the spaceship entity
is decomposed into components depends on the application being developed and may be done in a number of ways.

Components are stored in entity systems. A single system instance holds all instances of a specific component type.
A system is responsible for creating, deleting and retrieving its components.

Components are indexed by entity ids. An entity id is simply a unique integer identifying an entity. For each entity
id there may only be one component in a system.

All systems are stored in the EntityManager. Usually there is only one EntityManager instance
in an application. Access to the EntityManager gives access to all entity systems.
The EntityManager provides a number of convenience methods to create, retrieve or delete components
in its systems. It is also responsible for creating entity ids.

Example: Creating a spaceship entity with two components, transform and sprite.

        QtEntity::EntityId spaceShipId = entityManager.createEntityId();
        Transform* transform;
        entityManager.createComponent(spaceShipId, transform);
        Sprite* sprite;
        entityManager.createComponent(spaceShipId, sprite, {"path", ":/assets/spaceArt.svg"}, {"subTex", QRect(374,360,106,90)});

The third parameter to createComponent accepts a QVariantMap with values for the component properties - more later.

Implementation details
-------------
All components in QtEntity derive from the abstract interface QtEntity::Component. It defines a
single method that returns a type id for the component class.

The entity system class is derived from QObject.
Entity systems can be directly used to create, retrieve or delete components, although usually this is called by
the EntityManager.

Systems also give access to the properties of its components: The properties method accepts an entity id
and returns a QVariantMap containing properties of the component associated with the id. The setProperties method can
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
