////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "fontclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
	    XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, int, FontClass*, char*, int, int, float, float, float);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	bool UpdateText(ID3D11DeviceContext*, FontClass*, char*, int, int, float, float, float);
    XMFLOAT4 GetPixelColor();

private:
	bool InitializeBuffers(ID3D11Device*, ID3D11DeviceContext*, FontClass*, char*, int, int, float, float, float);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_screenWidth, m_screenHeight, m_maxLength, m_vertexCount, m_indexCount;
	XMFLOAT4 m_pixelColor;
};

#endif