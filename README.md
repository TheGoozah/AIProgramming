# AIProgramming
AI/Gameplay Programming Elite Framework integrates several AI techniques. The framework is still **"Under Development"** and is meant to be cross-platfom (supporting **PC**, **PlayStation 4** and **Nintendo Switch**). The framework is optimized using several techniques and guidelines (**Memory Pools** focusing on Data Oriented Programming, **Template Specialization** to mimic pure virtual interfaces at compile time, **avoiding virtual functions**, etc.).

## Interesting files
- [Vector2](source/framework/EliteMath/EVector2.h) & [Matrix2x2](source/framework/EliteMath/EMat22.h): custom mathematics implementation.
- [Geometric Utilities](source/framework/EliteGeometry/EGeometry2DUtilities.h#L22): geometric utilities to do line intersections, line projections, find convex corners, determining winding, etc.
- [Triangulator](source/framework/EliteGeometry/EGeometry2DTypes.cpp#L227): triangulator using the **ear-clipping algorithm** to triangulate the custom polygon implementation.
- [Engine Helpers](source/framework/EliteHelpers): several helpers used in the engine. These are: an **expandable memory pool**, a **singleton** and a **multicast delegate** that accepts function pointers that have diffirent signatures and that invokes them.
- [A* Navigation Mesh Pathfinder](source/framework/EliteAI/EliteNavigation/Pathfinders/ENavigationMeshPathfinder.cpp#L13): an custom A* pathfinding implementation for 2D Navigation Meshes. This implementation uses a **runtime query** and the **funnel algorithm** to determine the best possible path.
- [Behavior Tree](source/framework/EliteAI/EliteBehaviorTree/EBehaviorTree.h#L44): an Behavior Tree implementation using a **Blackboard** to accept and store different types of information. All basic modules of the Behavior Tree (Composites, Actions, etc.) are provided.
- [Finite State Machine](source/framework/EliteAI/EliteFiniteStateMachine/EFiniteStateMachine.h#L25): an implementation of a Finite State Machine using the Multicast Delegate. This allows **binding of lambda's, functors and functions pointers, with different signatures and data**. Because of this, behaviors can be coded to be **reusable**.
