#include <fstream>
#include <iostream>
#include <algorithm>
#include "../headers/Map.h"
#include "../headers/lodepng.h"

using namespace std;

void Map::printMat(int** mat, int width, int height, string i, Configuration* config)
{
	fstream fs;
	fs.open((string("ok") + i + ".txt"), fstream::out);

	Graph* g;
	vector<Node*> V;
	if (i == "path"){
		g = new Graph(this);
		V = g->CalculatePath(config->start().x, config->start().y, config->goal().x, config->goal().y);
	}

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (i == "path")
			{
				bool isPath = false;
				for (int i = 0; i < V.size() - 1; i++)
				{
					if (V[i]->y() == y && V[i]->x() == x)
					{
						isPath = true;
						break;
					}
				}

				if (isPath)
				{
					fs << 7;
				}
				else
				{
					fs << mat[y][x];
				}
			}
			else
			{
				fs << mat[y][x];
			}
		}
		fs << endl;
	}
	if (i == "path"){
		V.empty();
	}

	fs.close();
}

Map::Map()
{
	Configuration* config = Configuration::Instance();

	// Read robot size
	int nRobotWidth = config->robotSize().width;
	int nRobotLength = config->robotSize().length;

	// Read more config and set data members
	this->mGridResolutionCM = config->gridResolution();
	this->mMapResolutionCM = config->mapResolution();
	this->mInflateSize = ceil(max(nRobotWidth, nRobotLength) / this->mMapResolutionCM / 2);

	// Parse the map file (from the configuration)
	this->ParseMapFile(config->map());

	printMat(this->mMap, this->mWidth, this->mHeight, "10map", config);

	// Process the map
	this->InflateMap();

	printMat(this->mInflatedMap, this->mWidth, this->mHeight ,"inflated", config);

	this->CreateGrid();

	printMat(this->mGrid, this->mGridWidth, this->mGridHeight, "grid", config);

	printMat(this->mGrid, this->mGridWidth, this->mGridHeight, "path", config);
}

Map::~Map()
{
	for (unsigned i = 0; i < this->mHeight; i++)
	{
		delete[] this->mMap[i];
	}

	delete[] this->mMap;

	for (unsigned i = 0; i < this->mHeight; i++)
	{
		delete[] this->mInflatedMap[i];
	}

	delete[] this->mInflatedMap;

	for (unsigned i = 0; i < this->mGridHeight; i++)
	{
		delete[] this->mGrid[i];
	}

	delete[] this->mGrid;
}

void Map::ParseMapFile(string strFileName)
{
	vector<unsigned char> vImage;

	// Read the map file
	unsigned error = lodepng::decode(vImage, this->mWidth, this->mHeight, strFileName);

	if (error)
	{
		cout << "Decoder error: " << error << ": " << lodepng_error_text(error) << endl;
		return;
	}

	// Initialize the map matrix
	this->mMap = new int*[this->mHeight];

	for (unsigned int i = 0; i < this->mHeight; i++)
	{
		this->mMap[i] = new int[this->mWidth];
	}

	// Fill the matrix
	// Every black pixel will become 1 and white pixel will become 0
	for (unsigned int y = 0; y < this->mHeight; y++)
	{
		for (unsigned int x = 0; x < this->mWidth; x++)
		{
			if (vImage[y * this->mWidth * 4 + x * 4 + OFFSET_R] ||
				vImage[y * this->mWidth * 4 + x * 4 + OFFSET_G] ||
				vImage[y * this->mWidth * 4 + x * 4 + OFFSET_B])
			{
				this->mMap[y][x] = FREE_CELL;
			}
			else
			{
				this->mMap[y][x] = OCCUPIED_CELL;
			}
		}
	}
}

