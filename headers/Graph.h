#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>
#include "Node.h"
#include "Map.h"

class Map;

class Graph
{
	private:
		Map* mMap;
		std::vector<Node*>* mGridGraph;
		void CreateGridGraph();
		bool CompareNodesByPos(Node* a, Node* b);
		Node* FindNodeInVector(std::vector<Node*> container, int x, int y);
		Node* FindNodeInPriorityQueue(std::priority_queue<Node*, std::vector<Node*>, std::function<bool(Node*, Node*)> > container, int x, int y);
		Node* FindNodeInUnorderedMap(std::unordered_map<Node*, Node*> container, Node* nodeToFind);
		double HeuristicCostEstimate(Node* fromNode, Node* toNode);
		std::vector<Node*> ReconstructPath(std::unordered_map<Node*, Node*> cameFrom, Node* node);
		std::vector<Node*> AStar(Node* start, Node* goal);

	public:
		Graph(Map* map);
		~Graph();
		std::vector<Node*> CalculatePath(int nSrcX, int nSrcY, int nDstX, int nDstY);
};

#endif
