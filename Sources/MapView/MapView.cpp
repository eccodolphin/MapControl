#include "MapView.hpp"

namespace MapControl
{
	MapView::MapView(MapModel& mapModel) noexcept :
		_mapModel(mapModel)
	{
		_SetDefaults();
		_InitRenderers();
	}

	void MapView::_SetDefaults() noexcept
	{
		_uiWindow = RectInt{
			PointInt{ 0, 0 },
			mvDefW,
			mvDefH
		};

		_scaleLevel = wmMinScaleLevel;

		// Map h/w in pixels in currnet scale level.
		unsigned wmHPix = static_cast<unsigned>(wmTileHPix * std::pow(2.0, _scaleLevel));
		unsigned wmWPix = static_cast<unsigned>(wmTileWPix * std::pow(2.0, _scaleLevel));

		// Set scale.
		_scaleLat = static_cast<double>(wmHPix) / wmFullHDgs;
		_scaleLon = static_cast<double>(wmWPix) / wmFullWDgs;

		// Set origin and extrem.
		double originLat = wmMinLat - (static_cast<double>(static_cast<int>(_uiWindow.h) - wmHPix) / 2.0) / _scaleLat;
		double originLon = wmMinLon - (static_cast<double>(static_cast<int>(_uiWindow.w) - wmWPix) / 2.0) / _scaleLon;
		_wmWindow.origin = PointWM{ originLat, originLon };
		_RefreshExtrem();

		_necessaryToClearScreen = _IsNecessaryToClearScreen();

		_ResizeTransparentBitmapPixels(mvDefW, mvDefH);
	}

	void MapView::_InitRenderers() noexcept
	{
		_tileRenderer = TileRenderer(
			&_mapModel.GetTileLayer(),
			&_hWnd,
			&_uiWindow,
			&_wmWindow,
			&_scaleLat,
			&_scaleLon,
			&_scaleLevel
			);

		_latLonGridRenderer = LatLonGridRenderer(
			&_mapModel.GetLatLonGridLayer(),
			&_hWnd,
			&_uiWindow,
			&_wmWindow,
			&_scaleLat,
			&_scaleLon,
			&_scaleLevel
		);

		_userPointsRenderer = UserPointsRenderer(
			&_mapModel.GetUserPointsLayer(),
			&_hWnd,
			&_uiWindow,
			&_wmWindow,
			&_scaleLat,
			&_scaleLon,
			&_scaleLevel
		);
	}

	void MapView::SetHwnd(HWND hWnd) noexcept
	{
		_hWnd = hWnd;
	}



	void MapView::_RefreshExtrem() noexcept
	{
		double extremeLat = _wmWindow.origin.lat + static_cast<double>(_uiWindow.h) / _scaleLat;
		double extremeLon = _wmWindow.origin.lon + static_cast<double>(_uiWindow.w) / _scaleLon;
		_wmWindow.extrem = PointWM{ extremeLat, extremeLon };
	}

	bool MapView::_IsNecessaryToClearScreen() noexcept
	{
		return !IsValid(_wmWindow.origin) || !IsValid(_wmWindow.extrem);
	}



	void MapView::Redraw()
	{
		PAINTSTRUCT wPs;
		HDC wHdc = BeginPaint(_hWnd, &wPs);

		// Form frame image.

		HBITMAP hFrameBitmap = CreateBitmap(
			_uiWindow.w,
			_uiWindow.h,
			bmpDefPlanes,
			bmpDefBitCount,
			&_transparentBitmapPixels[0]
		);

		HDC frameBitmapHdc = CreateCompatibleDC(NULL);
		HBITMAP oldFrameBitmap = static_cast<HBITMAP>(SelectObject(frameBitmapHdc, hFrameBitmap));

		if (_tileRenderer.GetActivity())
		{
			HBITMAP hTileLayerBitmap = _tileRenderer.GetTileLayerBitmap(_transparentBitmapPixels);
			_TransparentBlt(frameBitmapHdc, hTileLayerBitmap);
			DeleteObject(hTileLayerBitmap);
		}

		if (_latLonGridRenderer.GetActivity())
		{
			HBITMAP hLlgLayerBitmap = _latLonGridRenderer.GetLatLonGridLayerBitmap(_transparentBitmapPixels);
			_TransparentBlt(frameBitmapHdc, hLlgLayerBitmap);
			DeleteObject(hLlgLayerBitmap);
		}

		if (_userPointsRenderer.GetActivity())
		{
			HBITMAP hUpLayerBitmap = _userPointsRenderer.GetUserPointsLayerBitmap(_transparentBitmapPixels);
			_TransparentBlt(frameBitmapHdc, hUpLayerBitmap);
			DeleteObject(hUpLayerBitmap);
		}

		SelectObject(frameBitmapHdc, oldFrameBitmap);
		DeleteObject(frameBitmapHdc);

		// Draw frame image.

		if (!_tileRenderer.GetActivity())
		{
			_necessaryToClearScreen = true;
		}
		if (_necessaryToClearScreen)
		{
			_ClearScreen(wHdc, wPs);
			_necessaryToClearScreen = false;
		}

		_TransparentBlt(wHdc, hFrameBitmap);

		DeleteObject(hFrameBitmap);
		EndPaint(_hWnd, &wPs);
	}

