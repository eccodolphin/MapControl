#include "LayerPanelWindow.hpp"

namespace MapControl
{
	LayerPanelWindow::LayerPanelWindow() noexcept
	{
	}

	LayerPanelWindow::LayerPanelWindow(
		MapView* pMapView, 
		HWND mapViewWindowHwnd
		) 
		noexcept
		:
		_pMapView(pMapView),
		_mapViewWindowHwnd(mapViewWindowHwnd) 
	{
	}

	void LayerPanelWindow::_InitLabels() noexcept
	{
		LPCWSTR tileLayerLabelText = L"Show tile layer";
		LPCWSTR latLonGridLayerLabelText = L"Show lat/lon grid layer";
		LPCWSTR userPointsLayerLabelText = L"Show user points layer";

		CreateWindowEx(
			WS_EX_TRANSPARENT, L"STATIC", tileLayerLabelText,
			WS_CHILD | WS_VISIBLE | SS_LEFT | WS_SYSMENU,
			10, 10, 160, 18,
			m_hwnd, 
			NULL, NULL, NULL
			);
		CreateWindowEx(
			WS_EX_TRANSPARENT, L"STATIC", latLonGridLayerLabelText,
			WS_CHILD | WS_VISIBLE | SS_LEFT | WS_SYSMENU,
			10, 38, 160, 18,
			m_hwnd,
			NULL, NULL, NULL
			);
		CreateWindowEx(
			WS_EX_TRANSPARENT, L"STATIC", userPointsLayerLabelText,
			WS_CHILD | WS_VISIBLE | SS_LEFT | WS_SYSMENU,
			10, 66, 160, 18,
			m_hwnd,
			NULL, NULL, NULL
			);
	}

	void LayerPanelWindow::_InitCheckboxes() noexcept
	{
		CreateWindowEx(
			NULL, L"BUTTON", L"",
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
			180, 13, 14, 14,
			m_hwnd, (HMENU)tileLayerCheckboxNumber,
			NULL, NULL
			);
		CreateWindowEx(
			NULL, L"BUTTON", L"",
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
			180, 41, 14, 14,
			m_hwnd, (HMENU)latLonGridLayerCheckboxNumber,
			NULL, NULL
		);
		CreateWindowEx(
			NULL, L"BUTTON", L"",
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
			180, 69, 14, 14,
			m_hwnd, (HMENU)userPointsLayerCheckboxNumber,
			NULL, NULL
		);

		CheckDlgButton(m_hwnd, tileLayerCheckboxNumber, BST_CHECKED);
		CheckDlgButton(m_hwnd, latLonGridLayerCheckboxNumber, BST_CHECKED);
		CheckDlgButton(m_hwnd, userPointsLayerCheckboxNumber, BST_CHECKED);
	}



	LPCWSTR LayerPanelWindow::ClassName() const noexcept
	{
		return _className;
	}



	LRESULT LayerPanelWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
	{
		switch (uMsg)
		{
		case WM_CREATE:
			_InitLabels();
			_InitCheckboxes();
			return 0;

		case WM_COMMAND:
			switch (wParam)
			{
			// Tile layer checkbox checking.
			case tileLayerCheckboxNumber:
				if (IsDlgButtonChecked(m_hwnd, tileLayerCheckboxNumber))
				{
					CheckDlgButton(m_hwnd, tileLayerCheckboxNumber, BST_UNCHECKED);
					_pMapView->SetTileRendererActivity(false);
				}
				else
				{
					CheckDlgButton(m_hwnd, tileLayerCheckboxNumber, BST_CHECKED);
					_pMapView->SetTileRendererActivity(true);
				}
				InvalidateRect(_mapViewWindowHwnd, NULL, TRUE);
				break;

			// Lat/lon grid layer checkbox checking.
			case latLonGridLayerCheckboxNumber:
				if (IsDlgButtonChecked(m_hwnd, latLonGridLayerCheckboxNumber))
				{
					CheckDlgButton(m_hwnd, latLonGridLayerCheckboxNumber, BST_UNCHECKED);
					_pMapView->SetLatLonGridRendererActivity(false);
				}
				else
				{
					CheckDlgButton(m_hwnd, latLonGridLayerCheckboxNumber, BST_CHECKED);
					_pMapView->SetLatLonGridRendererActivity(true);
				}
				InvalidateRect(_mapViewWindowHwnd, NULL, TRUE);
				break;

			// User points layer checkbox checking.
			case userPointsLayerCheckboxNumber:
				if (IsDlgButtonChecked(m_hwnd, userPointsLayerCheckboxNumber))
				{
					CheckDlgButton(m_hwnd, userPointsLayerCheckboxNumber, BST_UNCHECKED);
					_pMapView->SetUserPointsRendererActivity(false);
				}
				else
				{
					CheckDlgButton(m_hwnd, userPointsLayerCheckboxNumber, BST_CHECKED);
					_pMapView->SetUserPointsRendererActivity(true);
				}
				InvalidateRect(_mapViewWindowHwnd, NULL, TRUE);
				break;
			}

			return 0;

		case WM_CLOSE:
			// Just hide. This window will be destroyed, when map view window will closing.
			ShowWindow(m_hwnd, SW_HIDE);
			return 0;

		default:
			return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
		}
		return TRUE;
	}
}