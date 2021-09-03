#include "Reciever.h"
#include <iostream>
#include <Windows.h>
#include <synchapi.h>

Reciever::Reciever(int _srcNumber) : m_srcNumber(_srcNumber)
{

}

void Reciever::Recv(const std::filesystem::path& _source, std::atomic<bool>& _readyFlag, std::condition_variable& _cv,
	std::mutex& _locker, std::map<uint32_t, Structures::Epoch>& _mergedStorage)
{
	std::ifstream input(_source, std::ios::binary);
	if (!input.is_open())
	{
		_readyFlag.store(true);
		return;
	}
	std::pair<uint32_t, Structures::PosVel> epoch;
	while (!input.eof())
	{
		Structures::Msg msg;
		input.read(reinterpret_cast<char*>(&msg), sizeof(Structures::Msg));
		if (std::string(msg.msgId).find(constants::MessageNames::RECIEVER_TIME) != std::string::npos)
		{
			Structures::RcvTime* rcv = new Structures::RcvTime;
			input.read(reinterpret_cast<char*>(rcv), std::stoi(msg.size, 0, 16));
			epoch.first = rcv->tod;
			delete rcv;
		}
		else if (std::string(msg.msgId).find(constants::MessageNames::POSITION_VELOCITY) != std::string::npos)
		{
			Structures::PosVel* pv = new Structures::PosVel;
			input.read(reinterpret_cast<char*>(pv), std::stoi(msg.size, 0, 16));
			epoch.second = *pv;

			InsertToStorage(epoch, _cv, _locker, _mergedStorage);
			delete pv;
		}
		else if (std::string(msg.msgId).find(constants::MessageNames::EPOCH_END) != std::string::npos)
		{
			Structures::EpochEnd* ee = new Structures::EpochEnd;
			input.read(reinterpret_cast<char*>(ee), std::stoi(msg.size, 0, 16));
			delete ee;
		}
		else
		{
			char* str = new char[std::stoi(msg.size, 0, 16)];
			input.read(str, std::stoi(msg.size, 0, 16));
			delete[] str;
		}
		while (input.get() != '\n' && input)
			continue;
	}
	_readyFlag.store(true);
}

void Reciever::InsertToStorage(const std::pair<uint32_t, Structures::PosVel> _newEpoch, 
	std::condition_variable& _cv, std::mutex& _locker, std::map<uint32_t, Structures::Epoch>& _mergedStorage)
{

	std::unique_lock<std::mutex> lk(_locker);
	auto it = _mergedStorage.find(_newEpoch.first);
	if (it == _mergedStorage.end())
	{
		Structures::Epoch e;
		if (m_srcNumber == 0)
		{
			e.pv1 = _newEpoch.second;
			e.src1Set = true;
		}
		else
		{
			e.pv2 = _newEpoch.second;
			e.src2Set = true;
		}
		_mergedStorage.emplace(_newEpoch.first, e);
		lk.unlock();
	}
	else
	{
		if (it->second.src1Set)
		{
			it->second.pv2 = _newEpoch.second;
			it->second.src2Set = true;
		}
		else
		{
			it->second.pv1 = _newEpoch.second;
			it->second.src1Set = true;
		}
		lk.unlock();
		std::unique_lock ul(_locker);
		_cv.wait(ul,
			[&]
			{
				return it->second.isReady();
			}
		);
		_cv.notify_all();
		ul.unlock();
	}
}