#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__
#include <unordered_map>

typedef struct
{
	int x;
	int y;
	int yaw;
} StartLocation;

typedef struct
{
	int x;
	int y;
} Goal;

typedef struct
{
	int width;
	int length;
} RobotSize;

class Configuration
{
	private:
		static Configuration* _instance;

		Configuration(const char* szFile);
		std::unordered_map<std::string, std::string> mConfigMap;
		StartLocation mStartLocation;
		Goal mGoal;
		RobotSize mRobotSize;
		double mMapResolutionCM;
		double mGridResolutionCM;
		std::string mMapFile;

		std::string ReadString(const char* szKey);
		int	ReadIntToken(const char* szKey, int nTokenNum);
		float ReadFloat(const char* szKey);

	public:
		static Configuration* Instance()
		{
			return (_instance);
		}

		static void Init(const char* szFile)
		{
			_instance = new Configuration(szFile);
		}

		StartLocation start()
		{
			return (this->mStartLocation);
		}

		Goal goal()
		{
			return (this->mGoal);
		}

		RobotSize robotSize()
		{
			return (this->mRobotSize);
		}

		double mapResolution()
		{
			return (this->mMapResolutionCM);
		}

		double gridResolution()
		{
			return (this->mGridResolutionCM);
		}

		std::string map()
		{
			return (this->mMapFile);
		}
};

#endif
