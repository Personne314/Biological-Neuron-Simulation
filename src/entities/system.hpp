#pragma once

#include <type_traits>

#include "entities.h"
#include "entity.hpp"



namespace chroma::ecs
{

/**
 * @interface ISystem
 * @brief Used for System storage through ISystem*.
 */
class ISystem {
public:
    virtual ~ISystem() = default;
    virtual void update(Entities &entities, float dt) = 0;
};



/**
 * @class System
 * @brief This is used to perform specific action on entities that shares the same components.
 * @note This class must be extended in order to works. T_Parent is the name of the parent class.
 * @note Class extending System must implement process(Entity, T_Components&..., float).
 */
template<typename T_Parent, typename... T_Components>
class System : public ISystem
{
public:

	System() noexcept = default;
	~System() = default;

	void update(Entities &entities, float dt) override 
	{
		static_assert(std::is_invocable_v<
			decltype(&T_Parent::process), 
			T_Parent, Entity, T_Components&..., float
		>, "Error: Your System class must implement 'process(Entity, T_Components&..., float)'.");
		auto &derived = *static_cast<T_Parent*>(this);
		entities.run_system<T_Components...>(derived, dt);
	}

};

}
