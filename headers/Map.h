#ifndef __MAP_H__
#define __MAP_H__

#include <string>
#include "Graph.h"
#include "Configuration.h"

enum OFFSET { OFFSET_R, OFFSET_G, OFFSET_B, OFFSET_A };
enum CELL_TYPE { FREE_CELL = 0, OCCUPIED_CELL = 1, CLOSE_TO_WALL = 4, VERY_CLOSE_TO_WALL = 8 };

class Graph;

class Map
{
	private:
		// Common properties
		unsigned int mWidth;
		unsigned int mHeight;

		// Regular map
		int** mMap;

		// Inflated map
		int	  mInflateSize;
		int** mInflatedMap;

		// Grid (for A*)
		unsigned int mGridWidth;
		unsigned int mGridHeight;
		int** mGrid;

		// Configurations
		double mMapResolutionCM;
		double mGridResolutionCM;

		// Methods
		void ParseMapFile(std::string strFileName);
		void InflateMap();
		void CreateGrid();

	public:
		Map();
		virtual ~Map();
		void printMat(int** mat, int width, int height, std::string i, Configuration* config);

		int** map()
		{
			return (this->mMap);
		}

		int** inflatedMap()
		{
			return (this->mInflatedMap);
		}

		int mapWidth()
		{
			return (this->mWidth);
		}

		int mapHeight()
		{
			return (this->mHeight);
		}

		int** grid()
		{
			return (this->mGrid);
		}

		int gridHeight()
		{
			return (this->mGridHeight);
		}

		int gridWidth()
		{
			return (this->mGridWidth);
		}
};

#endif
