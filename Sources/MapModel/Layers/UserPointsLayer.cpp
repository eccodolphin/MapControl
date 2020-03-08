#include "UserPointsLayer.hpp"

namespace MapControl
{
	UserPointsLayer::UserPointsLayer() noexcept
	{
		_InitUserPoints();
	}

	void UserPointsLayer::_InitUserPoints() noexcept
	{
		_userPoints.push_back(PointWM{ 0.0, 0.0 });
	}



	std::vector<PointWM> UserPointsLayer::GetVisibleUserPoints(RectWM wmWindow) const noexcept
	{
		std::vector<PointWM> visibleUserPoints;
		for (const PointWM& userPoint : _userPoints)
		{
			if (IsVisible(userPoint, wmWindow))
			{
				visibleUserPoints.push_back(userPoint);
			}
		}
		return visibleUserPoints;
	}

	void UserPointsLayer::AddUserPoint(const PointWM& newUserPoint) noexcept
	{
		if (IsValid(newUserPoint))
		{
			_userPoints.push_back(newUserPoint);
		}
	}
}