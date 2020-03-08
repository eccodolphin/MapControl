#include "MapModel.hpp"

namespace MapControl
{
	MapModel::MapModel(TileRequester& tileRequester, std::mutex& tileServicesMutex) noexcept :
		_tileLayer(tileRequester, tileServicesMutex)
	{
	}



	TileLayer& MapModel::GetTileLayer() noexcept
	{
		return _tileLayer;
	}

	LatLonGridLayer& MapModel::GetLatLonGridLayer() noexcept
	{
		return _latLonGridLayer;
	}

	UserPointsLayer& MapModel::GetUserPointsLayer() noexcept
	{
		return _userPointsLayer;
	}
}