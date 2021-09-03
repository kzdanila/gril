#pragma once
#include <string>

namespace constants
{
	namespace MessageNames
	{
		const std::string POSITION_VELOCITY = "PV";
		const std::string RECIEVER_TIME = "~~";
		const std::string EPOCH_END = "||";
	}

	namespace geiodetic
	{
		constexpr double a = 6378137;
		constexpr double f = 0.0034;
		constexpr double b = 6.3568e6;
		constexpr double e = Math.sqrt((Math.pow(a, 2) - Math.pow(b, 2)) / Math.pow(a, 2));
		constexpr double e2 = Math.sqrt((Math.pow(a, 2) - Math.pow(b, 2)) / Math.pow(b, 2));

	}
}
