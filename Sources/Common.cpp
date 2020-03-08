#include "Common.hpp"

int SafePow(int number, unsigned pow) noexcept
{
	int res = 1;
	for (unsigned i = 0; i < pow; ++i)
	{
		res *= number;
	}
	return res;
}

std::chrono::milliseconds GetCurrentTimestamp() noexcept
{
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}
