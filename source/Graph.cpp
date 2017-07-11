#include <algorithm>
#include <queue>
#include <functional>
#include <iostream>
#include "../headers/Graph.h"

using namespace std;

Graph::Graph(Map* map)
{
	this->mMap = map;
	this->CreateGridGraph();
}

std::vector<Node*> Graph::CalculatePath(int nSrcX, int nSrcY, int nDstX, int nDstY)
{
	double fPixelToGrid =  Configuration::Instance()->gridResolution() / Configuration::Instance()->mapResolution();

	// Calculate start and goal nodes
	Node* start = FindNodeInVector((*this->mGridGraph), nSrcX / fPixelToGrid, nSrcY / fPixelToGrid);
	Node* goal = FindNodeInVector((*this->mGridGraph), nDstX / fPixelToGrid, nDstY / fPixelToGrid);

	// Run A* and get the result
	vector<Node*> result = this->AStar(start, goal);

	// Smoothen the result and remove unnecessary waypoints
	int dx = 0;
	int dy = 0;
	vector<Node*> smooth;

	// If we have a path, make it smooth
	if (result.size() > 0)
	{
		Node* last = result[0];

		for (int i = 1; i < result.size(); i++)
		{
			if (result[i] != NULL)
			{
				// Calculate the new deltas
				int dxNew = result[i]->x() - last->x();
				int dyNew = result[i]->y() - last->y();

				// If the deltas have changed, push the last into the vector
				if (dx != dxNew || dy != dyNew)
				{
					dx = dxNew;
					dy = dyNew;
					smooth.push_back(last);
				}

				last = result[i];
			}
		}

		// Push last to the smooth result to finish it
		smooth.push_back(last);
	}

	return (smooth);
}

void Graph::CreateGridGraph()
{
	// The list of all nodes that are free and have neighbours
	this->mGridGraph = new vector<Node*>();

	// Helper map
	Node*** graphMap = new Node**[this->mMap->gridHeight()];

	for (int i = 0; i < this->mMap->gridHeight(); i++)
	{
		graphMap[i] = new Node*[this->mMap->gridWidth()];
	}

	// Build the node objects for the free cells
	for (int y = 0; y < this->mMap->gridHeight(); y++)
	{
		for (int x = 0; x < this->mMap->gridWidth(); x++)
		{
			// If the current cell is free
			if (this->mMap->grid()[y][x] != OCCUPIED_CELL)
			{
				// Create a node object for it, add it to the graph list and map it
				Node* node = new Node(x, y);
				this->mGridGraph->push_back(node);
				graphMap[y][x] = node;
			}
		}
	}

	// Loop through the node objects (only free cells) and link their neighbours
	for (unsigned int i = 0; i < this->mGridGraph->size(); i++)
	{
		Node* current = this->mGridGraph->at(i);

		// Loop through the neighbours of the current node
		for (int yNeigh = current->y() - 1; yNeigh <= current->y() + 1; yNeigh++)
		{
			for (int xNeigh = current->x() - 1; xNeigh <= current->x() + 1; xNeigh++)
			{
				// Check if the neighbour's coords are out of bounds, or is the current node's coords
				if (yNeigh < 0 || xNeigh < 0 ||
					yNeigh >= this->mMap->gridHeight() || xNeigh >= this->mMap->gridWidth() ||
					(xNeigh == current->x() && yNeigh == current->y()))
				{
					continue;
				}

				// Check if the neighbour is free (= we can go to it)
				if (this->mMap->grid()[yNeigh][xNeigh] != OCCUPIED_CELL)
				{
					Node* neighbour = graphMap[yNeigh][xNeigh];
					current->neighbours()->push_back(neighbour);
				}
			}
		}
	}

	// Free helper graph map
	for (int i = 0; i < this->mMap->gridHeight(); i++)
	{
		delete[] graphMap[i];
	}

	delete[] graphMap;
}

bool Graph::CompareNodesByPos(Node* a, Node* b)
{
	return (a->x() == b->x() && a->y() == b->y());
}

