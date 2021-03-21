#include "framework.h"

minstd_rand myrand;
POINT g_pt;
extern SIZE g_size;
extern ID2D1HwndRenderTarget* pRT;

struct circle {
	D2D1_POINT_2F pt;
	int r;
	D2D1_COLOR_F color;
	double scale;
};

class Circles {
public:
	static const int Len = 20;
	HWND hwnd;
	circle c[Len]{};
	int rmax;
	int rmin;
	ID2D1SolidColorBrush* brush;
	Circles(HWND hwn) {
		hwnd = hwn;
		brush = NULL;
	}

	void createBrush() {
		pRT->CreateSolidColorBrush(ColorF(ColorF::White), &brush);
	}

	~Circles() {
		if (brush) {
			brush->Release();
		}
	}

	void setWindow(HWND hwn) {
		hwnd = hwn;
	}

	void init() {
		LONG maxa;
		RECT rect;
		GetClientRect(hwnd, &rect);
		g_size.cx = rect.right;
		g_size.cy = rect.bottom;
		maxa = g_size.cx > g_size.cy ? g_size.cx : g_size.cy;
		rmax = maxa / 10;
		rmin = rmax / 10;
		for (int i = 0; i < Len; i++) {
			if (g_size.cx == 0) {
				c[i].pt.x = 0;
			}
			else {
				c[i].pt.x = rand() % g_size.cx;
			}
			if (g_size.cy == 0) {
				c[i].pt.y = 0;
			}
			else {
				c[i].pt.y = rand() % g_size.cy;
			}
			if (rmax - rmin == 0) {
				if (rmin != 0) {
					c[i].r = rmin;
				}
				else {
					c[i].r = 1;
				}
			}
			else {
				c[i].r = rand() % (rmax - rmin) + rmin;
			}
			c[i].color = ColorF(rand() / float(RAND_MAX + 1), rand() / float(RAND_MAX + 1), rand() / float(RAND_MAX + 1));
			c[i].scale = 1.0;
		}
	}

	void mouseOn() {
		double distance;
		for (int i = Len - 1; i >= 0; --i) {
			distance = hypot(g_pt.x - c[i].pt.x, g_pt.y - c[i].pt.y);
			if (distance < c[i].r) {
				c[i].scale = 2 * (1.0 - distance / c[i].r) + 1.0;
			}
			else {
				c[i].scale = 1.0;
			}
		}
	}

	void paint() {
		pRT->BeginDraw();
		pRT->Clear(ColorF(ColorF::Black));
		for (int i = 0; i < Len; i++) {
			brush->SetColor(c[i].color);
			pRT->FillEllipse(Ellipse(c[i].pt, c[i].r*c[i].scale, c[i].r*c[i].scale), brush);
		}
		pRT->EndDraw();
		SetWindowText(hwnd, L"");
		//SetWindowText(mwnd, wstring(L"By ºº·þ¹úÔÏ (").append(to_wstring(g_pt.x)).append(L",").append(to_wstring(g_pt.y)).append(L")").data());
	}
};

Circles cs(nullptr);

SIZE g_size;
ID2D1Factory* pD2DFactory = NULL;
ID2D1HwndRenderTarget* pRT = NULL;
HRESULT hr;


void OnPaint(HWND hwnd) {

	PAINTSTRUCT ps;
	BeginPaint(hwnd, &ps);

	cs.paint();
	EndPaint(hwnd, &ps);

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
	case WM_MOUSEMOVE:
	{
		g_pt.x = LOWORD(lParam);
		g_pt.y = HIWORD(lParam);
		cs.mouseOn();
		InvalidateRect(hwnd, NULL, false);
	}
	break;

	case WM_RBUTTONDOWN:
	{
		cs.init();
		InvalidateRect(hwnd, NULL, false);
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		OnPaint(hwnd);

		EndPaint(hwnd, &ps);
	}
	break;
	case WM_SIZE:
	{
		g_size.cx = LOWORD(lParam);
		g_size.cy = HIWORD(lParam);
		pRT->Resize(SizeU(g_size.cx, g_size.cy));
		cs.init();
		InvalidateRect(hwnd, nullptr, false);
	}
	break;

	case WM_CREATE:
	{
		// Create a Direct2D render target          
		hr = D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			&pD2DFactory
		);
		if (SUCCEEDED(hr)) {
			hr = pD2DFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(g_size.cy, g_size.cy)),
				&pRT
			);
		}
		else {
			MessageBox(hwnd, L"failed", L"", MB_OK);
		}
		cs.setWindow(hwnd);
		cs.createBrush();
		cs.init();
	}
	break;
	case WM_DESTROY: {
		if (pD2DFactory) {
			pD2DFactory->Release();
			pD2DFactory = NULL;
		}
		if (pRT) {
			pRT->Release();
			pRT = nullptr;
		}

		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR lpCmdLine, int nCmdShow) {

	WNDCLASSEX wc{};
	HWND hwnd;
	MSG msg;

	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);

	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = L"WindowClass";
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindow(L"WindowClass", L"Circles", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

