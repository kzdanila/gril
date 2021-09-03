#pragma once
#include <thread>
#include <condition_variable>
#include <fstream>
#include <map>
#include <atomic>
#include "Structures.h"
#include "Reciever.h"
#include "Constants.h"
#include <iostream>
#include <cmath>

class Matcher
{
public:
	Matcher();
	void InitRecivers();
	void CalculateRoutinine();

	~Matcher();

private:
	Reciever m_reciever1;
	Reciever m_reciever2;
	double Distance(const Structures::Epoch& epoch)
	{
		return std::sqrt((std::pow(epoch.pv2.coordinates.x - epoch.pv1.coordinates.x, 2))
			+ (std::pow(epoch.pv1.coordinates.y - epoch.pv1.coordinates.y, 2))
			+ (std::pow(epoch.pv1.coordinates.z - epoch.pv2.coordinates.z, 2)));
	}
	// sync variable && threads
	std::mutex m_recvLocker;
	std::condition_variable m_cv;
	std::thread m_firstReaderThread;
	std::thread m_secondReaderThread;
	std::thread m_computeThread; 

	std::atomic<bool> m_endFlag1;
	std::atomic<bool> m_endFlag2;
	Structures::Epoch m_currentEpoch;
	std::map<uint32_t, Structures::Epoch> m_currentEpochMap;
	
	//std::pair<Structures::PosVel, Structures::PosVel> m_mergedEpoch;
};

