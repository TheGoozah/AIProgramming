//=== General Includes ===
#include "stdafx.h"
#include "ENavigationMeshPathfinder.h"
using namespace Elite;

//=== Static Debug Variable Initialization ===
#ifdef _DEBUG
	bool NavigationMeshPathfinder::sDrawDebugInfoQueries = false;
	bool NavigationMeshPathfinder::sDrawPortals = false;
#endif

//=== Pathfinder Functions ===
std::vector<Elite::Vector2> NavigationMeshPathfinder::FindPath(
	Graph<NavigationMeshNode>* pGraph,
	NavigationMeshNode StartNode, NavigationMeshNode EndNode,
	Heuristic heuristicFunction)
{
	//Variables
	std::vector<Elite::Vector2> vPath;
	NavigationMesh* pNavigationMesh = reinterpret_cast<NavigationMesh*>(pGraph);
	if (pNavigationMesh == nullptr)
	{
		std::cout << "ERROR: searching path on NavigationMesh without passing a NavigationMesh Graph!" << std::endl;
		return vPath;
	}

	//--------------- PREPARATIONS ---------------
	//Clearing Graph, adding initial path nodes & setting up variables
	//--------------------------------------------
	//Clear old NavigationMesh graph (flush nodes and connections) and flags first!
	pNavigationMesh->ResetNavigationMesh();

	//Add the start and endNode (copies over the information to the local memory pool)
	auto pStartNode = pNavigationMesh->AppendNode(StartNode);
	auto pEndNode = pNavigationMesh->AppendNode(EndNode);

	//A* containers and variables
	std::vector<Connection*> openList;
	std::vector<Connection*> closedList;
	Connection* pCurrentConnection = nullptr;

	//Start by finding the current triangle we reside in AND the triangle that our goal is in.
	auto pStartTriangle = pNavigationMesh->GetCurrentTriangleFromPosition(pStartNode->GetPosition());
	auto pEndTriangle = pNavigationMesh->GetCurrentTriangleFromPosition(pEndNode->GetPosition());

	//Fallback: if one of the triangle is not found, get the closest triangle instead
	if (pStartTriangle == nullptr)
		pStartTriangle = pNavigationMesh->GetClosestTriangle(pStartNode->GetPosition());
	if (pEndTriangle == nullptr)
		pEndTriangle = pNavigationMesh->GetClosestTriangle(pEndNode->GetPosition());

	//Last safety check!
	if (pStartTriangle == nullptr || pEndTriangle == nullptr)
		return vPath;

	//If they are equal, pass the endNode as part of path and stop already! We've already reached our goal :)
	if (pStartTriangle == pEndTriangle)
	{
		vPath.push_back(pEndNode->GetPosition());
		return vPath;
	}
	
	//--------------- CALCULATIONS ---------------
	//Do the actual path calculations. We generate our "graph" (== query) on the fly, 
	//using the A* (heurstics) algorithm to determine next connections.
	//--------------------------------------------
	//Do the initial setup == create the first nodes we can visit.
	//Use PlausibleLines(...) to get all the surrounding lines we need to check, start by using our start triangle.
	auto vPossibleSegments = pNavigationMesh->GetPlausibleLines(pStartTriangle);
	for (auto pL : vPossibleSegments)
	{
		//Project point
		auto projP = ProjectOnLineSegment(pL->p1, pL->p2, pStartNode->GetPosition(), 1.0f);
		//"Create" node by asking for available node (AppendNode) from pool (returns pointer to node from pool).
		auto pNode = pNavigationMesh->AppendNode(NavigationMeshNode(projP, pL->index, pStartTriangle));
		//Connect
		pStartNode->AddConnection(pNode);
		//Flag line
		pNavigationMesh->SetLineReviewed(pL->index);
	}
	//Calculate the costs of the startNode connections and append to openlist. This only needs to happen
	//once, so not in the loop. Because we've created this during the setup in the beginning of this function.
	for (auto pC : pStartNode->GetConnections())
	{
		CalculateCosts(pC, pStartNode, pEndNode, heuristicFunction);
		openList.push_back(pC);
	}

	//Generate query on the fly - expanding based on the current closest node (through connection).
	//We loop while we still have nodes in the openlist.
	//We stop when we hit the triangle that holds the targetNode (internal check)
	while (openList.size() != 0)
	{
		//--------------------------------------------------------------------------------------------
		//1. Get the connection with the lowest F from the openList
		float lowestFScore = (std::numeric_limits<float>::max)(); //Wrapping prevents macro expansion!
		for (auto pC : openList)
		{
			auto fCost = pC->GetFCost();
			if (fCost < lowestFScore)
			{
				pCurrentConnection = pC;
				lowestFScore = fCost;
			}
		}

		//2. Pop current off the open list and push it to the closed
		//First check if not out of range, else we would try to erase something that is not there.
		//If so, recalculate and restart loop!
		if (std::find(openList.begin(), openList.end(), pCurrentConnection) == openList.end())
		{
			printf("CurrentConnection not in openlist! Forced retrieve cheapest connection!");
			for (auto pC : openList)
				CalculateCosts(pC, pStartNode, pEndNode, heuristicFunction);
			continue; //Retry == Restart outer loop
		}
		//If ok, do the actual push and pop.
		openList.erase(std::remove(openList.begin(), openList.end(), pCurrentConnection));
		closedList.push_back(pCurrentConnection);

		//--------------------------------------------------------------------------------------------
		//3. From this connection (pEndNode), see if we need to expand our graph.
		//3.1 If one of the surroundingTriangles is the end triangle, STOP!
		auto pCurrentNode = pCurrentConnection->GetEndNode();
		auto targetIndex = static_cast<const NavigationMeshNode*>(pCurrentNode)->GetAssociatedLineIndex(); //In this graph, every node is NavigationMeshNode
		if (targetIndex == pEndTriangle->metaData.IndexLine1 
			|| targetIndex == pEndTriangle->metaData.IndexLine2
			|| targetIndex == pEndTriangle->metaData.IndexLine3)
		{
			//Set the traversed triangle of endNode equals to it's own triangle
			auto pTempConnection = pCurrentConnection->GetEndNode()->AddConnection(pEndNode);
			pTempConnection->SetHeadConnection(pCurrentConnection);
			pCurrentConnection = pTempConnection;
			openList.clear();
			break;
		}

		//Else continue with current targetIndex
		auto currentPosition = pCurrentNode->GetPosition();
		auto vpSurroundingTriangles = pNavigationMesh->GetTrianglesFromLineIndex(targetIndex);

		//EDGE CASE: test to see if the start triangle is included (edge case where you start in a triangle with only 
		//one possible way out = 1 possible edge, and the start triangle hasn't been parsed). Because by default,
		//we search for triangles with 2 possible edges.
		if (std::find(vpSurroundingTriangles.begin(), vpSurroundingTriangles.end(), pStartTriangle) 
			!= vpSurroundingTriangles.end())
		{
			vpSurroundingTriangles.erase(
				std::remove(vpSurroundingTriangles.begin(), vpSurroundingTriangles.end(), pStartTriangle),
				vpSurroundingTriangles.end());
		}

		//Find new triangle we are interested in
		const Triangle* pCurrentTriangle = nullptr;
		if (vpSurroundingTriangles.size() > 1)
		{
			//The new triangle is the one with only one line reviewed
			auto ft = vpSurroundingTriangles[0];
			auto v = static_cast<int>(pNavigationMesh->IsLineReviewed(ft->metaData.IndexLine1))
				| static_cast<int>((pNavigationMesh->IsLineReviewed(ft->metaData.IndexLine2)) << 1)
				| static_cast<int>((pNavigationMesh->IsLineReviewed(ft->metaData.IndexLine3)) << 2);

			pCurrentTriangle = vpSurroundingTriangles[(v <= 4 ? 0 : 1)];
		}
		else if (vpSurroundingTriangles.size() == 1)
			pCurrentTriangle = vpSurroundingTriangles[0];

		//3.3 Check if we need to expand by checking asking the adjacent lines and see if all are flagged
		vPossibleSegments = pNavigationMesh->GetPlausibleLines(pCurrentTriangle);
		for (auto pL : vPossibleSegments)
		{
			if (!pNavigationMesh->IsLineReviewed(pL->index))
			{
				//Project point
				auto projP = ProjectOnLineSegment(pL->p2, pL->p1, currentPosition, 1.0f);
				//"Create" node by asking for available node (AppendNode) from pool (returns pointer to node from pool).
				//Do use currentTriangle this time!
				auto pNode = pNavigationMesh->AppendNode(NavigationMeshNode(projP, pL->index, pCurrentTriangle));
				//Connect
				auto pNewConnection = pCurrentNode->AddConnection(pNode);
				pNewConnection->SetHeadConnection(pCurrentConnection);
				//Flag line
				pNavigationMesh->SetLineReviewed(pL->index);
			}
		}

		//--------------------------------------------------------------------------------------------
		//4. Calculate all the costs for the current connection
		auto vpConnections = pCurrentConnection->GetEndNode()->GetConnections();
		for (auto pC : vpConnections)
		{
			if (std::find(closedList.begin(), closedList.end(), pC) != closedList.end())
			{}  //do nothing
			else
			{
				if (std::find(openList.begin(), openList.end(), pC) == openList.end())
				{
					//Calculate costs
					CalculateCosts(pC, pStartNode, pEndNode, heuristicFunction);
					//Add it
					openList.push_back(pC);
				}
			}
		}
	}

	//--- DEBUG RENDERING
#ifdef _DEBUG
	if (sDrawDebugInfoQueries)
	{
		for (auto pN : pGraph->GetNodes())
		{
			for (auto pC : pN->GetConnections())
			{	
				DEBUGRENDERER2D->DrawPoint(pC->GetStartNode()->GetPosition(), 5.0f, { 0,1.f,1.f }, 0.3f);
				DEBUGRENDERER2D->DrawPoint(pC->GetEndNode()->GetPosition(), 5.0f, { 0,1.f,1.f }, 0.3f);
				DEBUGRENDERER2D->DrawSegment(pC->GetStartNode()->GetPosition(),
					pC->GetEndNode()->GetPosition(), { 0,1.f,1.f }, 0.3f);
			}
		}
	}
#endif

	//--------------- OPTIMIZE ---------------
	//Retrace the path and store this path in a container. Then optimize it using the funnel algorithm and
	//return the optimized path! 
	//--------------------------------------------
	std::vector<NavigationMeshNode*> vPathNodes;
	while (pCurrentConnection->GetStartNode() != pStartNode)
	{
		vPathNodes.push_back(static_cast<NavigationMeshNode*>(pCurrentConnection->GetEndNode()));
		pCurrentConnection = pCurrentConnection->GetHeadConnection();
	}
	vPathNodes.push_back(static_cast<NavigationMeshNode*>(pCurrentConnection->GetEndNode()));
	std::reverse(vPathNodes.begin(), vPathNodes.end());

	//Optimize our path using funnel algorithm!
	auto portals = FindPortals(pNavigationMesh, vPathNodes, pStartNode->GetPosition(), pEndNode->GetPosition());
	vPath = OptimizePortals(portals, pStartNode->GetPosition());
	vPath.push_back(vPathNodes[vPathNodes.size() - 1]->GetPosition()); //Push goal as part of the path
	return vPath;
}

