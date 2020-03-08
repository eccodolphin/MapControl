#pragma once

#include "BaseLayer.hpp"

namespace MapControl
{
	class UserPointsLayer : BaseLayer
	{
	public:
		UserPointsLayer() noexcept;
	private:
		void _InitUserPoints() noexcept;
	
	public:
		std::vector<PointWM> GetVisibleUserPoints(RectWM wmWindow) const noexcept;
		void AddUserPoint(const PointWM& newUserPoint) noexcept;

	private:
		std::vector<PointWM> _userPoints;
	};
}