Node* Graph::FindNodeInVector(vector<Node*> container, int x, int y)
{
	for (unsigned i = 0; i < container.size(); i++)
	{
		if (container[i]->x() == x && container[i]->y() == y)
		{
			return container[i];
		}
	}

	return NULL;
}

Node* Graph::FindNodeInPriorityQueue(priority_queue<Node*, vector<Node*>, function<bool(Node*, Node*)> > container, int x, int y)
{
	int size = container.size();

	for (int i = 0; i < size; i++)
	{
		if (container.top()->x() == x && container.top()->y() == y)
		{
			return container.top();
		}

		container.pop();
	}

	return NULL;
}

Node* Graph::FindNodeInUnorderedMap(unordered_map<Node*, Node*> container, Node* nodeToFind)
{
	for (unordered_map<Node*, Node*>::iterator it = container.begin();
	     it != container.end(); ++it)
	{
		if (CompareNodesByPos(it->first, nodeToFind))
		{
			return it->second;
		}
	}

	return NULL;
}

double Graph::HeuristicCostEstimate(Node* fromNode, Node* toNode)
{
	// Return the distance between the two nodes
	return sqrt(pow(toNode->x() - fromNode->x(), 2) + pow(toNode->y() - fromNode->y(), 2));
}

vector<Node*> Graph::ReconstructPath(unordered_map<Node*, Node*> cameFrom, Node* node)
{
	vector<Node*> vTotalPath;

	while (node != NULL)
	{
		node = FindNodeInUnorderedMap(cameFrom, node);
		vTotalPath.push_back(node);
	}

	return vTotalPath;
}

bool CompareNodesByScore(Node* firstNode, Node* secondNode)
{
	return (firstNode->fscore() > secondNode->fscore());
}

vector<Node*> Graph::AStar(Node* start, Node* goal)
{
	vector<Node*> closedSet;
	unordered_map<Node*, Node*> cameFrom;
	Node* currentNode = NULL;
	Node* neighbor = NULL;
	vector<Node*>* neighbours = NULL;

	// Initialize the queue with the function that will be the comparison of him
	priority_queue<Node*, vector<Node*>, function<bool(Node*, Node*)> > openSet(CompareNodesByScore);
	openSet.push(start);

	// Setting the cost from start along best known path
	start->gscore(0);

	// Estimated total cost from start to goal
	start->fscore(start->gscore() + HeuristicCostEstimate(start, goal));

	while (openSet.size() != 0)
	{
		currentNode = openSet.top();

		if (CompareNodesByPos(currentNode, goal))
		{
			return ReconstructPath(cameFrom, goal);
		}

		openSet.pop();

		closedSet.push_back(currentNode);

		neighbours = currentNode->neighbours();

		for (unsigned i = 0; i < neighbours->size(); i++)
		{
			neighbor = (*neighbours)[i];

			if (FindNodeInVector(closedSet, neighbor->x(), neighbor->y()) != NULL)
			{
				continue;
			}

			int nScore = this->mMap->grid()[currentNode->y()][currentNode->x()];

			if (nScore == 0)
			{
				nScore = 1;
			}

			double fTentativeGScore = currentNode->gscore() + nScore; // TODO: change the 1 to the weight between current and neighbor

			bool bIsFoundInQueue = (FindNodeInPriorityQueue(openSet, neighbor->x(), neighbor->y()) != NULL);

			if (!bIsFoundInQueue || fTentativeGScore < neighbor->gscore())
			{
				cameFrom.insert(pair<Node*, Node*>(neighbor, currentNode));

				neighbor->gscore(fTentativeGScore);
				neighbor->fscore(neighbor->gscore() + HeuristicCostEstimate(neighbor, goal));

				// Fix for priority queue
				// If there are elements in the priority queue, need to rebuild the heap
				// This is due to invalid heap error caused when the priority of the nodes change
				// Rebuilding the heap fixes the error
				if (openSet.size() > 0)
				{
					make_heap(const_cast<Node**>(&openSet.top()), const_cast<Node**>(&openSet.top()) + openSet.size(), CompareNodesByScore);
				}

				if (!bIsFoundInQueue)
				{
					openSet.push(neighbor);
				}
			}
		}
	}

	throw "A* failure";
}
