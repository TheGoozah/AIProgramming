/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EConnection.h: class that represents a connection. Holds the cost of traveling from one Node to another.
/*=============================================================================*/
#ifndef ELITE_NAVIGATION_CONNECTION
#define ELITE_NAVIGATION_CONNECTION
//---Includes ---
#include "../../EliteHelpers/EMemoryPoolHelpers.h"
namespace Elite
{
	class Node; //Forward declaration of Node for Connection
	class Connection final : public IPoolable<Connection>
	{
	public:
		//--- Constructor & Destructor ---
		Connection() = default;
		Connection(Node* pStartNode, Node* pTargetNode) :
			m_pStartNode(pStartNode), m_pTargetNode(pTargetNode) {}
		~Connection() {}

		//--- Base Class Function ---
		void Initialize(){};
		void Destroy(){};

		//---- Cost Functions ---
		void SetGCost(float g) { m_GCost = g; }
		float GetGCost() const { return m_GCost; }
		void SetHCost(float h) { m_HCost = h; }
		float GetHCost() const { return m_HCost; }
		float GetFCost() const { return m_GCost + m_HCost; }
		void ResetCosts() { m_GCost = 0; m_FCost = 0; m_HCost = 0; }

		//--- General Functions ---
		Node* GetStartNode() const { return m_pStartNode; }
		void SetStartNode(Node* pStartNode) { m_pStartNode = pStartNode; }
		Node* GetEndNode() const { return m_pTargetNode; }
		void SetEndNode(Node* pEndNode) { m_pTargetNode = pEndNode; }
		Connection* GetHeadConnection() const { return m_pHeadConnection; }
		void SetHeadConnection(Connection* pC) { m_pHeadConnection = pC; }

	private:
		//--- Datamembers ---
		Node* m_pStartNode = nullptr;
		Node* m_pTargetNode = nullptr;
		Connection* m_pHeadConnection = nullptr; //Holds the connection that has been used to go to this connection

		float m_GCost = 0.f;
		float m_HCost = 0.f;
		float m_FCost = 0.f;
	};
}
#endif