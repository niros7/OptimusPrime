#include "ConfigFile.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

ConfigFile::ConfigFile() {
	this->ReadConfigFile();
}

ConfigFile::~ConfigFile() {
	// TODO Auto-generated destructor stub
}

bool ConfigFile::instanceFlag = false;

ConfigFile* ConfigFile::cnfFile = NULL;

ConfigFile* ConfigFile::GetInstance()
{
    if(! instanceFlag)
    {
    	cnfFile = new ConfigFile();
        instanceFlag = true;
        return cnfFile;
    }
    else
    {
        return cnfFile;
    }
}

void ConfigFile::ReadConfigFile()
{
		string temp;


		ifstream fin("Configuration.cnf");
		string line;
		istringstream sin;

		// Getting the start location
		getline(fin, line);
		istringstream bb(line);
		bb >> temp >> this->startX >> this->startY >> this->startYAW;

		// Getting the goal
		getline(fin, line);
		istringstream cc(line);
		cc >> temp >> this->goalX >> this->goalY;

		// Getting the robot size
		getline(fin, line);
		istringstream dd(line);
		dd >> temp >> this->hamsterRadius;

		// Getting the map resolution
		getline(fin, line);
		istringstream ee(line);
		ee >> temp >> this->mapRes;
	}
