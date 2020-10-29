#ifndef VERTEX_H
#define VERTEX_H

#include<d3d11_1.h>
#include<DirectXMath.h>

struct VertexPos
{
	VertexPos() = default;

	VertexPos(const VertexPos&) = default;
	VertexPos& operator=(const VertexPos&) = default;

	VertexPos(VertexPos&&) = default;
	VertexPos& operator=(VertexPos&&) = default;

	constexpr VertexPos(DirectX::XMFLOAT3& pos):position(pos) {}

	DirectX::XMFLOAT3 position;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[1];

};


struct VertexPosColor
{
	VertexPosColor() = default;

	VertexPosColor(const VertexPosColor&) = default;
	VertexPosColor& operator=(const VertexPosColor&) = default;

	VertexPosColor(VertexPosColor&&) = default;
	VertexPosColor& operator=(VertexPosColor&&) = default;

	constexpr VertexPosColor(DirectX::XMFLOAT3& pos, DirectX::XMFLOAT4 & col) :position(pos),color(col) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];

};


struct VertexPosTex
{
	VertexPosTex() = default;

	VertexPosTex(const VertexPosTex&) = default;
	VertexPosTex& operator=(const VertexPosTex&) = default;

	VertexPosTex(VertexPosTex&&) = default;
	VertexPosTex& operator=(VertexPosTex&&) = default;

	constexpr VertexPosTex(DirectX::XMFLOAT3& pos, DirectX::XMFLOAT2& tex) :position(pos), texture(tex) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};

struct VertexPosSize
{
	VertexPosSize() = default;

	VertexPosSize(const VertexPosSize&) = default;
	VertexPosSize& operator=(const VertexPosSize&) = default;

	VertexPosSize(VertexPosSize&&) = default;
	VertexPosSize& operator=(VertexPosSize&&) = default;

	constexpr VertexPosSize(DirectX::XMFLOAT3 & pos, DirectX::XMFLOAT2 & _size) :position(pos), size(_size) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 size;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};


struct VertexPosNormalColor
{
	VertexPosNormalColor() = default;

	VertexPosNormalColor(const VertexPosNormalColor&) = default;
	VertexPosNormalColor& operator=(const VertexPosNormalColor&) = default;

	VertexPosNormalColor(VertexPosNormalColor&&) = default;
	VertexPosNormalColor& operator=(VertexPosNormalColor&&) = default;

	constexpr VertexPosNormalColor(DirectX::XMFLOAT3& pos, DirectX::XMFLOAT3& nor,DirectX::XMFLOAT4& col ) :position(pos),normal(nor), color(col){}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 color;

	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];

};


struct VertexPosNormalTexture
{
	VertexPosNormalTexture() = default;

	VertexPosNormalTexture(const VertexPosNormalTexture&) = default;
	VertexPosNormalTexture& operator=(const VertexPosNormalTexture&) = default;

	VertexPosNormalTexture(VertexPosNormalTexture&&) = default;
	VertexPosNormalTexture& operator=(VertexPosNormalTexture&&) = default;

	constexpr VertexPosNormalTexture(DirectX::XMFLOAT3& pos, DirectX::XMFLOAT3& nor, DirectX::XMFLOAT2& tex) :position(pos), normal(nor), texture(tex) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texture;

	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};


struct VertexPosNormalTangentTexture
{
	VertexPosNormalTangentTexture() = default;

	VertexPosNormalTangentTexture(const VertexPosNormalTangentTexture&) = default;
	VertexPosNormalTangentTexture& operator=(const VertexPosNormalTangentTexture&) = default;

	VertexPosNormalTangentTexture(VertexPosNormalTangentTexture&&) = default;
	VertexPosNormalTangentTexture& operator=(VertexPosNormalTangentTexture&&) = default;

	constexpr VertexPosNormalTangentTexture(DirectX::XMFLOAT3& pos, DirectX::XMFLOAT3& nor,DirectX::XMFLOAT4& tan, DirectX::XMFLOAT2& tex) 
		:position(pos), normal(nor),tangent(tan), texture(tex) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 tangent;
	DirectX::XMFLOAT2 texture;

	static const D3D11_INPUT_ELEMENT_DESC inputLayout[4];
};











#endif

