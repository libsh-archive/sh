#include <windows.h>
#include <d3dx9.h>

// ** Global Constants **
#define Deg2Rad(x) x * D3DX_PI / 180
#define FPS 60
#define TIMER_EVENT 0x2000
#define XROT_FACTOR Deg2Rad(1)
#define YROT_FACTOR Deg2Rad(1)
#define ZROT_FACTOR Deg2Rad(0)
#define ROTATE_FRAMES 1
#define NORM 0.57735026918962576450914878050196f

// ** Global Variables **
HINSTANCE g_hInstance; // App's instance handle (probably not needed)
HWND g_hWnd; // App's window
char *g_szAppName = "DirectX Rotating Cube Native Phong Shader Demo"; // App name for window captions, etc.
UINT_PTR g_pTimer; // Timer for rendering

// ** DirectX Globals **
LPDIRECT3D9 g_pD3D = NULL; // Direct3D Main Interface
LPDIRECT3DDEVICE9 g_pD3DDevice = NULL; // Direct3D Device
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // Vertex buffer
LPDIRECT3DINDEXBUFFER9 g_pIB = NULL; // Index buffer
LPDIRECT3DTEXTURE9 g_pTexture = NULL; // Texture for the cube
LPDIRECT3DVERTEXSHADER9 g_pVS = NULL; // Compiled vertex shader
LPDIRECT3DPIXELSHADER9 g_pPS = NULL; // Compiled pixel shader

// ** Function Prototypes **
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void cleanup(void);
bool initGeometry(void);
bool initDirectX(HWND hWnd);
void renderFrame(void);
void setupMatrices(void);

// DirectX vertex format
struct XYZC_VERTEX
{
    FLOAT x, y, z; // Vertex position
	FLOAT nx, ny, nz; // Vertex normal
	DWORD diffuse; // Diffuse colour
	DWORD specular; // Specular colour
	FLOAT u, v; // Texture coordinates
};

// ***** Shader input *****

// Light position
float g_pVShaderLightPos[] =
{
	 0.5f, 1.0f, 5.0f, // Light pos
};

// Eye position
float g_pVShaderEyePos[] =
{
	0.0f, 0.5f,-3.0f, 0.0f, // Eye pos
};

// Light material constants
float g_pVShaderMaterials[] =
{
 	 1.0f, 1.0f, 1.0f, 0.0f, // Diffuse
	 1.0f, 1.0f, 1.0f, 0.0f, // Specular
	 1.0f, 1.0f, 1.0f, 0.0f, // Emissive
 	50.0f, 50.0f, 50.0f, 50.0f, // Specular power?
};

// Actual vertex shader
const char szVertexShader[] =
"vs.1.1\n"\
"\n"\
"//c0 = Full Transformation\n"\
"//c4 = World Transformation\n"\
"//c20 = Light Position\n"\
"//c23 = Diffuse Material\n"\
"//c24 = Specular Material\n"\
"//c25 = Emissive Material\n"\
"//c26 = Specular Power = 50\n"\
"//c27 = Eye Position\n"\
"\n"\
"//-----declarations-------------\n"\
"dcl_position v0\n"\
"dcl_normal v3\n"\
"dcl_texcoord0 v7\n"\
"\n"\
"//-----transformations----------\n"\
"m4x4 oPos, v0, c0 //full transform\n"\
"m4x4 r5, v0, c4     //world transform\n"\
"m3x3 r9, v3, c4     //rotate normals\n"\
"mov oT0, v7\n"\
"\n"\
"//---light vector\n"\
"sub r0,c20,r5\n"\
"dp3 r0.w,r0,r0  //normalize\n"\
"rsq r0.w,r0.w\n"\
"mul r0, r0, r0.w\n"\
"\n"\
"//----view vector\n"\
"sub r1,c27,r5\n"\
"dp3 r1.w,r1,r1  //normalize\n"\
"rsq r1.w,r1.w\n"\
"mul r1, r1, r1.w\n"\
"\n"\
"//----diffuse Illumination---------\n"\
"\n"\
"dp3 r2.x, r9,r0  //diffuse product\n"\
"\n"\
"//----specular illumination\n"\
"add r3, r0, r1\n"\
"\n"\
"dp3 r3.w,r3,r3  //normalize\n"\
"rsq r3.w,r3.w\n"\
"mul r3, r3, r3.w\n"\
"\n"\
"dp3 r2.y,r9,r3      //specular product\n"\
"mov r2.w,c26.w   //get power\n"\
"\n"\
"lit r4,r2    //process lighting\n"\
"\n"\
"//----combine colors\n"\
"mov r0,c25            //emissive color\n"\
"mad oD0,c23,r4.y,r0   //diffuse color\n"\
"mul oD1,c24,r4.z   //specular color\n";

