/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EPhysicsWorldBase.h: Physics World base for Engine.
/*=============================================================================*/
#ifndef ELITE_PHYSICS_WORLD_BASE
#define	ELITE_PHYSICS_WORLD_BASE

//--- Includes ---
#include "../EliteGeometry/EGeometry2DTypes.h"

namespace Elite 
{
	template<typename physicsWorldType>
	class EPhysicsWorld : public ESingleton<EPhysicsWorld<physicsWorldType>>
	{
	public:
		//=== Constructors & Destructors ===
		EPhysicsWorld() { Initialize(); }
		~EPhysicsWorld();

		//=== World Functions ===
		void Simulate(float elapsedTime = 0.f);
		void RenderDebug() const;
		physicsWorldType GetWorld() const { return m_pPhysicsWorld; }
		std::vector<Elite::Polygon> GetAllStaticShapesInWorld(PhysicsFlags userFlags) const;

	private:
		//=== Datamembers ===
		physicsWorldType m_pPhysicsWorld;
		void* m_pDebugRenderer = nullptr;

		//=== Internal Functions ===
		void Initialize();
	};
}
#endif