//=== Private Pathfinder Functions ===
//--- References ---
//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
std::vector<Portal> NavigationMeshPathfinder::FindPortals(const NavigationMesh* pNavigationMesh,
	const std::vector<NavigationMeshNode*>& nodePath,
	const Elite::Vector2& startPos, const Elite::Vector2& endPos) const
{
	//Container
	std::vector<Portal> vPortals = {};

	//For each node received, get it's corresponding line
	const Triangle* pTriangle = nullptr;
	for (size_t i = 0; i < nodePath.size() - 1; ++i)
	{
		//Local variables
		auto pNode = nodePath[i]; //Store node, except last node, because this is our target node!
		auto pLine = pNavigationMesh->GetPolygon()->GetLines()[pNode->GetAssociatedLineIndex()];
		
		//Redetermine it's "orientation" based on the required path (left-right vs right-left) - p1 should be right point
		auto centerLine = (pLine->p1 + pLine->p2) / 2.0f;
		//Find the corresponding triangled based on node linking
		pTriangle = pNode->GetTraversedTriangle();
		auto centerTriangle = pTriangle->GetCenter();
		auto cp = Cross((centerLine - centerTriangle), (pLine->p1 - centerTriangle));
		Line portalLine = {};
		if (cp > 0)//Left
			portalLine = Line(pLine->p2, pLine->p1);
		else //Right
			portalLine = Line(pLine->p1, pLine->p2);

		//Store portal
		vPortals.push_back(Portal(portalLine));
	}
	//Add degenerate portal to force end evaluation
	vPortals.push_back(Portal(Line(endPos, startPos)));

#ifdef _DEBUG
	if (sDrawPortals)
	{
		for (auto portal : vPortals)
			DEBUGRENDERER2D->DrawSegment(portal.Line.p1, portal.Line.p2, Color(1.f, .5f, 0.f));
	}
#endif

	return vPortals;
}

