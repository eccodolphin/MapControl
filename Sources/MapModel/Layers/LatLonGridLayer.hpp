#pragma once

#include "BaseLayer.hpp"

namespace MapControl
{
	class LatLonGridLayer : public BaseLayer
	{
	public:
		LatLonGridLayer() noexcept;
	private:
		void _InitLatLonGrid() noexcept;

	public:
		std::vector<SectWM> GetVisibleLatLonGrid(RectWM wmWindow) noexcept;

	private:
		std::vector<SectWM> _latLonGrid;
	};
}