
#ifndef CONFIGFILE_H_
#define CONFIGFILE_H_
#include <string>
#include "Location.h"

class ConfigFile {
 private:
	ConfigFile();
	virtual ~ConfigFile();


public:
	void ReadConfigFile();
	static ConfigFile* GetInstance();
	double	hamsterRadius;
	int  startX;
	int startY;
	double  startYAW;
	int goalX;
	int goalY;
	double  mapRes;

private:
	static ConfigFile *cnfFile;
	static bool instanceFlag;
};






#endif /* CONFIGFILE_H_ */