std::vector<Elite::Vector2> NavigationMeshPathfinder::OptimizePortals(const std::vector<Portal>& portals, 
	const Elite::Vector2& startPos) const
{
	//P1 == right point of portal, P2 == left point of portal
	std::vector<Vector2> vPath = {};
	auto apex = startPos;
	auto apexIndex = 0, leftLegIndex = 0, rightLegIndex = 0;
	auto rightLeg = portals[leftLegIndex].Line.p1 - apex;
	auto leftLeg = portals[rightLegIndex].Line.p2 - apex;
	auto leftChanged = false;
	auto rightChanged = false;

	for (unsigned int i = 1; i < static_cast<unsigned int>(portals.size()); ++i)
	{
		//Locals
		auto portal = portals[i];
		leftChanged = false;
		rightChanged = false;

		//--- RIGHT CHECK ---
		//1. See if moving funnel inwards - RIGHT
		auto newRightLeg = portal.Line.p1 - apex;
		auto cpTightenFunnel = Cross(newRightLeg, rightLeg);
		if (cpTightenFunnel <= 0.0f) //Move inwards
		{
			//2. See if new line degenerates a line segment - RIGHT
			auto cpDenegrateFunnel = Cross(newRightLeg, leftLeg);
			if (cpDenegrateFunnel > 0.0f) //No overlap, tighten!
			{
				rightLeg = newRightLeg;
				rightLegIndex = i;
				rightChanged = true;
			}
			else
			{
				//Leftleg becomes new apex point
				apex += leftLeg;
				apexIndex = leftLegIndex;
				unsigned int newIt = apexIndex + 1;
				leftLegIndex = newIt;
				rightLegIndex = newIt;
				i = newIt;
				//Store point
				vPath.push_back(apex);
				//Calculate new legs (if not the end)
				if (newIt < static_cast<unsigned int>(portals.size()))
				{
					rightLeg = portals[rightLegIndex].Line.p1 - apex;
					leftLeg = portals[leftLegIndex].Line.p2 - apex;
					continue; //Restart
				}
			}
		}

		//--- LEFT CHECK ---
		//1. See if moving funnel inwards - LEFT
		auto newLeftLeg = portal.Line.p2 - apex;
		cpTightenFunnel = Cross(newLeftLeg, leftLeg);
		if (cpTightenFunnel >= 0.0f) //Move inwards
		{
			//2. See if new line degenerates a line segment - LEFT
			auto cpDenegrateFunnel = Cross(newLeftLeg, rightLeg);
			if (cpDenegrateFunnel < 0.0f) //No overlap, tighten!
			{
				leftLeg = newLeftLeg;
				leftLegIndex = i;
				leftChanged = true;
			}
			else
			{
				//Rightleg becomes new apex point
				apex += rightLeg;
				apexIndex = rightLegIndex;
				unsigned int newIt = apexIndex + 1;
				leftLegIndex = newIt;
				rightLegIndex = newIt;
				i = newIt;
				//Store point
				vPath.push_back(apex);
				//Calculate new legs (if not the end)
				if (newIt < static_cast<unsigned int>(portals.size()))
				{
					//Calculate new legs (if not the end)
					rightLeg = portals[rightLegIndex].Line.p1 - apex;
					leftLeg = portals[leftLegIndex].Line.p2 - apex;
				}
			}
		}
	}
	return vPath;
}