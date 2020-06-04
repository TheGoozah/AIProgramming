/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EPathfinder.h: Elite pathfinder base
/*=============================================================================*/
#ifndef ELITE_NAVIGATION_PATHFINDERBASE
#define ELITE_NAVIGATION_PATHFINDERBASE
namespace Elite
{
	template<class NodeType, class = std::enable_if<std::is_base_of<Node, NodeType>::value>>
	class Pathfinder
	{
	public:
		//--- Constructor & Destructor ---
		Pathfinder() = default;
		~Pathfinder() = default; //Non virtual Destructor

		//--- Pathfinder Template Functions ---
		std::vector<Vector2> FindPath(
			Graph<NodeType>* pGraph, 
			NodeType* pStartNode, NodeType* pEndNode,
			Heuristic heuristicFunction);

	protected:
		void CalculateCosts(Connection* pC, Node* pStartNode, Node* pTargetNode, Heuristic heuristicFunction)
		{
			//Calculate the g and h cost (f is calculate when requested)
			//g = current.g + cost(displacement vector current to this)
			float currentGCost = 0;
			if (pC->GetHeadConnection() != nullptr)
				currentGCost = pC->GetHeadConnection()->GetGCost();

			Vector2 parentPos = pStartNode->GetPosition();
			if (pC->GetHeadConnection() != nullptr)
				parentPos = pC->GetHeadConnection()->GetStartNode()->GetPosition();

			Vector2 tempV = GetAbs(pC->GetEndNode()->GetPosition() - parentPos);
			float gCost = heuristicFunction(tempV.x, tempV.y);
			pC->SetGCost(currentGCost + gCost);

			//h = distance from absolute displacement vector from this to goal, using the 'heuristicFunction'
			tempV = GetAbs(pC->GetEndNode()->GetPosition() - pTargetNode->GetPosition());
			float hCost = heuristicFunction(tempV.x, tempV.y);
			pC->SetHCost(hCost);
		}
	};
}
#endif