void Map::InflateMap()
{
	// Create the inflated map matrix
	this->mInflatedMap = new int*[this->mHeight];

	for (int i = 0; i < this->mHeight; i++)
	{
		this->mInflatedMap[i] = new int[this->mWidth];

		for (int j = 0; j < this->mWidth; j++)
		{
			this->mInflatedMap[i][j] = FREE_CELL;
		}
	}

	// Inflate the map by adding cells next to the occupied cells by the half size of the robot
	for (int y = 0; y < this->mHeight; y++)
	{
		for (int x = 0; x < this->mWidth; x++)
		{
			if (this->mMap[y][x] == OCCUPIED_CELL)
			{
				for (int deltaY = (y - this->mInflateSize);
					deltaY <= (y + this->mInflateSize) && (deltaY < this->mHeight);
					deltaY++)
				{
					if (deltaY >= 0)
					{
						for (int deltaX = (x - this->mInflateSize);
							deltaX <= (x + this->mInflateSize) && (deltaX < this->mWidth);
							deltaX++)
						{
							if (deltaX >= 0)
							{
								this->mInflatedMap[deltaY][deltaX] = OCCUPIED_CELL;
							}
						}
					}
				}
			}
		}
	}
}

void Map::CreateGrid()
{
	int nRow;
	int nCol;
	bool bFoundOccupiedCell = false;
	int nGridResolution = ceil((float)this->mGridResolutionCM / this->mMapResolutionCM);
	this->mGridHeight = ceil((float)this->mHeight / nGridResolution);
	this->mGridWidth = ceil((float)this->mWidth / nGridResolution);

	// Create the grid matrix
	this->mGrid = new int*[this->mGridHeight];

	for (int i = 0; i < this->mGridHeight; i++)
	{
		this->mGrid[i] = new int[this->mGridWidth];
	}

	// Expand the cells to create the grid
	for (int y = 0; y < this->mHeight; y = y + nGridResolution)
	{
		for (int x = 0; x < this->mWidth; x = x + nGridResolution)
		{
			for (int deltaY = y;
				 deltaY < y + nGridResolution && deltaY < this->mHeight && !bFoundOccupiedCell;
				 deltaY++)
			{
				for (int deltaX = x;
					 deltaX < x + nGridResolution && deltaX < this->mWidth && !bFoundOccupiedCell;
					 deltaX++)
				{
					if (this->mInflatedMap[deltaY][deltaX] == OCCUPIED_CELL)
					{
						bFoundOccupiedCell = true;
					}
				}
			}

			// Calculate the cell number in the merged matrix
			nRow = y / nGridResolution;
			nCol = x / nGridResolution;

			// Check if we found an occupied cell
			if (bFoundOccupiedCell)
			{
				this->mGrid[nRow][nCol] = OCCUPIED_CELL;
			}
			else
			{
				this->mGrid[nRow][nCol] = FREE_CELL;
			}

			bFoundOccupiedCell = false;
		}
	}

	// For A*, we need the path to be a bit far from the walls to avoid collision
	// So wrap the occupied cells with the value VERY_CLOSE_TO_WALL
	for (int y = 0; y < this->mGridHeight; y++)
	{
		for (int x = 0; x < this->mGridWidth; x++)
		{
			if (this->mGrid[y][x] == OCCUPIED_CELL)
			{
				for (int dy = y - 1; dy <= y + 1; dy++)
				{
					for (int dx = x - 1; dx <= x + 1; dx++)
					{
						if (dy < 0 || dx < 0 || dy >= this->mGridHeight || dx >= this->mGridWidth)
						{
							continue;
						}

						if (this->mGrid[dy][dx] == FREE_CELL)
						{
							this->mGrid[dy][dx] = VERY_CLOSE_TO_WALL;
						}
					}
				}
			}
		}
	}

	// Now wrap the VERY_CLOSE_TO_WALL cells with CLOSE_TO_WALL
	for (int y = 0; y < this->mGridHeight; y++)
	{
		for (int x = 0; x < this->mGridWidth; x++)
		{
			if (this->mGrid[y][x] == VERY_CLOSE_TO_WALL)
			{
				for (int dy = y - 1; dy <= y + 1; dy++)
				{
					for (int dx = x - 1; dx <= x + 1; dx++)
					{
						if (dy < 0 || dx < 0 || dy >= this->mGridHeight || dx >= this->mGridWidth)
						{
							continue;
						}

						if (this->mGrid[dy][dx] == FREE_CELL)
						{
							this->mGrid[dy][dx] = CLOSE_TO_WALL;
						}
					}
				}
			}
		}
	}
}

