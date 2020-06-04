/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EGraph.h: class that implements a graph using a memory pool. Using Nodes and Connections.
/*=============================================================================*/
#ifndef ELITE_NAVIGATION_GRAPH
#define ELITE_NAVIGATION_GRAPH

namespace Elite
{
	//Forward Declarations
	class Node;

	//Class
	template<class T, class = std::enable_if<std::is_base_of<Node, T>::value>>
	class Graph
	{
	public:
		//--- Constructor & Destructor ---
		//Always initialize pool of graph because that is the purpose of the graph, storing data
		Graph(unsigned int poolSize = 20, bool isExpandable = false) { m_Nodes.InitializePool(poolSize, isExpandable); };
		~Graph() = default; //Non virtual Destructor

		//--- Graph Functions ---
		void ClearGraph()
		{ 
			m_Nodes.Flush(); 
		}

		T* AppendNode(const T& node)
		{
			T* pNode = m_Nodes.GetAvailableUnit();
			if (pNode != nullptr)
			{
				//Copy content to pool using 'placement new'.
				//This will make sure any vtable data is set too! Lese we would just do: *pNode = node;
				//IMPORTANT: this does not allocate new memory!
				//Reference: https://stackoverflow.com/questions/222557/what-uses-are-there-for-placement-new
				pNode = new (pNode) T(node);
				//Initialize this object (so it's internal data get's initialized)! In this case, also a memory pool!
				pNode->Initialize();
			}
			return pNode;
		}

		//Return vector with pointers from MemoryPool
		std::vector<T*> GetNodes() const
		{ return m_Nodes.GetAllActiveUnits(); }
		
		//Search for the closest node in the graph
		T* GetClosestNode(const Vector2& p, Heuristic heuristicFunction)
		{
			//Local variables
			T* pClosestNode = nullptr;
			unsigned int pAmountOfActiveNodes = 0;
			auto closestSqrtDistance = (std::numeric_limits<float>::max)();
			//Retrieve pool information
			auto pNodes = m_Nodes.GetAllActiveUnits();
			if (pNodes.size() == 0)
				return nullptr;
			//Loop and find closest
			for (auto pNode : pNodes)
			{
				//Cast to base Node, is always of base Node!
				Node* pBaseNode = static_cast<Node*>(pNode);

				//Check if walkable, if not, increment and continue search
				if (!pBaseNode->IsWalkable())
				{ continue;	}

				auto direction = GetAbs(pBaseNode->GetPosition() - p);
				auto sqrtDistance = heuristicFunction(direction.x, direction.y);
				if (sqrtDistance < closestSqrtDistance)
				{
					pClosestNode = pNode;
					closestSqrtDistance = sqrtDistance;
				}
			}
			return pClosestNode;
		}

		//Search for the closest node in the graph in direction
		T* GetClosestNodeInDirection(const Vector2& p, const Vector2& d, Heuristic heuristicFunction)
		{
			//Local variables
			T* pClosestNode = nullptr;
			T* pCurrentNode = nullptr;
			unsigned int pAmountOfActiveNodes = 0;
			auto closestSqrtDistance = (std::numeric_limits<float>::max)();
			auto bestDP = -1.f;
			//Retrieve pool information
			m_ConnectionsPool.GetAllActiveUnits(pCurrentNode, pAmountOfActiveNodes);
			if (pCurrentNode == nullptr)
				return pCurrentNode;
			//Loop and find closest
			for (unsigned int i = 0; i < pAmountOfActiveNodes; ++i)
			{
				//Check if walkable, if not, increment and continue search
				if (!static_cast<Node*>(pCurrentNode)->IsWalkable()) //Is always of base Node!
				{
					++pCurrentNode; //Move to next node
					continue;
				}

				auto direction = abs(static_cast<Node*>(pCurrentNode)->GetPosition() - p);
				auto normalizedDirection = direction.GetNormalized();
				auto dp = normalizedDirection.Dot(d);
				if (dp > bestDP)
				{
					bestDP = dp;
					auto sqrtDistance = heuristicFunction(direction.x, direction.y);
					if (sqrtDistance < closestSqrtDistance)
					{
						pClosestNode = pCurrentNode;
						closestSqrtDistance = sqrtDistance;
					}
				}
				++pCurrentNode; //Move to next node
			}
			return pClosestNode;
		}

	private:
		//--- Datamembers ---
		EMemoryPool<T> m_Nodes = {};
	};
}
#endif