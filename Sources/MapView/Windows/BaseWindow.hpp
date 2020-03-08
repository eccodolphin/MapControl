#pragma once

#include <Windows.h>

template <class DERIVED_TYPE>
class BaseWindow
{
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	BaseWindow();

	BOOL Create(
		LPCWSTR lpWindowName,
		DWORD dwStyle,
		int nWidth = CW_USEDEFAULT,
		int nHeight = CW_USEDEFAULT,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		DWORD dwExStyle = 0,
		HWND hWndParent = 0,
		HMENU hMenu = 0
	);

	HWND Window() const;

protected:
	HWND m_hwnd;

	virtual LPCWSTR ClassName() const = 0;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
};

template <class DERIVED_TYPE>
LRESULT CALLBACK BaseWindow<DERIVED_TYPE>::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DERIVED_TYPE *pThis = NULL;

	if (uMsg == WM_NCCREATE)
	{
		// Connect pointer of window data to self object.
		CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
		pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
		pThis->m_hwnd = hwnd;
	}
	else
	{
		// Get pointer to window associated data.
		pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}
	if (pThis)
	{
		// Call intire handle procedure.
		return pThis->HandleMessage(uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

template <class DERIVED_TYPE>
BaseWindow<DERIVED_TYPE>::BaseWindow() :
	m_hwnd{ NULL }
{}

// Initialize and registarate the new window class.
template <class DERIVED_TYPE>
BOOL BaseWindow<DERIVED_TYPE>::Create(
	LPCWSTR lpWindowName,
	DWORD dwStyle,
	int nWidth = CW_USEDEFAULT,
	int nHeight = CW_USEDEFAULT,
	int x = CW_USEDEFAULT,
	int y = CW_USEDEFAULT,
	DWORD dwExStyle = 0,
	HWND hWndParent = 0,
	HMENU hMenu = 0
)
{
	WNDCLASS wc = { 0 };

	wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = ClassName();
	wc.style = CS_DBLCLKS; // Allow to handle double clicks.

	RegisterClass(&wc);

	m_hwnd = CreateWindowEx(
		dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
		nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
		);

	return (m_hwnd ? TRUE : FALSE);
}

template <class DERIVED_TYPE>
HWND BaseWindow<DERIVED_TYPE>::Window() const
{
	return m_hwnd;
}