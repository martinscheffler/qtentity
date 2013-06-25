qtentity
========

A component entity system based on Qt's metatype system and signal/slot communication.
This can be used for game or simulation programming.
The basic idea is this: Game object functionality is not shared by inheritance but by composition.
Each game object is an aggregation of components, linked by a common entity id.

Base of the component entity system is the entity manager. It holds a number of entity systems.
Each entity system holds a list of components, indexed by entity id.

Any class derived from QObject can be used for components. Qt's property system is used for in-game introspection and serialization.

Example of an entity system holding components with a single int value:
`
	
	#pragma once
	#include <QtEntity/EntitySystem>

	class Damage : public QObject
	{
		Q_OBJECT

		// make energy getter/setters visible to Qt meta type system
		Q_PROPERTY(int energy READ energy WRITE setEnergy USER true)

	public:

		// Have to declare constructor as invokable so Qt can construct this
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
`

	QtEntity::EntityManager em;
	em.addEntitySystem(new DamageSystem());

Now create an entity and add a damage component to it:

`

	QtEntity::EntityId eid = em.createEntityId();

	Damage* damage; em.createComponent(eid, damage);

	// C++11 alternative:
	auto damage = em.createComponent<Damage>(eid);

You can retrieve components later by doing:
`

	Damage* damage; em.getComponent(eid, damage);

Included in the library are some examples and Qt widgets for managing and visualizing entities and their components.