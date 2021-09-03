#pragma once
#include <cstdint>
#include <unordered_map>

namespace Structures
{
	using u1 = uint8_t;
	using u4 = uint32_t;
	using f4 = float;
	using f8 = double;

	
	struct Msg
	{
		char msgId[2];
		char size[3];
	};

	struct RcvTime
	{
		u4 tod = 0; // Time representation modulo 1 day (86400000 ms) [ms]
		u1 cs = 0; // checksum
	};

	struct CartesianCoordinates
	{
		f8 x = 0;
		f8 y = 0;
		f8 z = 0;
		f4 pSigma = 0; // Position SEP
	};

	struct CartesianVelocities
	{
		f4 vx = 0;
		f4 vy = 0;
		f4 vz = 0;
		f4 vSigma = 0; // Velocity SEP
	};

	struct PosVel
	{
		CartesianCoordinates coordinates; // [m]
		CartesianVelocities velocities; // [m/s]
		u1 solType = 0; // solution type
		u1 cs = 0; // checksum
	};

	struct EpochEnd
	{
		u1 cs = 0; //checksum
	};

	struct Epoch
	{
		PosVel pv1;
		PosVel pv2;
		bool src1Set = false;
		bool src2Set = false;

		bool isReady() { return src1Set && src2Set; };
	};

	using RecieverLocalEpochStorage = std::unordered_map<uint32_t, PosVel>;
	//using EpochQue = std::deque<MergedEpoch>;
}