/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringAgent.h: basic agent using steering behaviours
/*=============================================================================*/
#ifndef STEERING_AGENT_H
#define STEERING_AGENT_H

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "../Shared/BaseAgent.h"
#include "SteeringHelpers.h"
class ISteeringBehavior;

class SteeringAgent final : public BaseAgent
{
public:
	//--- Constructor & Destructor ---
	SteeringAgent() = default;
	~SteeringAgent() = default;

	//--- Agent Functions ---
	void Update(float dt) override;
	void Render(float dt) override;

	float GetMaxLinearSpeed() const { return m_MaxLinearSpeed; }
	void SetMaxLinearSpeed(float maxLinSpeed) { m_MaxLinearSpeed = maxLinSpeed; }

	float GetMaxAngularSpeed() const { return m_MaxAngularSpeed; }
	void SetMaxAngularSpeed(float maxAngSpeed) { m_MaxAngularSpeed = maxAngSpeed; }

	bool IsAutoOrienting() const { return m_AutoOrient; }
	void SetAutoOrient(bool autoOrient) { m_AutoOrient = autoOrient; }

	float GetOrientation() const { return GetRotation(); }

	Elite::Vector2 GetDirection() const { return GetLinearVelocity().GetNormalized(); }
	const TargetData* GetTargetRef() const { return &m_SteeringParams; }

	void SetSteeringBehaviour(ISteeringBehavior* pBehaviour) { m_pSteeringBehaviour = pBehaviour; }
	ISteeringBehavior* GetSteeringBehaviour() const { return m_pSteeringBehaviour; }

	void SetRenderBehaviour(bool isEnabled) { m_RenderBehaviour = isEnabled; }
	bool CanRenderBehaviour() const { return m_RenderBehaviour; }

private:
	//--- Datamembers ---
	ISteeringBehavior* m_pSteeringBehaviour = nullptr;
	SteeringParams m_SteeringParams = {};

	float m_MaxLinearSpeed = 10.f;
	float m_MaxAngularSpeed = 10.f;
	bool m_AutoOrient = false;
	bool m_RenderBehaviour = false;
};
#endif