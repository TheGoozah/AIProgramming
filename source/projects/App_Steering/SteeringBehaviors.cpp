//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "SteeringAgent.h"

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = (*m_pTargetRef).Position - pAgent->GetPosition(); //Desired Velocity
	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); //Rescale to Max Speed
	
	//DEBUG RENDERING
	if (pAgent->CanRenderBehaviour())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, steering.LinearVelocity.Magnitude(), { 0, 1, 0 ,0.5f }, 0.40f);

	return steering;
}

//WANDER (base> SEEK)
//******
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	//Calculate WanderOffset
	auto offset = pAgent->GetLinearVelocity();
	offset.Normalize();
	offset *= m_Offset;

	//WanderCircle Offset (Polar to Cartesian Coordinates)
	Elite::Vector2 circleOffset = { cos(m_WanderAngle) * m_Radius, sin(m_WanderAngle) * m_Radius };

	//Change the WanderAngle slightly for next frame
	m_WanderAngle += randomFloat() * m_AngleChange - (m_AngleChange * .5f); //RAND[-angleChange/2,angleChange/2]

	//Set target as Seek::Target
	auto newTarget = TargetData(pAgent->GetPosition() + offset + circleOffset);
	Seek::m_pTargetRef = &newTarget;

	//DEBUG RENDERING
	if (pAgent->CanRenderBehaviour())
	{
		auto pos = pAgent->GetPosition();
		DEBUGRENDERER2D->DrawSegment(pos, pos + offset, { 0,0,1 ,0.5f});
		DEBUGRENDERER2D->DrawCircle(pos + offset, m_Radius, { 0,0,1 ,0.5f }, -0.7f);
		DEBUGRENDERER2D->DrawSolidCircle(pos + offset + circleOffset, 0.1f, { 0,0 }, { 0, 1, 0 ,0.5f }, -0.75f);
	}

	//Return Seek Output (with our wander target)
	return Seek::CalculateSteering(deltaT, pAgent);
}

//PURSUIT (base> SEEK)
//*******
SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	auto distanceToTarget = Elite::Distance(pAgent->GetPosition(), (*m_pTargetRef).Position);
	auto offsetFromTarget = distanceToTarget / pAgent->GetMaxLinearSpeed();
	auto targetDirection = (*m_pTargetRef).LinearVelocity.GetNormalized();

	auto newTarget = TargetData((*m_pTargetRef).Position + (targetDirection * offsetFromTarget));
	Seek::m_pTargetRef = &newTarget;

	//DEBUG RENDERING
	if (pAgent->CanRenderBehaviour())
		DEBUGRENDERER2D->DrawSolidCircle(newTarget.Position, 0.3f, { 0,0 }, { 1, 0, 1, 0.5f }, 0.4f);

	return Seek::CalculateSteering(deltaT, pAgent);
}

//FLEE
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	auto steering = Seek::CalculateSteering(deltaT, pAgent);
	steering.LinearVelocity = steering.LinearVelocity * -1.0f;

	//DEBUG RENDERING
	if (pAgent->CanRenderBehaviour())
	{
		auto pos = pAgent->GetPosition();
		auto magn = steering.LinearVelocity.Magnitude();
		DEBUGRENDERER2D->DrawDirection(pos, steering.LinearVelocity, magn, { 0, 1, 0 ,0.5f }, 0.40f);
	}

	return steering;
}

//EVADE (base> FLEE)
//*****
SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	auto distanceToTarget = Elite::Distance(pAgent->GetPosition(), (*m_pTargetRef).Position);

	auto offsetFromTarget = distanceToTarget / pAgent->GetMaxLinearSpeed();
	auto targetDirection = (*m_pTargetRef).LinearVelocity.GetNormalized();

	auto newTarget = TargetData((*m_pTargetRef).Position + (targetDirection * offsetFromTarget));
	Flee::m_pTargetRef = &newTarget;

	//DEBUG RENDERING
	if (pAgent->CanRenderBehaviour())
		DEBUGRENDERER2D->DrawSolidCircle(newTarget.Position, 0.3f, { 0,0 }, { 1, 0, 1, 0.5f }, 0.4f);

	return Flee::CalculateSteering(deltaT, pAgent);
}

//ARRIVE
//******
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = (*m_pTargetRef).Position - pAgent->GetPosition(); //Total_Velocity to Target
	auto distance = steering.LinearVelocity.Normalize() - m_TargetRadius; //Total_Distance to Target + Normalize Total_Velocity 

	if (distance < m_SlowRadius) //Inside SlowRadius
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed() * (distance / (m_SlowRadius + m_TargetRadius)); //Slow DownS
	else
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); //Move to target at max speed

	//DEBUG RENDERING
	if (pAgent->CanRenderBehaviour())
	{
		auto pos = pAgent->GetPosition();
		DEBUGRENDERER2D->DrawCircle(pos, m_SlowRadius, { 0,0,1 ,0.5f }, 0.7f);
		DEBUGRENDERER2D->DrawCircle(pos, m_TargetRadius, { 1.0f, 0.271f, 0.f ,0.5f }, 0.7f);
		DEBUGRENDERER2D->DrawDirection(pos, steering.LinearVelocity, steering.LinearVelocity.Magnitude(), { 0, 1, 0 ,0.5f }, 0.40f);
	}

	return steering;
}

