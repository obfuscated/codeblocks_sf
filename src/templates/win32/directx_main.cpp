#include <d3d8.h>

LPDIRECT3D8       g_pD3D       = NULL;
LPDIRECT3DDEVICE8 g_pD3DDevice = NULL;

HRESULT InitialiseD3D(HWND hWnd)
{
  // First of all, create the main D3D object. If it is created successfully
  // we should get a pointer to an IDirect3D8 interface.
  g_pD3D = Direct3DCreate8(D3D_SDK_VERSION);

  if(g_pD3D == NULL)
  {
    return E_FAIL;
  }

  //Get the current display mode
  D3DDISPLAYMODE d3ddm;
  if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
  {
    return E_FAIL;
  }

  // Create a structure to hold the settings for our device
  D3DPRESENT_PARAMETERS d3dpp;
  ZeroMemory(&d3dpp, sizeof(d3dpp));

  // Fill the structure: Program shall be windowed,
  // back buffer format matches current display mode
  d3dpp.Windowed         = TRUE;
  d3dpp.SwapEffect       = D3DSWAPEFFECT_COPY_VSYNC;
  d3dpp.BackBufferFormat = d3ddm.Format;

  //Create a Direct3D device.
  if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDevice)))
  {
    return E_FAIL;
  }

  return S_OK;
}

void Render()
{
  if(g_pD3DDevice == NULL)
  {
    return;
  }

  // Clear the backbuffer to blue
  g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET,
                      D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

  // Begin the scene
  g_pD3DDevice->BeginScene();

  // Fill in here the rendering of other objects

  // End the scene
  g_pD3DDevice->EndScene();

  // Fill back and front buffers so that back buffer will be visible on screen
  g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

void CleanUp()
{
  if(g_pD3DDevice != NULL)
  {
    g_pD3DDevice->Release();
    g_pD3DDevice = NULL;
  }

  if(g_pD3D != NULL)
  {
    g_pD3D->Release();
    g_pD3D = NULL;
  }
}

void MainLoop()
{
  // Enter the main loop
  MSG  msg;
  BOOL bMessage;

  PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

  while(msg.message != WM_QUIT)
  {
    bMessage = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);

    if(bMessage)
    {
      // Process message
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
      Render(); // No message to process -> render the scene
    }
  }// while
}

// The windows message handler
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
      break;

    case WM_KEYUP:
      switch (wParam)
      {
        case VK_ESCAPE:
          // Escape key pressed -> exit
          DestroyWindow(hWnd);
          return 0;
          break;
      }
      break;
  }// switch

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Application main entry point
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
  //Register the window class
  WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WinProc, 0L, 0L,
                    GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                    "DirectX Project", NULL };
  RegisterClassEx(&wc);

  // Create the application's main window
  HWND hWnd = CreateWindow("DirectX Project", "Code::Blocks Template",
                           WS_OVERLAPPEDWINDOW, 50, 50, 500, 500,
                           GetDesktopWindow(), NULL, wc.hInstance, NULL);

  // Initialize Direct3D
  if(SUCCEEDED(InitialiseD3D(hWnd)))
  {
    // Show window
    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);

    //Start game running: Enter the game loop
    MainLoop();
  }

  CleanUp();

  UnregisterClass("DirectX Project", wc.hInstance);

  return 0;
}
