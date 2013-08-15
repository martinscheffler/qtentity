QtEntity
========

QtEntity is an entity component system using the Qt framework. It can aid in the creation of games, simulation systems or other applications that need to handle lots of state in a flexible way. Functionality of game or simulation objects can be separated into components and composed during runtime in a flexible and efficient way. 

All components derive from the Qt base class QObject. QtEntity uses the Qt property system for component introspection, serialization and scripting. The Qt meta type system is used for component construction and for handling property object arrays.


Entity Component System Basics
-------------
From  [http://entity-systems.wikidot.com/]
	Entity Systems (ES) are a technique/architecture for building complex extensible projects (mostly: computer games) based around pluggable behaviours and lean, fast, modular data.
	This makes the programming process leaner and easier to extend, with advantages in: performance, extensibility, and game-design flexibility

Entities are collections of components. A spaceship entity may consist of a drawable component, a physics component, a sound component and a position component. The way the spaceship entity is decomposed into components depends on the application being developed and may be done in a number of ways.  

The class EntitySystem is responsible for storing components. An EntitySystem instance holds all instances of a specific component class. The components are stored indexed by an EntityId, which is simply an integer.  Because all instances of a component class are in one place it is extremely efficient and simple to iterate them.
The EntitySystem provides methods to create, retrieve and delete components for a given EntityId. It is not possible to create multiple components with the same id in an entity system.

All EntitySystems are stored in the EntityManager. Usually there is only one EntityManager instance in an application. Access to the EntityManager gives access to all entity systems and the components they hold.
The EntityManager provides a number of convenience methods to create, retrieve or delete components in its systems.

Usage of Qt functionality
-------------
All components in QtEntity derive from QObject. Component subclasses must use the Q_OBJECT macro to establish their own Qt meta type. Components can have QProperties. These are used in editor tools for introspection and manipulation and also for serializing to JSON or other formats. 

Example of an entity system holding components with a single int value:

	
	#pragma once
	#include <QtEntity/SimpleEntitySystem>

	class Damage : public QObject
	{
		Q_OBJECT

		// make energy getter/setters visible to Qt meta type system
		Q_PROPERTY(int energy READ energy WRITE setEnergy USER true)

	public:

		// Have to declare default constructor as invokable so Qt can construct this
		Q_INVOKABLE Damage() : _energy(0) { }

		void setEnergy(int v) { _energy = v; }
		int energy() const { return _energy; }

	private:
		int _energy;
	};


	class DamageSystem : public QtEntity::EntitySystem
	{
		Q_OBJECT

	public:
		DamageSystem();
	};

Now add this to an entity manager:


	QtEntity::EntityManager em;
	em.addEntitySystem(new DamageSystem());

Now create an entity and add a damage component to it:


	QtEntity::EntityId eid = em.createEntityId();
	Damage* damage; em.createComponent(eid, damage);

	// C++11 alternative:
	// auto damage = em.createComponent<Damage>(eid);

You can retrieve components later by doing:


	Damage* damage; em.getComponent(eid, damage);

Included in the library are some examples and Qt widgets for managing and visualizing entities and their components.
