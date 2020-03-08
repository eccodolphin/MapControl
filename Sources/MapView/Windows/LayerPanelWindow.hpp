#pragma once

#include <Windows.h>
#include <windowsx.h>
#include "BaseWindow.hpp"

#include <exception>

#include "MapView.hpp"

namespace MapControl
{
	// Checkboxes numbers.
	const unsigned tileLayerCheckboxNumber = 1;
	const unsigned latLonGridLayerCheckboxNumber = 2;
	const unsigned userPointsLayerCheckboxNumber = 3;



	class LayerPanelWindow : public BaseWindow<LayerPanelWindow>
	{
	public:
		LayerPanelWindow() noexcept;
		LayerPanelWindow(MapView* pMapView, HWND mapViewWindowHwnd) noexcept;
	private:
		void _InitLabels() noexcept;
		void _InitCheckboxes() noexcept;

	public:
		LPCWSTR ClassName() const noexcept;
		
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

	private:
		MapView* _pMapView;
		HWND _mapViewWindowHwnd;
		
		LPCWSTR _className = L"Layer Panel Class";
	};
}