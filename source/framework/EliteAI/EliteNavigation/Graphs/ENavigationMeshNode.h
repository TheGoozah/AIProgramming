/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ENavigationMeshNode.h: navigationmesh node representation,
// that holds the traversed triangle and the associated line index 
/*=============================================================================*/
#ifndef ELITE_NAVIGATION_MESH_NODE
#define ELITE_NAVIGATION_MESH_NODE

//--- Includes ---
#include "ENode.h"

namespace Elite
{
	class NavigationMeshNode final : public Node
	{
	public:
		//--- Constructor & Destructor ---
		NavigationMeshNode() = default;
		explicit NavigationMeshNode(float x, float y, int lineIndex, const Triangle* pTraversedTriangle) :
			Node(x, y), m_LineIndex(lineIndex), m_pTraversedTriangle(pTraversedTriangle) {}
		explicit NavigationMeshNode(const Vector2& pos, int lineIndex, const Triangle* pTraversedTriangle) :
			Node(pos), m_LineIndex(lineIndex), m_pTraversedTriangle(pTraversedTriangle) {}
		~NavigationMeshNode() = default; //Non virtual Destructor

		//--- Datamember Functions ---
		const int GetAssociatedLineIndex() const { return m_LineIndex; }
		const Triangle* GetTraversedTriangle() const { return m_pTraversedTriangle; }
		void SetTraversedTriangle(const Triangle* pT) { m_pTraversedTriangle = pT; }

	private:
		//--- Datamembers ---
		int m_LineIndex = -1; //Associated line, used for optimizalization (= less intersection calculations)
		const Triangle* m_pTraversedTriangle = nullptr; //Used to keep track of triangle that was used to get to this node
	};
}
#endif