	void MapView::_ClearScreen(HDC wHdc, PAINTSTRUCT& wPs) noexcept
	{
		FillRect(wHdc, &wPs.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
	}

	void MapView::_TransparentBlt(HDC hdc, HBITMAP hBitmap) noexcept
	{
		// Get bitmap info.
		BITMAP layerBitmapInfo;
		GetObject(hBitmap, sizeof(BITMAP), &layerBitmapInfo);

		// Create bitmap HDC.
		HDC layerBitmapHdc = CreateCompatibleDC(hdc);
		HGDIOBJ oldBitmap = SelectObject(layerBitmapHdc, hBitmap);


		// Draw layer.
		TransparentBlt(
			hdc, 0, 0, _uiWindow.w, _uiWindow.h,
			layerBitmapHdc, 0, 0, layerBitmapInfo.bmWidth, layerBitmapInfo.bmHeight,
			transparentColor
		);

		// Clear bitmap HDC.
		SelectObject(layerBitmapHdc, oldBitmap);
		DeleteDC(layerBitmapHdc);
	}



	void MapView::AddUserPoint(const PointInt& scaledNewUserPoint) noexcept
	{
		PointWM unscaledNewUserPoint = _UnscalePoint(scaledNewUserPoint);
		_mapModel.GetUserPointsLayer().AddUserPoint(unscaledNewUserPoint);
	}

	PointWM MapView::_UnscalePoint(const PointInt& scaledPoint) const noexcept
	{
		double unscaledPointLat = (
			_wmWindow.origin.lat + static_cast<double>(scaledPoint.y) / _scaleLat
			);
		double unscaledPointLon = (
			_wmWindow.origin.lon + static_cast<double>(scaledPoint.x) / _scaleLon
			);
		return PointWM{ unscaledPointLat, unscaledPointLon };
	}



	void MapView::MoveLeft() noexcept
	{
		// Count step and move if possible.
		double moveStepLon = (static_cast<double>(_uiWindow.w) / _scaleLon) * mvMoveFactor;
		PointWM futureOrigin{ _wmWindow.origin.lat, _wmWindow.origin.lon - moveStepLon };
		if (SafeLessOrEqual(wmMinLon, futureOrigin.lon))
		{
			_wmWindow.origin = futureOrigin;
			_RefreshExtrem();
		}
	}

	void MapView::MoveRight() noexcept
	{
		// Count step and move if possible.
		double moveStepLon = (static_cast<double>(_uiWindow.w) / _scaleLon) * mvMoveFactor;
		PointWM futureExtrem{ _wmWindow.extrem.lat, _wmWindow.extrem.lon + moveStepLon };
		if (SafeLessOrEqual(futureExtrem.lon, wmMaxLon))
		{
			_wmWindow.origin = PointWM{ _wmWindow.origin.lat, _wmWindow.origin.lon + moveStepLon };
			_RefreshExtrem();
		}
	}

	void MapView::MoveUp() noexcept
	{
		// Count step and move if possible.
		double moveStepLat = (static_cast<double>(_uiWindow.h) / _scaleLat) * mvMoveFactor;
		PointWM futureOrigin{ _wmWindow.origin.lat - moveStepLat, _wmWindow.origin.lon };
		if (SafeLessOrEqual(wmMinLat, futureOrigin.lat))
		{
			_wmWindow.origin = futureOrigin;
			_RefreshExtrem();
		}
	}

	void MapView::MoveDown() noexcept
	{
		// Count step and move if possible.
		double moveStepLat = (static_cast<double>(_uiWindow.h) / _scaleLat) * mvMoveFactor;
		PointWM futureExtreme{ _wmWindow.extrem.lat + moveStepLat, _wmWindow.extrem.lon };
		if (SafeLessOrEqual(futureExtreme.lat, wmMaxLat))
		{
			_wmWindow.origin = PointWM{ _wmWindow.origin.lat + moveStepLat, _wmWindow.origin.lon };
			_RefreshExtrem();
		}
	}

	void MapView::ZoomIn() noexcept
	{
		// Check possiblity of zooming.
		int futureScaleLevel = static_cast<int>(_scaleLevel) + 1;
		if (futureScaleLevel > static_cast<int>(wmMaxScaleLevel))
		{
			return;
		}

		// Calculate position of future origin.
		double curUnscaledH = _wmWindow.extrem.lat - _wmWindow.origin.lat;
		double curUnscaledW = _wmWindow.extrem.lon - _wmWindow.origin.lon;
		double futUnscaledH = curUnscaledH / mvScaleFactor;
		double futUnscaledW = curUnscaledW / mvScaleFactor;
		double originOffsetLat = (curUnscaledH - futUnscaledH) / 2.0;
		double originOffsetLon = (curUnscaledW - futUnscaledW) / 2.0;
		_wmWindow.origin = PointWM{ 
			_wmWindow.origin.lat + originOffsetLat,  
			_wmWindow.origin.lon + originOffsetLon 
		};

		_scaleLat *= mvScaleFactor;
		_scaleLon *= mvScaleFactor;
		
		_RefreshExtrem();

		_scaleLevel = static_cast<unsigned>(futureScaleLevel);
	}

	void MapView::ZoomOut() noexcept
	{
		// Check possiblity of zooming.
		int futureScaleLevel = static_cast<int>(_scaleLevel) - 1;
		if (futureScaleLevel < static_cast<int>(wmMinScaleLevel))
		{
			return;
		}

		// Calculate position of future origin.
		double curUnscaledH = _wmWindow.extrem.lat - _wmWindow.origin.lat;
		double curUnscaledW = _wmWindow.extrem.lon - _wmWindow.origin.lon;
		double futUnscaledH = curUnscaledH * mvScaleFactor;
		double futUnscaledW = curUnscaledW * mvScaleFactor;
		double originOffsetLat = (futUnscaledH - curUnscaledH) / 2.0;
		double originOffsetLon = (futUnscaledW - curUnscaledW) / 2.0;
		_wmWindow.origin = PointWM{ 
			_wmWindow.origin.lat - originOffsetLat, 
			_wmWindow.origin.lon - originOffsetLon 
		};

		_scaleLat /= mvScaleFactor;
		_scaleLon /= mvScaleFactor;

		_RefreshExtrem();

		_scaleLevel = static_cast<unsigned>(futureScaleLevel);

		// Could get out from screen borders, so, stabilize.
		_CentralizeIfNecessary();
		_necessaryToClearScreen = _IsNecessaryToClearScreen();
	}

	void MapView::_CentralizeIfNecessary() noexcept
	{
		// Calculate window unscaled h/w.
		double curUnscaledH = _wmWindow.extrem.lat - _wmWindow.origin.lat;
		double curUnscaledW = _wmWindow.extrem.lon - _wmWindow.origin.lon;

		// Compare with Web Mercator unscaled h/w and centrize if it is smaller than window unscaled h/w.
		if (curUnscaledH > wmFullHDgs)
		{
			double difUnscaledH = curUnscaledH - wmFullHDgs;
			_wmWindow.origin = PointWM{ wmMinLat - difUnscaledH / 2.0, _wmWindow.origin.lon };
			_RefreshExtrem();
		}
		if (curUnscaledW > wmFullWDgs)
		{
			double difUnscaledW = curUnscaledW - wmFullWDgs;
			_wmWindow.origin = PointWM{ _wmWindow.origin.lat, wmMinLon - difUnscaledW / 2.0 };
			_RefreshExtrem();
		}
	}



	void MapView::ResizeWindow(unsigned wNewW, unsigned wNewH) noexcept
	{
		_RecalcOriginWhenResize(
			static_cast<int>(_uiWindow.w), 
			static_cast<int>(_uiWindow.h),
			static_cast<int>(wNewW),
			static_cast<int>(wNewH)
			);

		_ResizeTransparentBitmapPixels(wNewW, wNewH);
		
		_uiWindow.w = wNewW;
		_uiWindow.h = wNewH;
		
		_RefreshExtrem();

		_CentralizeIfNecessary();

		_necessaryToClearScreen = _IsNecessaryToClearScreen();
	}

	void MapView::_RecalcOriginWhenResize(int wOldW, int wOldH, int wNewW, int wNewH) noexcept
	{
		// Calculate deltas in pixels.
		int wDeltaW = wOldW - wNewW;
		int wDeltaH = wOldH - wNewH;

		// Calculate deltas in degrees.
		double oDeltaLon = (static_cast<double>(wDeltaW) / _scaleLon) / 2.0;
		double oDeltaLat = (static_cast<double>(wDeltaH) / _scaleLat) / 2.0;

		_wmWindow.origin = PointWM{ _wmWindow.origin.lat + oDeltaLat, _wmWindow.origin.lon + oDeltaLon };

		_necessaryToClearScreen = _IsNecessaryToClearScreen();
	}

	void MapView::_ResizeTransparentBitmapPixels(unsigned wNewW, unsigned wNewH) noexcept
	{
		unsigned requiredTransparentBitmapSize = wNewW * wNewH;
		_transparentBitmapPixels.resize(requiredTransparentBitmapSize, transparentColor);
	}



	void MapView::SetTileRendererActivity(bool activity) noexcept
	{
		_tileRenderer.SetActivity(activity);
	}

	void MapView::SetLatLonGridRendererActivity(bool activity) noexcept
	{
		_latLonGridRenderer.SetActivity(activity);
	}

	void MapView::SetUserPointsRendererActivity(bool activity) noexcept
	{
		_userPointsRenderer.SetActivity(activity);
	}
}