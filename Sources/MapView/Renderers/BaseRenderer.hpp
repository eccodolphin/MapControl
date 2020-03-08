#pragma once

#include "Windows.h"

#include <vector>

#include "Common.hpp"
#include "Geometry.hpp"

namespace MapControl
{
	const bool defRendererActivity = true;

	class BaseRenderer
	{
	public:
		BaseRenderer() noexcept;
		BaseRenderer(
			HWND*		mvHwnd,
			RectInt*	mvUiWindow,
			RectWM*		mvWmWindow,
			double*		mvScaleLat,
			double*		mvScaleLon,
			unsigned*	mvScaleLevel
			) noexcept;

		bool GetActivity() noexcept;
		void SetActivity(bool activity) noexcept;

	protected:
		PointInt _ScalePoint(const PointWM& unscaledPoint) const noexcept;
		SectInt _ScaleSect(const SectWM& unscaledSect) const noexcept;

	protected:
		bool _activity;

		HWND* _mvHwnd;

		RectInt*	_mvUiWindow;
		RectWM*		_mvWmWindow;
		double*		_mvScaleLat;
		double*		_mvScaleLon;
		unsigned*	_mvScaleLevel;
	};
}