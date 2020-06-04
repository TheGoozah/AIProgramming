/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ENavigationMeshBuilder.h: builder that accepts a polygon and rigidbody shapes,
// substracts the rigidbody shapes of polygon and finally triangulates the mesh.
// It only subtracts the shapes IF they are STATIC!
/*=============================================================================*/
#ifndef ELITE_NAVIGATION_MESH_BUILDER
#define ELITE_NAVIGATION_MESH_BUILDER
namespace Elite
{
	class NavigationMeshBuilder final
	{
	public:
		//--- Constructor & Destructor ---
		NavigationMeshBuilder() = default;
		~NavigationMeshBuilder() = default;

		//--- NavigationMeshBuilder Functions ---
		//This function gives ownership to the user, so he/she is responsable for the the destruction of the mesh
		NavigationMesh* GenerateNavMesh(Elite::Polygon& baseMesh, float playerRadius, unsigned int nodePoolSize);
	};
}
#endif