// Actual pixel shader
const char szPixelShader[] =
"ps.1.1\n"\
"def c0, 1.0f, 1.0f, 1.0f, 1.0f\n"\
"\n"\
"tex t0\n"\
"mul r1,v1,c0\n"\
"mad r0,t0,v0,r1\n";


// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_XYZC_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL | D3DFVF_SPECULAR | D3DFVF_TEX1)


// Main: where it all begins
APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;

	// Set up and register a window class
	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = g_hInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "DirectXDemoMainWindow";
	RegisterClassEx(&wc);

	// Now, we can make a window
	g_hWnd = CreateWindowEx(0, "DirectXDemoMainWindow", g_szAppName, WS_POPUP | WS_CAPTION | WS_SYSMENU, 0, 0, 800, 600, NULL, NULL, g_hInstance, NULL);
	if (!g_hWnd)
	{
		::MessageBox(NULL, "Couldn't create window", g_szAppName, MB_ICONSTOP);
		return 0;
	}

	// Try to initialize DirectX and the geometry
	if (!initDirectX(g_hWnd))
		return 0;

	if (!initGeometry())
		return 0;

	// Display the window and force the first refresh
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	// Set up a timer for rendering
	g_pTimer = SetTimer(g_hWnd, TIMER_EVENT, 1000 / FPS, NULL);

	// Message pump: read and dispatch messages until the user ends the app
	MSG msg;
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
				return msg.wParam;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

// Window Procedure: handle and dispatch basic window events
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
			cleanup();
			PostQuitMessage(0);
			return 0;
		case WM_TIMER:
			switch (wParam)
			{
			case TIMER_EVENT:
				renderFrame();
				break;
			default:
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
			return 0;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

// DirectX initialization code
bool initDirectX(HWND hWnd)
{
    // Create the D3D object.
    if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return false;

    // Seet up back buffers of the same format as the display mode
    D3DDISPLAYMODE d3ddm;
    if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
        return false;

    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format;

    // Create the D3DDevice
    if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDevice)))
        return false;

	// Turn off culling
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Turn off D3D lighting, since we are providing our own vertex colors
    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    // Turn on the zbuffer
    g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

    return true;
}

