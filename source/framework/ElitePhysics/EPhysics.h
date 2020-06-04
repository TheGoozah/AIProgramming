/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EPhysics.h: General Physics header that includes all physics types and utilities
/*=============================================================================*/
#ifndef ELITE_PHYSICS
#define ELITE_PHYSICS

/* --- FLAGS --- */
enum PhysicsFlags
{
	Default = 0,
	NavigationCollider = 1
};

/* --- INCLUDES --- */
#include "ERigidBodyBase.h"
#include "EPhysicsWorldBase.h"
#include "EPhysicsTypes.h"

/* --- PLATFORM-SPECIFIC DEFINES --- */
#ifdef USE_BOX2D
	//=== Third-Party Includes ===
	#include <Box2D/Box2D.h>
	
	typedef Elite::ETransform<Elite::Vector2, Elite::Vector2> Transform;
	typedef Elite::RigidBodyBase<Elite::Vector2, Elite::Vector2> RigidBody;
	typedef Elite::EPhysicsWorld<b2World*> PhysicsWorld;
#endif
#endif