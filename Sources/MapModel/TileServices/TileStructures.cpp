#include "TileStructures.hpp"

namespace MapControl
{
	bool operator<(const TileKey& lhs, const TileKey& rhs) noexcept
	{
		if (lhs.lod != rhs.lod)
		{
			return lhs.lod < rhs.lod;
		}
		if (lhs.x != rhs.x)
		{
			return lhs.x < rhs.x;
		}
		return lhs.y < rhs.y;
	}
}