//ALIGN
//*****
SteeringOutput Align::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	auto rotation = (*m_pTargetRef).Orientation - pAgent->GetRotation();

	//Wrap Angle
	auto a = fmodf(rotation + b2_pi, 2 * b2_pi);
	rotation = a >= 0 ? (a - b2_pi) : (a + b2_pi);

	auto rotationSize = abs(rotation);
	if (rotationSize <= 0 || rotationSize < m_TargetAngle)
		return steering; //Should be empty :)

	auto targetRotationSpeed = pAgent->GetMaxAngularSpeed();;
	if (rotationSize < m_SlowAngle)
		targetRotationSpeed = pAgent->GetMaxAngularSpeed() * rotationSize / m_SlowAngle;

	targetRotationSpeed *= rotation / rotationSize;
	steering.AngularVelocity = targetRotationSpeed - pAgent->GetAngularVelocity();
	steering.AngularVelocity /= m_TimeToTarget;

	auto angularSpeed = abs(steering.AngularVelocity);
	if (angularSpeed > pAgent->GetMaxAngularSpeed())
	{
		steering.AngularVelocity /= angularSpeed;
		steering.AngularVelocity *= pAgent->GetMaxAngularSpeed();
	}

	steering.LinearVelocity = Elite::ZeroVector2;
	return steering;
}

//FACE
//****
SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	auto direction = (*m_pTargetRef).Position - pAgent->GetPosition();
	auto distance = direction.Magnitude();

	if (distance == 0.f)
		return steering; //Should be empty :)

	//Assemble Align Target (= new orientation > face target)
	auto newTarget = TargetData(*m_pTargetRef);
	newTarget.Orientation = atan2f(direction.x, -direction.y);

	//ALIGN BEHAVIOUR
	Align::m_pTargetRef = &newTarget;
	return Align::CalculateSteering(deltaT, pAgent);
}

SteeringOutput FacedArrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Arrive arriveBeh = Arrive();
	arriveBeh.SetTarget(m_pTargetRef);
	steering.LinearVelocity = arriveBeh.CalculateSteering(deltaT, pAgent).LinearVelocity;

	Face faceBeh = Face();
	faceBeh.SetTarget(m_pTargetRef);
	steering.AngularVelocity = faceBeh.CalculateSteering(deltaT, pAgent).AngularVelocity;

	return steering;
}

SteeringOutput AvoidObstacle::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	auto normVel = pAgent->GetLinearVelocity();	
	auto lengthVel = normVel.Normalize();

	auto dynamicLength = lengthVel / pAgent->GetMaxLinearSpeed() * 20.f;
	auto ahead = pAgent->GetPosition() + (normVel * dynamicLength);
	auto ahead2 = pAgent->GetPosition() + (normVel * dynamicLength * 0.5f);

	bool set;
	auto mostThreatening = FindMostThreateningObstacle(ahead, ahead2, pAgent->GetPosition(), pAgent->GetRadius(), set);

	if(set && pAgent->CanRenderBehaviour())
	{
		DEBUGRENDERER2D->DrawCircle(ahead, 0.5f, { 1,1,0 }, 0.4f);
		DEBUGRENDERER2D->DrawCircle(mostThreatening.center, mostThreatening.radius, { 0,1,0 }, 0.4f);
	}

	SteeringOutput steering = {};
	steering.LinearVelocity = ahead - mostThreatening.center;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= m_MaxAvoidanceForce;
	steering.AngularVelocity = 50.f;

	steering.IsValid = set; //Priority Steering

	return steering;
}

Obstacle AvoidObstacle::FindMostThreateningObstacle(const Elite::Vector2& ahead, const Elite::Vector2& ahead2, 
	const Elite::Vector2& position, float radius, bool& set)
{
	Obstacle mostThreatening = (m_Obstacles.size()>0)?m_Obstacles[0]: Obstacle();
	set = false;

	for (auto obstacle : m_Obstacles)
	{
		obstacle.radius += radius;
		auto collision = LineIntersectsCircle(ahead, ahead2, obstacle);

		// "position" is the character's current position
		if (collision && (set || Distance(position, obstacle.center) < Distance(position, mostThreatening.center))) {
			mostThreatening = obstacle;
			set = true;
		}
	}
	
	return mostThreatening;
}

float AvoidObstacle::Distance(Elite::Vector2 a, Elite::Vector2 b) const
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

bool AvoidObstacle::LineIntersectsCircle(Elite::Vector2 ahead, Elite::Vector2 ahead2, Obstacle obstacle) const
{
	return Distance(obstacle.center, ahead) <= obstacle.radius || Distance(obstacle.center, ahead2) <= obstacle.radius;
}
