#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <queue>
#include <list>

#include "lodepng.h"

#include "TileStructures.hpp"
#include "ITileCache.hpp"
#include "TileRequester.hpp"

namespace MapControl
{
	class TileCache : public ITileCache
	{
	// Constants section.
	private:
		const unsigned SlotsQuantity = 100;
		const unsigned CleanUpRest = 20;


	// Initialization section.
	public:
		TileCache(TileRequester& tileRequester) noexcept;
	private:
		void _InitSlots() noexcept;
		void _InitRequester() noexcept;


	// Getting section.
	public:
		std::vector<unsigned char> const* GetTileImage(const TileKey& key) noexcept;
	private:
		void _MakeTileRequest(const TileKey& tileKey) noexcept;
		TileSlot* GetPDirtySlot() noexcept;
		void AddPCleanSlot(TileSlot* pCleanSlot) noexcept;

		
	// Clearing section.
	public:
		void Clear() noexcept;
	private:
		void _CleanUp() noexcept;


	// Destroying section.
	public:
		~TileCache() noexcept;


	// State section.
	private:
		std::vector<TileSlot> _slots;
		std::queue<TileSlot*> _pDirtySlots;
		std::map<TileKey, TileSlot*> _pCleanSlots;

		TileRequester& _requester;
	};

	// Helpers section.
	bool SlotSort(const TileSlot* lhs, const TileSlot* rhs) noexcept;
	std::vector<TileSlot*> MapToVector(const std::map<TileKey, TileSlot*>& m) noexcept;
}
