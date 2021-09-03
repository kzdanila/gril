#pragma once
#include <string>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <map>

#include "Structures.h"
#include "Constants.h"

class Reciever
{
public:
	Reciever(int _srcNumber);

	void Recv(const std::filesystem::path& _source, std::atomic<bool>& _readyFlag, std::condition_variable& _cv
		, std::mutex& _locker, std::map<uint32_t, Structures::Epoch>& _mergedStorage);
	void InsertToStorage(const std::pair<uint32_t, Structures::PosVel> _newEpoch, 
		std::condition_variable& _cv, std::mutex& _locker, std::map<uint32_t, Structures::Epoch>& _mergedStorage);
private:
	int m_srcNumber;
};

