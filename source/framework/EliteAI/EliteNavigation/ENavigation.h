/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ENavigation.h: General Navigation header that includes all navigation pathfinders and utilities
/*=============================================================================*/
#ifndef ELITE_NAVIGATION
#define ELITE_NAVIGATION

/* --- TYPE DEFINES --- */
//Function pointer type to heuristic function
typedef float(*Heuristic)(float, float);

/* --- UTILITIES --- */
//Utilities
#include "EHeuristicFunctions.h"
//Graphs
#include "Graphs/EConnections.h"
#include "Graphs/ENode.h"
#include "Graphs/EGraph.h"
#include "Graphs/ENavigationMeshNode.h"
#include "Graphs/ENavigationMesh.h"
//Pathfinders
#include "Pathfinders/EPathfinder.h"
#include "Pathfinders/ENavigationMeshPathfinder.h"
//Utilities
#include "Utilities/ENavigationMeshBuilder.h"
#endif