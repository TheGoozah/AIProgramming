/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ENavigationMeshPathfinder.h: A* Pathfinder implementation for Navigatoin Meshes using IPathfinder
/*=============================================================================*/
#ifndef ELITE_NAVIGATION_MESH_PATHFINDER
#define ELITE_NAVIGATION_MESH_PATHFINDER
namespace Elite
{
	//Portal struct (only contains line info atm, you can expand this if needed)
	struct Portal
	{
		Portal() {}
		explicit Portal(const Line& line) :
			Line(line)
		{}
		Line Line = {};
	};

	class NavigationMeshPathfinder final : public Pathfinder<NavigationMeshNode>
	{
	public:
		//--- Constructor & Destructor ---
		NavigationMeshPathfinder() :Pathfinder() {}
		~NavigationMeshPathfinder() = default; //Non virtual Destructor

		//--- Pathfinder Functions ---
		std::vector<Vector2> FindPath(
			Graph<NavigationMeshNode>* pGraph,
			NavigationMeshNode StartNode, NavigationMeshNode EndNode,
			Heuristic heuristicFunction);

		//--- DEBUG VARIABLES ---
#ifdef _DEBUG
		static bool sDrawDebugInfoQueries;
		static bool sDrawPortals;
#endif

	private:
		//--- Private Pathfinder Functions ---
		std::vector<Portal> FindPortals(const NavigationMesh* pNavigationMesh, 
			const std::vector<NavigationMeshNode*>& nodePath, 
			const Vector2& startPos, const Vector2& endPos) const;
		std::vector<Vector2> OptimizePortals(const std::vector<Portal>& portals, 
			const Vector2& startPos) const;
	};
}
#endif