/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ENode.h: Base class for nodes, each node has a position, a walkable flag and a pool of connections by default
/*=============================================================================*/
#ifndef ELITE_NAVIGATION_NODE
#define ELITE_NAVIGATION_NODE

//--- Includes ---
#include "EConnections.h"

namespace Elite
{
	//Default settings
	//Depending on usage of node, the defaultAmountOfConnections of connections can differ.
	//Nodes used in grid == 4 connections, triangle == 2 or 3 connections.
	//If default is 2, one expansion is required for grid, once.
	#define NODE_POOL_SIZE 2

	class Node : public IPoolable<Node>
	{
	public:
		//--- Constructor & Destructor ---
		Node() = default;
		Node(float x, float y) 
			:m_Position(Vector2(x, y)) {}
		explicit Node(const Vector2& pos)
			:m_Position(pos) {}
		~Node() = default; //Non virtual Destructor

		//--- Base Class Function ---
		//Initialize connections pool of node only when required!
		void Initialize()
		{ m_ConnectionsPool.InitializePool(NODE_POOL_SIZE, true); }
		void Destroy()
		{ m_ConnectionsPool.DestroyPool(); }

		//--- Datamember Functions ---
		const Vector2& GetPosition() const { return m_Position; }
		void SetPosition(const Vector2& p) { m_Position = p; }
		bool IsWalkable() const { return m_IsWalkable; }
		void SetWalkable(bool state) { m_IsWalkable = state; }

		//--- Connection Functions ---
		Connection* AddConnection(Node* t) 
		{ 
			auto availableConnection = m_ConnectionsPool.GetAvailableUnit();
			availableConnection->SetStartNode(this);
			availableConnection->SetEndNode(t);
			return availableConnection;
		}

		//Return vector with pointers from MemoryPool
		std::vector<Connection*> GetConnections() const 
		{ return m_ConnectionsPool.GetAllActiveUnits(); }

		void RemoveConnections() 
		{ m_ConnectionsPool.Flush(); }

		void ResetCosts() 
		{ 
			auto connections = m_ConnectionsPool.GetAllActiveUnits();
			for (auto pC : connections) 
			{ 
				pC->ResetCosts();
			} 
		}

	private:
		//--- Datamembers ---
		EMemoryPool<Connection> m_ConnectionsPool;
		Vector2 m_Position = ZeroVector2;
		bool m_IsWalkable = true;
	};
}
#endif