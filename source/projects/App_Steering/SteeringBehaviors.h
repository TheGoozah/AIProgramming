/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviours.h: SteeringBehaviours interface and different implementations
/*=============================================================================*/
#ifndef ELITE_STEERINGBEHAVIORS
#define ELITE_STEERINGBEHAVIORS

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "SteeringHelpers.h"
class SteeringAgent;
using namespace Elite;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior(){}

	virtual SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) = 0;

	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }
	int temp_id = 0;
};
#pragma endregion

///////////////////////////////////////
//SEEK
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	//Seek Functions
	virtual void SetTarget(const TargetData* pTarget) { m_pTargetRef = pTarget; }

protected:
	const TargetData* m_pTargetRef = nullptr;
};

//////////////////////////
//WANDER
//******
class Wander : public Seek
{
public:
	Wander() = default;
	virtual ~Wander() = default;

	//Wander Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	void SetWanderOffset(float offset) { m_Offset = offset; }
	void SetWanderRadius(float radius) { m_Radius = radius; }
	void SetMaxAngleChange(float rad) { m_AngleChange = rad; }

protected:
	float m_Offset = 6.f; //Offset (Agent Direction)
	float m_Radius = 4.f; //WanderRadius
	float m_AngleChange = ToRadians(45); //Max WanderAngle change per frame
	float m_WanderAngle = 0.f; //Internal

private:
	void SetTarget(const TargetData* pTarget) override {} //Hide SetTarget, No Target needed for Wander
};

////////////////////////////
//PURSUIT
//******
class Pursuit : public Seek
{
public:
	Pursuit() = default;
	virtual ~Pursuit() = default;

	//Pursuit Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	//Pursuit Functions (Hides Seek::SetTarget)
	virtual void SetTarget(const TargetData* pTarget) override { m_pTargetRef = pTarget; }

protected:
	const TargetData* m_pTargetRef = nullptr;
};

/////////////////////////
//FLEE
//****
class Flee : public Seek
{
public:
	Flee() = default;
	virtual ~Flee() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

//////////////////////////
//EVADE
//*****
class Evade : public Flee
{
public:
	Evade() = default;
	virtual ~Evade() = default;

	//Evade Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	//Evade Functions (Hides Flee::SetTarget)
	void SetTarget(const TargetData* pTarget) override { m_pTargetRef = pTarget; }

protected:
	const TargetData* m_pTargetRef = nullptr;
};

/////////////////////////////////////////
//ARRIVE
//******
class Arrive : public ISteeringBehavior
{
public:
	Arrive() = default;
	virtual ~Arrive() = default;

	//Arrive Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	//Arrive Functions
	virtual void SetTarget(const TargetData* pTarget) { m_pTargetRef = pTarget; }
	void SetSlowRadius(float radius) { m_SlowRadius = radius; }
	void SetTargetRadius(float radius) { m_TargetRadius = radius; }

protected:

	const TargetData* m_pTargetRef = nullptr;
	float m_SlowRadius = 15.f;
	float m_TargetRadius = 3.f;
};

/////////////////////////////////////////
//ALIGN
//******
class Align : public ISteeringBehavior
{
public:
	Align() = default;
	virtual ~Align() = default;

	//Align Fuctions
	virtual void SetTarget(const TargetData* pTarget) { m_pTargetRef = pTarget; }

	void SetTargetAngle(float rad) { m_TargetAngle = rad; }
	void SetSlowAngle(float rad) { m_SlowAngle = rad; }
	void SetTimeToTarget(float time) { m_TimeToTarget = time; }

	//Align Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

protected:
	const TargetData* m_pTargetRef = nullptr;

private:

	float m_TargetAngle = ToRadians(1);
	float m_SlowAngle = ToRadians(15);
	float m_TimeToTarget = 1.f;
};

/////////////////////////////////////////
//FACE
//******
class Face : public Align
{
public:
	Face() = default;
	virtual ~Face() = default;

	//Face Functions (Align::SetTarget override)
	void SetTarget(const TargetData* pTarget) override { m_pTargetRef = pTarget; }

	//Face Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

protected:
	const TargetData* m_pTargetRef; //Allign::m_pTargetRef 'override' > hiding :)
};

//FACED_ARRIVE
//************
class FacedArrive : public ISteeringBehavior
{
public:
	FacedArrive() = default;
	virtual ~FacedArrive() = default;

	//FacedArrive Functions
	virtual void SetTarget(const TargetData* pTarget) { m_pTargetRef = pTarget; }

	//Arrive Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

protected:
	const TargetData* m_pTargetRef = nullptr;
};


//AVOID-OBSTACLE
struct Obstacle
{
	Elite::Vector2 center;
	float radius;
};

class AvoidObstacle : public ISteeringBehavior
{
public:
	AvoidObstacle(vector<Obstacle> obstacles) : m_Obstacles(obstacles) {};
	virtual ~AvoidObstacle() = default;

	//AvoidObstacle Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	//AvoidObstacle Functions
	void SetMaxAvoidanceForce(float max) { m_MaxAvoidanceForce = max; }

protected:

	vector<Obstacle> m_Obstacles = {};
	float m_MaxAvoidanceForce = 10.f;

private:
	Obstacle FindMostThreateningObstacle(const Elite::Vector2& ahead, const Elite::Vector2& ahead2, 
		const Elite::Vector2& position, float radius, bool& set);
	float Distance(Elite::Vector2 a, Elite::Vector2 b) const;
	bool LineIntersectsCircle(Elite::Vector2 ahead, Elite::Vector2 ahead2, Obstacle obstacle) const;
		
};
#endif