#include "TileCache.hpp"

namespace MapControl
{
	// Initialization section.

	TileCache::TileCache(TileRequester& tileRequester) noexcept :
		_requester(tileRequester)
	{
		_InitSlots();
		_InitRequester();
	}

	void TileCache::_InitSlots() noexcept
	{
		_slots.resize(SlotsQuantity);
		for (TileSlot& tileSlot : _slots)
		{
			tileSlot.status = TileStatus::Dirty;
			_pDirtySlots.push(&tileSlot);
		}
	}

	void TileCache::_InitRequester() noexcept
	{
		_requester.SetPTileCache(this);
		_requester.StartRequesting();
	}



	// Getting section.

	std::vector<unsigned char> const* TileCache::GetTileImage(const TileKey& tileKey) noexcept
	{
		if (_pCleanSlots.find(tileKey) == _pCleanSlots.end())
		{
			_MakeTileRequest(tileKey);
			return nullptr;
		}

		_pCleanSlots[tileKey]->timestamp = GetCurrentTimestamp();
		return &(_pCleanSlots[tileKey]->image);
	}

	void TileCache::_MakeTileRequest(const TileKey& tileKey) noexcept
	{
		_requester.PushTileRequest(tileKey);
	}

	TileSlot* TileCache::GetPDirtySlot() noexcept
	{
		if (_pDirtySlots.empty())
		{
			_CleanUp();
		}

		TileSlot* pDirtySlot = _pDirtySlots.front();
		_pDirtySlots.pop();

		return pDirtySlot;
	}

	void TileCache::AddPCleanSlot(TileSlot* pCleanSlot) noexcept
	{
		_pCleanSlots.insert(std::make_pair(pCleanSlot->key, pCleanSlot));
	}



	// Clearing section.

	// Make all slots dirty.
	void TileCache::Clear() noexcept
	{
		_pCleanSlots.clear();
		for (TileSlot& tileSlot : _slots)
		{
			tileSlot.status = TileStatus::Dirty;
			_pDirtySlots.push(&tileSlot);
		}
	}

	// Leave the most recent used slots.
	void TileCache::_CleanUp()  noexcept
	{
		std::vector<TileSlot*> cleanSlots = MapToVector(_pCleanSlots);
		_pCleanSlots.clear();

		std::sort(cleanSlots.begin(), cleanSlots.end(), &SlotSort);

		while (_pCleanSlots.size() < CleanUpRest)
		{
			TileSlot* tileSlot = cleanSlots.back();
			cleanSlots.pop_back();

			_pCleanSlots.insert(std::make_pair(tileSlot->key, tileSlot));
		}

		for (TileSlot* tileSlot : cleanSlots)
		{
			tileSlot->status = TileStatus::Dirty;
			_pDirtySlots.push(tileSlot);
		}
	}



	// Destroying section.

	TileCache::~TileCache() noexcept
	{
		_requester.StopRequesting();
	}



	// Helpers section.

	bool SlotSort(const TileSlot* lhs, const TileSlot* rhs) noexcept
	{
		return lhs->timestamp < rhs->timestamp;
	};

	std::vector<TileSlot*> MapToVector(const std::map<TileKey, TileSlot*>& m) noexcept
	{
		std::vector<TileSlot*> v;
		for (auto it = m.begin(); it != m.end(); ++it)
		{
			v.push_back(it->second);
		}
		return v;
	}
}
