////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_Light = 0;
	m_ShaderManager = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128], textureFilename1[128], textureFilename2[128];
	bool result;


	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the camera object.
	m_Camera = new CameraClass;

	m_Camera->SetPosition(0.0f, 0.0f, -8.0f);
	m_Camera->Render();

	// Set the file name of the model.
    strcpy_s(modelFilename, "../Engine/data/sphere.txt");

    // Set the file name of the textures.
    strcpy_s(textureFilename1, "../Engine/data/stone01.tga");
    strcpy_s(textureFilename2, "../Engine/data/normal01.tga");

    // Create and initialize the model object.
    m_Model = new ModelClass;

    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename1, textureFilename2);
    if(!result)
    {
        return false;
    }

	 // Create and initialize the light object.
    m_Light = new LightClass;

    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	// Create and initialize the normal map shader object.
	m_ShaderManager = new ShaderManagerClass;

	result = m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		return false;
	}

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the shader manager object.
	if(m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}

	// Release the light object.
    if(m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

	// Release the model object.
    if(m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
    }

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}


bool ApplicationClass::Frame(InputClass* Input)
{
	static float rotation = 360.0f;
	bool result;


	// Check if the user pressed escape and wants to exit the application.
	if(Input->IsEscapePressed())
	{
		return false;
	}

	// Update the rotation variable each frame.
    rotation -= 0.0174532925f * 0.25f;
    if(rotation <= 0.0f)
    {
        rotation += 360.0f;
    }

	// Render the graphics scene.
	result = Render(rotation);
	if(!result)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotateMatrix, translateMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Setup matrices.
	rotateMatrix = XMMatrixRotationY(rotation);
	translateMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(rotateMatrix, translateMatrix);

    // Render the model using the texture shader.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	result = m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
												  m_Model->GetTexture(0));
	if(!result)
	{
		return false;
	}

	// Setup matrices.
	rotateMatrix = XMMatrixRotationY(rotation);
	translateMatrix = XMMatrixTranslation(-1.5f, -1.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(rotateMatrix, translateMatrix);

	// Render the model using the light shader.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	result = m_ShaderManager->RenderLightShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
												m_Model->GetTexture(0), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if(!result)
	{
		return false;
	}

	// Setup matrices.
	rotateMatrix = XMMatrixRotationY(rotation);
	translateMatrix = XMMatrixTranslation(1.5f, -1.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(rotateMatrix, translateMatrix);

	// Render the model using the normal map shader.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	result = m_ShaderManager->RenderNormalMapShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
													m_Model->GetTexture(0), m_Model->GetTexture(1), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if(!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}