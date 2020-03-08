#include "BaseRenderer.hpp"

namespace MapControl
{
	BaseRenderer::BaseRenderer() noexcept :
		_activity(defRendererActivity)
	{
	}

	BaseRenderer::BaseRenderer(
		HWND*		mvHwnd,
		RectInt*	mvUiWindow,
		RectWM*		mvWmWindow,
		double*		mvScaleLat,
		double*		mvScaleLon,
		unsigned*	mvScaleLevel
		) 
		noexcept 
		:
		_activity(defRendererActivity),
		_mvHwnd(mvHwnd),
		_mvUiWindow(mvUiWindow),
		_mvWmWindow(mvWmWindow),
		_mvScaleLat(mvScaleLat),
		_mvScaleLon(mvScaleLon),
		_mvScaleLevel(mvScaleLevel)
	{
	}


	bool BaseRenderer::GetActivity() noexcept
	{
		return _activity;
	}

	void BaseRenderer::SetActivity(bool activity) noexcept
	{
		_activity = activity;
	}



	PointInt BaseRenderer::_ScalePoint(const PointWM& unscaledPoint) const noexcept
	{
		int scaledPointX = static_cast<int>(
			(unscaledPoint.lon - _mvWmWindow->origin.lon) * (*_mvScaleLon)
			);
		int scaledPointY = static_cast<int>(
			(unscaledPoint.lat - _mvWmWindow->origin.lat) * (*_mvScaleLat)
			);
		return PointInt{ scaledPointX, scaledPointY };
	}

	SectInt BaseRenderer::_ScaleSect(const SectWM& wmSect) const noexcept
	{
		PointInt scaledP1 = _ScalePoint(wmSect.p1);
		PointInt scaledP2 = _ScalePoint(wmSect.p2);
		return SectInt{ scaledP1, scaledP2 };
	}
}