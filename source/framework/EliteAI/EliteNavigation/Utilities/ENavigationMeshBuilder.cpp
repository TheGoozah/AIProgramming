//=== General Includes ===
#include "stdafx.h"
#include "ENavigationMeshBuilder.h"
using namespace Elite;

//=== NavigationMeshBuilder Functions ===
NavigationMesh* NavigationMeshBuilder::GenerateNavMesh(Elite::Polygon& baseMesh, float playerRadius, unsigned int nodePoolSize)
{
	//Get all shapes from all static rigidbodies with NavigationCollider flag
	auto vShapes = PHYSICSWORLD->GetAllStaticShapesInWorld(PhysicsFlags::NavigationCollider);
	
	//Create navigation mesh (memory ownership goes to user)
	auto pNavigationMesh = new NavigationMesh(baseMesh, nodePoolSize);

	//Store all children
	for (auto shape : vShapes)
	{
		shape.ExpandShape(playerRadius);
		pNavigationMesh->GetPolygon()->AddChild(shape);
	}

	//Triangulate
	pNavigationMesh->m_pNavMeshPolygon->Triangulate();
	//Resize reviewlines to match the amount of lines
	pNavigationMesh->m_ReviewedLines.resize(pNavigationMesh->m_pNavMeshPolygon->GetLines().size());

	return pNavigationMesh;
}