// Initialize the geometry for the cube
bool initGeometry(void)
{
    // Initialize vertices for rendering a cube
    XYZC_VERTEX aVertices[] =
    {
        { -0.5f,-0.5f,-0.5f, -NORM,-NORM,-NORM, 0xff0000ff, 0xffffffff, 0.0f, 0.0f }, // Front face
		{ -0.5f, 0.5f,-0.5f, -NORM, NORM,-NORM, 0xff0000ff, 0xffffffff, 0.0f, 1.0f },
		{  0.5f, 0.5f,-0.5f,  NORM, NORM,-NORM, 0xff0000ff, 0xffffffff, 1.0f, 1.0f },
		{  0.5f,-0.5f,-0.5f,  NORM,-NORM,-NORM, 0xff0000ff, 0xffffffff, 1.0f, 0.0f },
		{ -0.5f,-0.5f, 0.5f, -NORM,-NORM, NORM, 0xff00ff00, 0xffffffff, 0.0f, 0.0f }, // Left face
		{ -0.5f, 0.5f, 0.5f, -NORM, NORM, NORM, 0xff00ff00, 0xffffffff, 0.0f, 1.0f },
		{ -0.5f, 0.5f,-0.5f, -NORM, NORM,-NORM, 0xff00ff00, 0xffffffff, 1.0f, 1.0f },
		{ -0.5f,-0.5f,-0.5f, -NORM,-NORM,-NORM, 0xff00ff00, 0xffffffff, 1.0f, 0.0f },
		{ -0.5f, 0.5f, 0.5f, -NORM, NORM, NORM, 0xffff0000, 0xffffffff, 0.0f, 0.0f }, // Back face
		{ -0.5f,-0.5f, 0.5f, -NORM,-NORM, NORM, 0xffff0000, 0xffffffff, 0.0f, 1.0f },
        {  0.5f,-0.5f, 0.5f,  NORM,-NORM, NORM, 0xffff0000, 0xffffffff, 1.0f, 1.0f },
		{  0.5f, 0.5f, 0.5f,  NORM, NORM, NORM, 0xffff0000, 0xffffffff, 1.0f, 0.0f },
		{  0.5f,-0.5f,-0.5f,  NORM,-NORM,-NORM, 0xffffff00, 0xffffffff, 0.0f, 0.0f }, // Right face
		{  0.5f, 0.5f,-0.5f,  NORM, NORM,-NORM, 0xffffff00, 0xffffffff, 0.0f, 1.0f },
		{  0.5f, 0.5f, 0.5f,  NORM, NORM, NORM, 0xffffff00, 0xffffffff, 1.0f, 1.0f },
		{  0.5f,-0.5f, 0.5f,  NORM,-NORM, NORM, 0xffffff00, 0xffffffff, 1.0f, 0.0f },
		{ -0.5f, 0.5f, 0.5f, -NORM, NORM, NORM, 0xff00ffff, 0xffffffff, 0.0f, 0.0f }, // Top face
		{ -0.5f, 0.5f,-0.5f, -NORM, NORM,-NORM, 0xff00ffff, 0xffffffff, 0.0f, 1.0f },
		{  0.5f, 0.5f,-0.5f,  NORM, NORM,-NORM, 0xff00ffff, 0xffffffff, 1.0f, 1.0f },
		{  0.5f, 0.5f, 0.5f,  NORM, NORM, NORM, 0xff00ffff, 0xffffffff, 1.0f, 0.0f },
		{ -0.5f,-0.5f, 0.5f, -NORM,-NORM, NORM, 0xffff00ff, 0xffffffff, 0.0f, 0.0f }, // Bottom face
		{ -0.5f,-0.5f,-0.5f, -NORM,-NORM,-NORM, 0xffff00ff, 0xffffffff, 0.0f, 1.0f },
		{  0.5f,-0.5f,-0.5f,  NORM,-NORM,-NORM, 0xffff00ff, 0xffffffff, 1.0f, 1.0f },
		{  0.5f,-0.5f, 0.5f,  NORM,-NORM, NORM, 0xffff00ff, 0xffffffff, 1.0f, 0.0f },
    };

	// Initialize indices for rendering a cube
	WORD aIndices[] =
	{
		0, 1, 3, // Front face
		1, 2, 3,
		4, 5, 7, // Left face
		5, 6, 7,
		8, 9, 11, // Back face
		9, 10, 11,
		12, 13, 15, // Right face
		13, 14, 15,
		16, 17, 19, // Top face
		17, 18, 19,
		20, 21, 23, // Bottom face
		21, 22, 23,
	};

    // Load a bitmap from a file
    if (FAILED(D3DXCreateTextureFromFile(g_pD3DDevice, "env2.bmp", &g_pTexture)))
		return false;

	// Create the vertex buffer
    if (FAILED(g_pD3DDevice->CreateVertexBuffer(24 * sizeof(XYZC_VERTEX), 0, D3DFVF_XYZC_VERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
		return false;

	// Create the index buffer
	if (FAILED(g_pD3DDevice->CreateIndexBuffer(36 * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL)))
		return false;

	// Fill the vertex buffer by locking, copying data, and unlocking
    void* pVertices;
    if (FAILED(g_pVB->Lock(0, sizeof(aVertices), (void**)&pVertices, 0)))
        return false;
    memcpy(pVertices, aVertices, sizeof(aVertices));
    g_pVB->Unlock();

	// Fill the index buffer by locking, copying data, and unlocking
	void *pIndices;
	if (FAILED(g_pIB->Lock(0, sizeof(aIndices), (void **)&pIndices, 0)))
		return false;
	memcpy(pIndices, aIndices, sizeof(aIndices));
	g_pIB->Unlock();

	// Compile the vertex shader
	LPD3DXBUFFER pErrors, pVSBuffer;
	if (FAILED(D3DXAssembleShader(szVertexShader, sizeof(szVertexShader) - 1, NULL, NULL, 0, &pVSBuffer, &pErrors)))
	{
		::MessageBox(NULL, "Couldn't compile vertex shader; errors: \n", g_szAppName, MB_ICONSTOP);
		::MessageBox(NULL, (char*)pErrors->GetBufferPointer(), g_szAppName, MB_ICONSTOP);
		return false;
	}

	// Create the vertex shader from the compiled code for the target hardware
	if (FAILED(g_pD3DDevice->CreateVertexShader((DWORD*)pVSBuffer->GetBufferPointer(), &g_pVS)))
		return false;

	// Compile the pixel shader
	LPD3DXBUFFER pPSBuffer;
	if (FAILED(D3DXAssembleShader(szPixelShader, sizeof(szPixelShader) - 1, NULL, NULL, 0, &pPSBuffer, &pErrors)))
	{
		::MessageBox(NULL, "Couldn't compile pixel shader; errors: \n", g_szAppName, MB_ICONSTOP);
		::MessageBox(NULL, (char*)pErrors->GetBufferPointer(), g_szAppName, MB_ICONSTOP);
		return false;
	}

	// Create the pixel shader from the compiled code for the target hardware
	if (FAILED(g_pD3DDevice->CreatePixelShader((DWORD*)pPSBuffer->GetBufferPointer(), &g_pPS)))
		return false;

	return true;
}

// Cleanup the DirectX interfaces
void cleanup(void)
{
	// Disable the timer
	KillTimer(g_hWnd, g_pTimer);

	// Cleanup the DirectX interfaces
    if (g_pVB != NULL)
        g_pVB->Release();

    if (g_pD3DDevice != NULL)
        g_pD3DDevice->Release();

    if (g_pD3D != NULL)
        g_pD3D->Release();
}

// Render a cube every frame, and rotate it a bit
void renderFrame(void)
{
	static int nRender = 0;

    // Clear the backbuffer to black
    g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    // Begin the scene
    g_pD3DDevice->BeginScene();

    // Setup the world, view, and projection matrices
	if (nRender % ROTATE_FRAMES == 0)
	//if (nRender ==0)
		setupMatrices();
	++nRender;

    // Texture phase 1: blend the texture with the diffuse colour
    g_pD3DDevice->SetTexture(0, g_pTexture);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	
	// Set constants for the vertex shader
	g_pD3DDevice->SetVertexShaderConstantF(23, (float *)g_pVShaderMaterials, 4); // Diffuse, specular, emissive, specular power
	g_pD3DDevice->SetVertexShaderConstantF(27, (float *)g_pVShaderEyePos, 1); // Eye position

	// Render the vertex buffer contents using the index buffer
    g_pD3DDevice->SetStreamSource(0, g_pVB, 0, sizeof(XYZC_VERTEX));
    g_pD3DDevice->SetFVF(D3DFVF_XYZC_VERTEX);
	g_pD3DDevice->SetIndices(g_pIB);
	g_pD3DDevice->SetVertexShader(g_pVS);
	g_pD3DDevice->SetPixelShader(g_pPS);
    g_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

    // End the scene
    g_pD3DDevice->EndScene();

    // Present the backbuffer contents to the display
    g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

// Set the world, view & transform matrices
void setupMatrices(void)
{
	static int nCounter = 0;

    // Rotate around all 3 axes at differing rates
    D3DXMATRIX mRotX, mRotY, mRotZ, mWorld;
    D3DXMatrixRotationX(&mRotX, XROT_FACTOR * nCounter);
    D3DXMatrixRotationY(&mRotY, YROT_FACTOR * nCounter);
    D3DXMatrixRotationZ(&mRotZ, ZROT_FACTOR * nCounter);
	D3DXMatrixMultiply(&mWorld, &mRotX, &mRotY);
	D3DXMatrixMultiply(&mWorld, &mWorld, &mRotZ);
    g_pD3DDevice->SetTransform(D3DTS_WORLD, &mWorld);

	nCounter++;

    // Set the view matrix: eye, look at, up
    D3DXMATRIX mView;
    D3DXMatrixLookAtLH(&mView, &D3DXVECTOR3(0.0f, 0.5f,-3.0f),
                               &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
                               &D3DXVECTOR3(0.0f, 1.0f, 0.0f) );
    g_pD3DDevice->SetTransform(D3DTS_VIEW, &mView);

	// Set the projection matrix: fov, aspect ratio, near, far
    D3DXMATRIX mProj;
    D3DXMatrixPerspectiveFovLH(&mProj, D3DX_PI / 4, 800.0f / 600.0f, 1.0f, 100.0f);
    g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mProj);

	// Set the matrices for the vertex shader
	D3DXMATRIX mTemp;
	D3DXMatrixTranspose(&mTemp , &(mWorld * mView * mProj));
	g_pD3DDevice->SetVertexShaderConstantF(0, (float *)(mTemp), 4);
	g_pD3DDevice->SetVertexShaderConstantF(4, (float *)(mWorld), 4);

	// Transform the light
	D3DXVECTOR3 vLight(g_pVShaderLightPos);
	D3DXVECTOR4 vTransformLight;
	D3DXMATRIX mLightInv;
	D3DXMatrixIdentity(&mLightInv);
    D3DXMatrixRotationX(&mRotX, XROT_FACTOR * nCounter * -1.0f);
    D3DXMatrixRotationY(&mRotY, YROT_FACTOR * nCounter * -1.0f);
    D3DXMatrixRotationZ(&mRotZ, ZROT_FACTOR * nCounter * -1.0f);
	D3DXMatrixMultiply(&mLightInv, &mRotX, &mRotY);
	D3DXMatrixMultiply(&mLightInv, &mLightInv, &mRotZ);
	D3DXVec3Transform(&vTransformLight, &vLight, &mLightInv);
	g_pD3DDevice->SetVertexShaderConstantF(20, vTransformLight, 1); // Eye position
}