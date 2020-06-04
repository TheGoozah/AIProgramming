/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ENavigationMesh.h: graph that represents a navigation mesh 
/*=============================================================================*/
#ifndef ELITE_NAVIGATION_MESH
#define ELITE_NAVIGATION_MESH
namespace Elite
{
	class NavigationMeshBuilder;
	class NavigationMesh final : public Graph<NavigationMeshNode>
	{
	public:
		//--- Constructor & Destructor ---
		explicit NavigationMesh(const Polygon& rawMesh, unsigned int poolSize = 100) : Graph(poolSize)
		{ m_pNavMeshPolygon = new Polygon(rawMesh);	} //Make copy on heap
		~NavigationMesh() //Non virtual Destructor
		{ delete m_pNavMeshPolygon; m_pNavMeshPolygon = nullptr; }

		//--- Datamember Functions ---
		Polygon* GetPolygon() const { return m_pNavMeshPolygon; }
		const bool IsLineReviewed(int index) const { return m_ReviewedLines[index]; }
		void SetLineReviewed(int index) { m_ReviewedLines[index] = true; }

		//--- General Functions ---
#pragma region BasicFunctions
		void ResetNavigationMesh() //Reset base graph (clear connections pool) and reset reviewed lines
		{ 
			ClearGraph(); 
			for (auto b : m_ReviewedLines) b = false;	
		}

		//Get the closest triangle to this position
		const Triangle* GetClosestTriangle(const Vector2& position) const
		{
			auto closestDistanceSqrt = std::numeric_limits<float>::max();
			Triangle* pClosestTriangle = nullptr;
			for (auto pT : m_pNavMeshPolygon->GetTriangles())
			{
				auto currentDistanceSqrt = (pT->GetCenter() - position).SqrtMagnitude();
				if (currentDistanceSqrt < closestDistanceSqrt)
				{
					closestDistanceSqrt = currentDistanceSqrt;
					pClosestTriangle = pT;
				}
			}
			return pClosestTriangle;
		}

		//Get the triangle in which this position resides 
		const Triangle* GetCurrentTriangleFromPosition(const Vector2& position, bool onLineAllowed = false) const
		{
			for (auto pT : m_pNavMeshPolygon->GetTriangles())
			{
				auto inTriangle = PointInTriangle(position, pT->p1, pT->p2, pT->p3, onLineAllowed);
				if (inTriangle)
					return pT;
			}
			return nullptr;
		}

		//Get all triangles from that share this line, using the LineIndex for quick lookup
		const std::vector<const Triangle*> GetTrianglesFromLineIndex(unsigned int lineIndex) const
		{
			std::vector<const Triangle*> vpFoundTriangles = {};
			for (auto pT : m_pNavMeshPolygon->GetTriangles())
			{
				if (pT->metaData.IndexLine1 == lineIndex ||
					pT->metaData.IndexLine2 == lineIndex ||
					pT->metaData.IndexLine3 == lineIndex)
				{
					vpFoundTriangles.push_back(pT);
				}
			}
			return vpFoundTriangles;
		}

		//Get all the lines from the adjacent triangles, that are shared with this triangle.
		//In other words, the lines should be shared by the provided triangle and another triangle in the mesh
		const std::vector<const Line*> GetPlausibleLines(const Triangle* pTriangle)
		{
			std::vector<const Line*> lines = {};
			auto l1Count = 0; auto l2Count = 0; auto l3Count = 0;
			for (auto pT : m_pNavMeshPolygon->GetTriangles())
			{
				if (pTriangle->metaData.IndexLine1 == pT->metaData.IndexLine1
					|| pTriangle->metaData.IndexLine1 == pT->metaData.IndexLine2
					|| pTriangle->metaData.IndexLine1 == pT->metaData.IndexLine3)
					++l1Count;

				if (pTriangle->metaData.IndexLine2 == pT->metaData.IndexLine1
					|| pTriangle->metaData.IndexLine2 == pT->metaData.IndexLine2
					|| pTriangle->metaData.IndexLine2 == pT->metaData.IndexLine3)
					++l2Count;

				if (pTriangle->metaData.IndexLine3 == pT->metaData.IndexLine1
					|| pTriangle->metaData.IndexLine3 == pT->metaData.IndexLine2
					|| pTriangle->metaData.IndexLine3 == pT->metaData.IndexLine3)
					++l3Count;
			}

			if (l1Count >= 2 && m_ReviewedLines[pTriangle->metaData.IndexLine1] == false)
				lines.push_back(m_pNavMeshPolygon->GetLines()[pTriangle->metaData.IndexLine1]);
			if (l2Count >= 2 && m_ReviewedLines[pTriangle->metaData.IndexLine2] == false)
				lines.push_back(m_pNavMeshPolygon->GetLines()[pTriangle->metaData.IndexLine2]);
			if (l3Count >= 2 && m_ReviewedLines[pTriangle->metaData.IndexLine3] == false)
				lines.push_back(m_pNavMeshPolygon->GetLines()[pTriangle->metaData.IndexLine3]);

			return lines;
		}

#pragma endregion //GeneralFunctions

	private:
		//--- Friends Classes ---
		friend NavigationMeshBuilder;

		//--- Datamembers ---
		Polygon* m_pNavMeshPolygon = nullptr; //Polygon that represents navigation mesh
		std::vector<bool> m_ReviewedLines = {}; //Container to keep track of reviewed lines
	};
}
#endif