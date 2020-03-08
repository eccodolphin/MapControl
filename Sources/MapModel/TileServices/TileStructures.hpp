#pragma once

#include <vector>

#include "Common.hpp"

namespace MapControl
{
	typedef std::vector<unsigned char>	TileImage;
	typedef std::chrono::milliseconds	TileTimestamp;

	enum class TileStatus
	{
		Dirty,
		InProgress,
		Clean
	};

	struct TileKey
	{
		unsigned lod;
		unsigned x;
		unsigned y;
	};

	struct TileSlot
	{
		TileStatus status;
		TileKey key;
		TileImage image;
		TileTimestamp timestamp;
	};

	struct TilePointer
	{
		TileKey key;
		TileImage const * image_ptr;
	};

	bool operator<(const TileKey& lhs, const TileKey& rhs) noexcept;
}