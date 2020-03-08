#pragma once

#include <Windows.h>

#include <cmath>
#include <vector>

#include "Common.hpp"
#include "Geometry.hpp"

#include "MapModel.hpp"

#include "TileRenderer.hpp"
#include "LatLonGridRenderer.hpp"
#include "UserPointsRenderer.hpp"

namespace MapControl
{
	// Map View parametrs.

	const unsigned mvDefW = 800; // Pixels.
	const unsigned mvDefH = 600;
	const unsigned mvMinW = 128;
	const unsigned mvMinH = 128;

	const double mvScaleFactor = 2.0;
	const double mvMoveFactor = 0.01; // % (0.01 = 1%).

	const unsigned transparentColor = 0x00000000;



	class MapView
	{

	// Initialization section.
	public:
		MapView(MapModel& mapModel) noexcept;
	private:
		void _SetDefaults() noexcept;
		void _InitRenderers() noexcept;
	public:
		void SetHwnd(HWND hWnd) noexcept;


	// Common helpers section.
	private:
		void _RefreshExtrem() noexcept;
		bool _IsNecessaryToClearScreen() noexcept;


	// Drawing section.
	public:
		void Redraw();
	private:
		void _ClearScreen(HDC wHdc, PAINTSTRUCT& ps) noexcept;
		void _TransparentBlt(HDC hdc, HBITMAP hBitmap) noexcept;


	// Adding user points section.
	public:
		void AddUserPoint(const PointInt& newUserPoint) noexcept;
	private:
		PointWM _UnscalePoint(const PointInt& scaledPoint) const noexcept;


	// Moving section.
	public:
		void MoveLeft() noexcept;
		void MoveRight() noexcept;
		void MoveUp() noexcept;
		void MoveDown() noexcept;
		void ZoomIn() noexcept;
		void ZoomOut() noexcept;
	private:
		void _CentralizeIfNecessary() noexcept;


	// Window resizing section.
	public:
		void ResizeWindow(unsigned wNewW, unsigned wNewH) noexcept;
	private:
		void _RecalcOriginWhenResize(int wOldW, int wOldH, int wNewW, int wNewH) noexcept;
		void _ResizeTransparentBitmapPixels(unsigned w, unsigned h) noexcept;


	// Layers on/off secton.
	public:
		void SetTileRendererActivity(bool activity) noexcept;
		void SetLatLonGridRendererActivity(bool activity) noexcept;
		void SetUserPointsRendererActivity(bool activity) noexcept;


	// State section.
	private:
		MapModel& _mapModel;

		HWND _hWnd;

		RectInt		_uiWindow;		// Pixels.
		RectWM		_wmWindow;		// Map units (degrees).
		double		_scaleLat;		// Pixels per map unit (degree).
		double		_scaleLon;
		unsigned	_scaleLevel;

		TileRenderer _tileRenderer;
		LatLonGridRenderer _latLonGridRenderer;
		UserPointsRenderer _userPointsRenderer;

		bool _necessaryToClearScreen;

		std::vector<unsigned> _transparentBitmapPixels;
	};
}
