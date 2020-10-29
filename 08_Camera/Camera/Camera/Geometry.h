#ifndef GEOMETRY_H
#define GEOMETRY_H

#include"Vertex.h"
#include<vector>
#include<map>
#include<string>



namespace Geometry
{
	//������
	template<class VertexType=VertexPosNormalTexture,class IndexType=DWORD>
	struct MeshData
	{
		std::vector<VertexType> vertexVec;//��������
		std::vector<IndexType> indexVec;  //��������
		
		MeshData() 
		{	// ������������ͺϷ���
			//����������WORD(unsigned short 2bytes)����DWORD(unsigned long 4bytes)
			//static_assert�ؼ��֣�����ʵ�ֱ����ڼ�Ķ��ԣ��о�̬����,static_assert���������ʽ��Ҫ��ʾ���ַ�������
			//�����һ�������������ʽ��ֵΪfalse�������һ��������󣬴���λ�þ��Ǹ�static_assert��������У��ڶ����������Ǵ�����ʾ�ַ���
			//is_unsigned<T>::value, value: true if T is an unsigned arithmetic type , false otherwise
			static_assert(sizeof(IndexType) == 2 || sizeof(IndexType) == 4 , "The size of IndexType must be 2 bytes or 4 bytes!");
			static_assert(std::is_unsigned<IndexType>::value, "IndexType must be unsigned integer!");
		}
	};

	// ����������������levels��slicesԽ�󣬾���Խ�ߡ�
	template<class VertexType=VertexPosNormalTexture, class IndexType=DWORD>
	MeshData<VertexType, IndexType> CreateSphere(float radius = 2.0f, UINT levels = 20, UINT slices = 20, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	//����������������
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateBox(float width = 2.0f, float height = 2.0f, float depth = 2.0f,const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	// ����Բ������������slicesԽ�󣬾���Խ��
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateCylinder(float radius = 1.5f, float height = 3.0f, UINT slices=20 ,
		UINT stacks=10, float texU=1.0f,float texV=1.0f, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	// ����ֻ��Բ����������������slicesԽ�󣬾���Խ��
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateCylinderNoCap(float radius = 1.5f, float height = 3.0f, UINT slices = 20,
		UINT stacks = 10, float texU = 1.0f, float texV = 1.0f, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });
	
	// ����Բ׶����������slicesԽ�󣬾���Խ��
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateCone(float radius = 1.5f, float height = 3.0f, UINT slices = 20, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	// ����Բ׶�������������slicesԽ�󣬾���Խ��
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateConeNoCap(float radius = 1.5f, float height = 3.0f, UINT slices = 20, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	// ����һ��ָ��NDC��Ļ�������(Ĭ��ȫ��)
	template<class VertexType=VertexPosTex, class IndexType=DWORD>
	MeshData<VertexType, IndexType> Create2DShow(float centerX=0.0f, float centerY=0.0f, float scaleX=1.0f, float scaleY=1.0f, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });//scaleָ����ˮƽ������Ļ��һ�볤��

	template<class VertexType = VertexPosTex, class IndexType = DWORD>
	MeshData<VertexType, IndexType> Create2DShow(const DirectX::XMFLOAT2& center, const DirectX::XMFLOAT2& scale,const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	//����һ��ƽ��
	template<class VertexType=VertexPosNormalTexture , class IndexType=DWORD>
	MeshData<VertexType, IndexType> CreatePlane(const DirectX::XMFLOAT2& planeSize, const DirectX::XMFLOAT2& maxTexCoord = { 1.0f, 1.0f }, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreatePlane(float width = 10.0f, float height = 10.0f, float texU =1.0f, float texV = 1.0f, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });


}

namespace Geometry
{
	namespace Internal
	{
		// ���½ṹ��ͺ��������ڲ�ʵ��ʹ��

		//�ڲ�ʵ������Ҫ����VertexData���������Ķ������ݣ�������Vertex.h�ж���Ķ��ֶ������;��������Щ���ݵ���Ӧ�Ķ�������
		struct VertexData
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT4 tangent;
			DirectX::XMFLOAT4 color;
			DirectX::XMFLOAT2 texture;
		};

		// ����Ŀ�궥������ѡ���Խ����ݲ���
		template<class VertexType>
		inline void InsertVertexElement(VertexType& vertexDes, VertexData& vertexSrc)//VertexData��source,���Ƶ�Ŀ��VertexType
		{
			static std::string semanticName;// Ԫ������,Ӧ�붥�������е�inputLayout������һ��
			//����ÿ��Ԫ�����������ڴ�ƫ������ӳ��map���ڴ�ƫ����ֻ���¼��ʼ/ĩβƫ������pair<>,size_t���޷���int��https://blog.csdn.net/sevenjoin/article/details/81937695
			static const std::map<std::string, std::pair<size_t, size_t>> semanticSizeMap =
			{
				{"POSITION",std::pair<size_t,size_t>(0,12)},
				{"NORMAL", std::pair<size_t,size_t>(12,24)},
				{"TANGENT", std::pair<size_t,size_t>(24,40)},
				{"COLOR", std::pair<size_t,size_t>(40,56)},
				{"TEXCOORD", std::pair<size_t,size_t>(56,64)},
			};

			for (size_t i = 0; i < ARRAYSIZE(vertexDes.inputLayout); i++)
			{
				semanticName = vertexDes.inputLayout[i].SemanticName;
				const std::pair<size_t, size_t>& range = semanticSizeMap.at(semanticName);
				memcpy_s(reinterpret_cast<char*>(&vertexDes) + vertexDes.inputLayout[i].AlignedByteOffset,  (range.second - range.first),
					reinterpret_cast<char*>(&vertexSrc) + (range.first),  (range.second - range.first));
				//https://blog.csdn.net/lizhihaoweiwei/article/details/76184056
			}
		}
	}

	//������ʵ��

	
	//�ú����ᴴ������24�����������(������һ�������ظ�3�Σ�������������ͬ)���Լ�һ����36�����������飺
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	inline MeshData<VertexType, IndexType> CreateBox(float width, float height, float depth, const DirectX::XMFLOAT4& color)
	{
		using namespace DirectX;

		MeshData<VertexType, IndexType> meshData;//����һ��meshData����
		meshData.vertexVec.resize(24);//�����������24������

		std::vector<Internal::VertexData> BoxVertexVec(24);

		float x = width / 2.0f; float y = height / 2.0f; float z = depth / 2.0f;

		//�𶥵�����λ������
		//���棨+X�棩
		BoxVertexVec[0].position = XMFLOAT3(x, -y, -z);
		BoxVertexVec[1].position = XMFLOAT3(x, y, -z);
		BoxVertexVec[2].position = XMFLOAT3(x, y, z);
		BoxVertexVec[3].position = XMFLOAT3(x, -y, z);

		//���棨-X�棩
		BoxVertexVec[4].position = XMFLOAT3(-x, -y, z);
		BoxVertexVec[5].position = XMFLOAT3(-x, y, z);
		BoxVertexVec[6].position = XMFLOAT3(-x, y, -z);
		BoxVertexVec[7].position = XMFLOAT3(-x, -y, -z);

		//���棨+Y�棩
		BoxVertexVec[8].position = XMFLOAT3(-x, y, -z);
		BoxVertexVec[9].position = XMFLOAT3(-x, y, z);
		BoxVertexVec[10].position = XMFLOAT3(x, y, z);
		BoxVertexVec[11].position = XMFLOAT3(x, y, -z);

		//���棨-Y�棩
		BoxVertexVec[12].position = XMFLOAT3(x, -y, -z);
		BoxVertexVec[13].position = XMFLOAT3(x, -y, z);
		BoxVertexVec[14].position = XMFLOAT3(-x, -y, z);
		BoxVertexVec[15].position = XMFLOAT3(-x, -y, -z);

		//���棨+Z�棩
		BoxVertexVec[16].position = XMFLOAT3(x, -y, z);
		BoxVertexVec[17].position = XMFLOAT3(x, y, z);
		BoxVertexVec[18].position = XMFLOAT3(-x, y, z);
		BoxVertexVec[19].position = XMFLOAT3(-x, -y, z);

		//���棨-Z�棩
		BoxVertexVec[20].position = XMFLOAT3(-x, -y, -z);
		BoxVertexVec[21].position = XMFLOAT3(-x, y, -z);
		BoxVertexVec[22].position = XMFLOAT3(x, y, -z);
		BoxVertexVec[23].position = XMFLOAT3(x, -y, -z);

		//ÿ����ȡ1���������÷��ߣ����ߺ���ɫ��һ�����ĸ�����ķ���������ͬ��
		for (UINT i = 0; i < 4; i++)
		{
			//���棨+X�棩
			BoxVertexVec[i].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
			BoxVertexVec[i].tangent = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
			BoxVertexVec[i].color = color;

			//���棨-X�棩
			BoxVertexVec[i+4].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			BoxVertexVec[i+4].tangent = XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);
			BoxVertexVec[i+4].color = color;

			//���棨+Y�棩
			BoxVertexVec[i + 8].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			BoxVertexVec[i + 8].tangent = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			BoxVertexVec[i + 8].color = color;

			//���棨-Y�棩
			BoxVertexVec[i + 12].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
			BoxVertexVec[i + 12].tangent = XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f);
			BoxVertexVec[i + 12].color = color;

			//���棨+Z�棩
			BoxVertexVec[i + 16].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			BoxVertexVec[i + 16].tangent = XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f);
			BoxVertexVec[i + 16].color = color;

			//���棨-Z�棩
			BoxVertexVec[i + 20].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			BoxVertexVec[i + 20].tangent = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			BoxVertexVec[i + 20].color = color;
		}

		//ÿ���������������꣬ÿ����������ͬ
		for (UINT i = 0; i < 6; i++)
		{
			BoxVertexVec[4 * i].texture = XMFLOAT2(0.0f, 1.0f);
			BoxVertexVec[4 * i + 1].texture = XMFLOAT2(0.0f, 0.0f);
			BoxVertexVec[4 * i + 2].texture = XMFLOAT2(1.0f, 0.0f);
			BoxVertexVec[4 * i + 3].texture = XMFLOAT2(1.0f, 1.0f);
		}

		for (UINT i = 0; i < 24; i++)
		{
			Internal::InsertVertexElement(meshData.vertexVec[i], BoxVertexVec[i]);
		}

		meshData.indexVec =
		{
			0,1,2,2,3,0,//���棨+X�棩
			4,5,6,6,7,4,//���棨-X�棩
			8,9,10,10,11,8,//���棨+Y�棩
			12,13,14,14,15,12,//���棨-Y�棩
			16,17,18,18,19,16,//���棨+Z�棩
			20,21,22,22,23,20//���棨-Z�棩
		};
		
		return meshData;

	}
	

	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateSphere(float radius, UINT levels, UINT slices, const DirectX::XMFLOAT4& color)//levels���ˮƽ������slicesÿһ��Ķ�����
	{
		using namespace DirectX;

		MeshData<VertexType, IndexType> meshData;

		UINT vertexcount = 2 + (levels - 1) * (slices + 1);//levels-1����Ƭ��ÿ����Ƭslices+1�����㣨����յ��������겻һ�£���������
		//UINT vertexcount = 2 + (levels ) * (slices );
		UINT indexcount = 6 * (levels - 1) * slices;//�ϱ����㹲slices����������һ�����һ��slices�������5��������ʣ�µ�levels-3����м�㣬ÿ������6������
		meshData.vertexVec.resize(vertexcount);
		meshData.indexVec.resize(indexcount);

		Internal::VertexData vertexData;//����һ����������
		IndexType vertex = 0, index = 0;//����ı�ź������ı��

		//������ϵ
		float phi = 0.0f;//��ֱ�����ת��
		float theta = 0.0f;//ˮƽ�����ת��
		float per_phi = XM_PI / levels;//ÿ�仯һ�㴹ֱ�����ת��
		float per_theta = XM_2PI / slices;//ˮƽԲÿ�仯һ����ˮƽ�����ת��
		float x, y, z;

		//���붥������(������)
		vertexData = { XMFLOAT3(0.0f,radius,0.0f),XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(0.0f,0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		for (UINT i = 1; i < levels; i++)
		{
			phi = per_phi * i;
			// ��Ҫslices + 1����������Ϊ �����յ���Ϊͬһ�㣬����������ֵ��һ��
			for (UINT j = 0; j <= slices; j++)
			{
				//������ϵ
				theta = per_theta * j;
				x = radius * sinf(phi) * cosf(theta);
				y = radius * cosf(phi);
				z = radius * sinf(phi) * sinf(theta);
				// ������ֲ����ꡢ��������Tangent��������������
				//XMFLOATX����Ҫ��ת����Vector���ܲ����������㣬��������ת��XMFLOATX
				XMFLOAT3 pos = XMFLOAT3(x, y, z);
				XMFLOAT3 normal;
				XMStoreFloat3(&normal, XMVector3Normalize(XMLoadFloat3(&pos)));//���϶���ķ��߾��Ƕ��������ȥ���ĵı�׼������
				//����ö�������
				vertexData.position = pos;
				vertexData.normal = normal;
				vertexData.color = color;
				vertexData.tangent = XMFLOAT4(-sinf(theta), 0.0f, cosf(theta), 1.0f);
				vertexData.texture = XMFLOAT2(theta / XM_2PI, phi / XM_PI);
				//vertexData = { pos,normal,XMFLOAT4(-sinf(theta),0.0f,cosf(theta),1.0f),XMFLOAT2(theta / XM_2PI,phi / XM_PI) };
				Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);
			}
		}

		//����׶����ݣ��ϼ��㣩
		vertexData = { XMFLOAT3(0.0f,-radius,0.0f),XMFLOAT3(0.0f,-1.0f,0.0f),XMFLOAT4(-1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(0.0f,1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//������������,index++

		//��һ����Ƭ�ͱ�������ɵĶ������׶��ÿƬ������˳ʱ�����������
		//�ӱ�����������0��ʼ����Ƭ��һ���������������1(��һƬ������0��2��1)��Ҳ��slices+1�����һƬ������0��slices+1,slices��������յ��������겻һ��
		if (levels > 1)//�зֲ�
		{
			for (UINT j = 1; j <= slices; j++)
			{
				meshData.indexVec[index++] = 0;
				meshData.indexVec[index++] = j % (slices + 1) + 1;
				meshData.indexVec[index++] = j;
			}
		}

		//�м�������Ƭ��ɵĶ����̨�����±߶�Ӧ������ɵ��ı��η�Ϊ���������Σ�˳ʱ�����α�������������n��ʵ��ֻ������n-1����Ƭ��ÿ����һ����Ƭ������slices+1������
		if (levels > 2)
		{
			for (UINT i = 1; i < levels - 1; i++)
			{
				for (UINT j = 1; j <= slices; j++)
				{
					//�ı��������Ϸ�������
					meshData.indexVec[index++] = (i - 1) * (slices + 1) + j;
					meshData.indexVec[index++] = (i - 1) * (slices + 1) + j % (slices + 1) + 1;
					meshData.indexVec[index++] = i * (slices + 1) + j % (slices + 1) + 1;
					//�ı��������·�������
					meshData.indexVec[index++] = i * (slices + 1) + j % (slices + 1) + 1;
					meshData.indexVec[index++] = i * (slices + 1) + j;
					meshData.indexVec[index++] = (i - 1) * (slices + 1) + j;
				}
			}
		}

		//���һ����Ƭ���ϼ�����ɵĶ������׶��ÿƬ������˳ʱ�����������
		//���м������������һ����Ƭ��ʼ���ϼ��㣬��ʱi=levels-2
		if (levels > 1)//�зֲ�
		{
			for (UINT j = 1; j <= slices; j++)
			{
				meshData.indexVec[index++] = (levels - 2) * (slices + 1) + j;
				meshData.indexVec[index++] = (levels - 2) * (slices + 1) + j % (slices + 1) + 1;
				meshData.indexVec[index++] = (levels - 1) * (slices + 1) + 1;
			}
		}
		return meshData;
	}


	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateCylinderNoCap(float radius, float height, UINT slices,
		UINT stacks, float texU, float texV, const DirectX::XMFLOAT4& color)
	{
		using namespace DirectX;

		MeshData<VertexType, IndexType> meshData;
		//ÿ����Ƭslices+1�����㣬stacks����stacks+1����Ƭ
		//��һ�����һ����Ƭ��slieces������ÿ������3�Σ�ʣ�µ�stcks-1����Ƭ��slieces������ÿ������6��
		UINT vertexCount = (slices + 1) * (stacks + 1);
		UINT indexCount = 6 * stacks * slices;
		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		float halfHeight = height / 2.0f;//�ֲ�����ԭ������Ͱ�м�
		float theta = 0.0f;//ˮƽ�����ת��
		float per_theta = XM_2PI / slices;//һ����ÿ����һ�����㣬ˮƽ����ת���ļӽǶ�
		float stackHeight = height / stacks;//���

		Internal::VertexData vertexData;
		IndexType vertex = 0, index = 0;

		//�Ե������������˵�
		for (UINT i = 0; i <= stacks; i++)
		{
			float y = stackHeight* i - halfHeight;
			for (UINT j = 0; j <= slices; j++)
			{
				theta = j * per_theta;
				float u = theta / XM_2PI;
				float v = 1.0f - (float)(i / stacks);
				vertexData = { XMFLOAT3(radius * cosf(theta),y,radius * sinf(theta)),XMFLOAT3(cosf(theta),0.0f,sinf(theta)),XMFLOAT4(-sinf(theta),0.0f,cosf(theta),1.0f),
				color,XMFLOAT2(u * texU,v * texV) };
				Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);
			}
		}

		//�Ե����������������
		for (UINT i = 0; i < stacks; i++)
		{
			for (UINT j = 0; j < slices; j++)
			{
				meshData.indexVec[index++] = i * (slices + 1) + j;
				meshData.indexVec[index++] = (i + 1) * (slices + 1) + j;
				meshData.indexVec[index++] = (i + 1) * (slices + 1) + j + 1;
			
				meshData.indexVec[index++] = (i + 1) * (slices + 1) + j + 1;
				meshData.indexVec[index++] = i * (slices + 1) + j + 1;
				meshData.indexVec[index++] = i * (slices + 1) + j;
			}
		}
		return meshData;
	}


	//�޵ף�������Բ���������ټ�����Բ����Ϊ�����ס�
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateCylinder(float radius, float height, UINT slices,
		UINT stacks, float texU, float texV, const DirectX::XMFLOAT4& color)
	{
		using namespace DirectX;

		MeshData<VertexType, IndexType> meshData = CreateCylinderNoCap<VertexType, IndexType>(radius, height, slices, stacks, texU, texV);
		UINT vertexCount = (slices + 1) * (stacks + 3)+2;
		UINT indexCount = 6 * stacks * slices + 6*slices;
		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		float halfHeight = height / 2.0f;//�ֲ�����ԭ������Ͱ�м�
		float theta = 0.0f;//ˮƽ�����ת��
		float per_theta = XM_2PI / slices;//һ����ÿ����һ�����㣬ˮƽ����ת���ļӽǶ�

		Internal::VertexData vertexData;
		IndexType vertex = (slices + 1) * (stacks + 1); //������ʼ��=�޵�Բ���Ķ�������
		IndexType index = 6 * stacks * slices; //������ʼ��=�޵�Բ������������
		IndexType offset = vertex;

		//���붥��Բ��
		vertexData = { XMFLOAT3(0.0f,halfHeight,0.0f),XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(0.5f,0.5f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//���붥��Բ�ϸ���
		for (UINT j = 0; j <= slices; j++)
		{
			theta = j * per_theta;
			float u = theta / XM_2PI;
			float v = 0.5f;
			vertexData = { XMFLOAT3(radius * cosf(theta),halfHeight,radius * sinf(theta)),XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),
			color,XMFLOAT2(u ,v ) };
			Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);
		}

		//����׶�Բ��
		vertexData = { XMFLOAT3(0.0f,-halfHeight,0.0f),XMFLOAT3(0.0f,-1.0f,0.0f),XMFLOAT4(-1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(0.5f,0.5f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//����׶�Բ�ϸ���
		for (UINT j = 0; j <= slices; j++)
		{
			theta = j * per_theta;
			float u = theta / XM_2PI;
			float v = 0.5f;
			vertexData = { XMFLOAT3(radius * cosf(theta),-halfHeight,radius * sinf(theta)),XMFLOAT3(0.0f,-1.0f,0.0f),XMFLOAT4(-1.0f,0.0f,0.0f,1.0f),
			color,XMFLOAT2(u ,v) };
			Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);
		}

		//���������ε�����,ͬ��ı�����
		for (UINT i = 1; i <= slices; i++)
		{
			meshData.indexVec[index++] = offset;//Բ��
			meshData.indexVec[index++] = offset + i % (slices + 1) + 1;
			meshData.indexVec[index++] = offset + i ;
		}

		//�ײ������ε�����,ͬ����ϼ���
		offset += slices + 2;//һ��Բ������Բ��һ��slices+2��
		for (UINT i = 1; i <= slices; i++)
		{
			meshData.indexVec[index++] = offset;//Բ��
			meshData.indexVec[index++] = offset + i  ;
			meshData.indexVec[index++] = offset + i % (slices + 1)+1;
		}
		return meshData;
	}


	//�ú����������ײ���Բ���棬ע��Բ׶�ļ����Ϊ���������������η�������һ�£���Ҫ�����͵ײ�Բ��ͬ����Ŀ�Ķ���
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateConeNoCap(float radius, float height, UINT slices, const DirectX::XMFLOAT4& color)
	{
		using namespace DirectX;

		MeshData<VertexType, IndexType> meshData;
		meshData.vertexVec.resize(2 * slices);
		meshData.indexVec.resize(3 * slices);

		float halfHeight = height / 2.0f;
		float theta = 0.0f;
		float per_theta = XM_2PI / slices;
		float length = sqrtf(height * height + radius * radius);//Բ׶б�߳���
		UINT vertex = 0;
		UINT index = 0;

		Internal::VertexData vertexData;

		//����Բ׶��˶���(ÿ�����������ͬ�ķ���������������)
		for (UINT i = 0; i < slices; i++)
		{
			theta = i * per_theta + per_theta / 2.0f;
			vertexData = { XMFLOAT3(0.0f,halfHeight,0.0f), XMFLOAT3(radius * cosf(theta) / length, height / length, radius * sinf(theta) / length),
				XMFLOAT4(-sinf(theta),0.0f,cosf(theta),1.0f),color,XMFLOAT2(0.5f,0.5f) };
			Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);
		}

		//����Բ׶����Ķ���
		for (UINT i = 0; i < slices; i++)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(radius*cosf(theta),-halfHeight,radius*sinf(theta)), XMFLOAT3(radius * cosf(theta) / length, height / length, radius * sinf(theta) / length),
				XMFLOAT4(-sinf(theta),0.0f,cosf(theta),1.0f),color,XMFLOAT2(cosf(theta) / 2 + 0.5f, sinf(theta) / 2 + 0.5f) };
			Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);
		}

		//��������
		for (UINT i = 0; i < slices; i++)
		{
			meshData.indexVec[index++] = i;//׶��������
			meshData.indexVec[index++] = slices + (i + 1) % slices;
			meshData.indexVec[index++] = slices + i;
		}

		return meshData;

	}


	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateCone(float radius, float height, UINT slices, const DirectX::XMFLOAT4& color)
	{
		using namespace DirectX;

		MeshData<VertexType, IndexType> meshData=CreateConeNoCap<VertexType, IndexType>(radius, height, slices, color);

		int vertexCount = 3 * slices + 1;
		int indexCount = 6 * slices;
		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		float halfHeight = height / 2.0f;
		float theta = 0.0f;
		float per_theta = XM_2PI / slices;
		UINT vertex = 2*slices;
		UINT index = 3*slices;

		Internal::VertexData vertexData;

		//�������Բ�Ķ���
		for (UINT i = 0; i < slices; i++)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(radius*cosf(theta),-halfHeight,radius*sinf(theta)), XMFLOAT3(0.0f, -halfHeight, 0.0f),
				XMFLOAT4(-1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(cosf(theta) / 2 + 0.5f, sinf(theta) / 2 + 0.5f) };
			Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);
		}
		//Բ��
		vertexData = { XMFLOAT3(0.0f, -halfHeight, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),
			  XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.5f, 0.5f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//��������
		UINT offset = 2 * slices;
		for (UINT i = 0; i < slices; i++)
		{
			meshData.indexVec[index++] = offset + slices;
			meshData.indexVec[index++] = offset + i;
			meshData.indexVec[index++] = offset + (i+1)%slices;
		}

		return meshData;
	}

	
	template<class VertexType = VertexPosTex, class IndexType = DWORD>
	MeshData<VertexType, IndexType> Create2DShow(float centerX, float centerY, float scaleX, float scaleY, const DirectX::XMFLOAT4& color)
	{
		using namespace DirectX;

		MeshData<VertexType, IndexType> meshData;
		meshData.vertexVec.resize(4);

		Internal::VertexData vertexData;
		UINT vertex = 0;

		//��Ļ���½�
		vertexData = { XMFLOAT3(centerX - scaleX,centerY - scaleY,0.0f),XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(0.0f,1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//��Ļ���Ͻ�
		vertexData = { XMFLOAT3(centerX - scaleX,centerY + scaleY,0.0f),XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(0.0f,0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//��Ļ���Ͻ�
		vertexData = { XMFLOAT3(centerX + scaleX,centerY + scaleY,0.0f),XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(1.0f,0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//��Ļ���½�
		vertexData = { XMFLOAT3(centerX + scaleX,centerY - scaleY,0.0f),XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(1.0f,1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		meshData.indexVec = { 0, 1, 2, 2, 3, 0 };
		return meshData;

	}
	
	template<class VertexType = VertexPosTex, class IndexType = DWORD>
	MeshData<VertexType, IndexType> Create2DShow(const DirectX::XMFLOAT2& center, const DirectX::XMFLOAT2& scale, const DirectX::XMFLOAT4& color)
	{
		return Create2DShow<VertexType, IndexType>(center.x, center.y, scale.x, scale.y, color);
	}

	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreatePlane(const DirectX::XMFLOAT2& planeSize, const DirectX::XMFLOAT2& maxTexCoord , const DirectX::XMFLOAT4& color )
	{
		return CreatePlane<VertexType, IndexType>(planeSize.x, planeSize.y, maxTexCoord.x, maxTexCoord.y, color);
	}

	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreatePlane(float width, float height, float texU, float texV, const DirectX::XMFLOAT4& color )
	{
		using namespace DirectX;

		MeshData<VertexType, IndexType> meshData;
		meshData.vertexVec.resize(4);

		Internal::VertexData vertexData;
		UINT vIndex = 0;

		vertexData = { XMFLOAT3(-width / 2, 0.0f, -height / 2), XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.0f, texV) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		vertexData = { XMFLOAT3(-width / 2, 0.0f, height / 2), XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		vertexData = { XMFLOAT3(width / 2, 0.0f, height / 2), XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(texU, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		vertexData = { XMFLOAT3(width / 2, 0.0f, -height / 2), XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(texU, texV) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		meshData.indexVec = { 0, 1, 2, 2, 3, 0 };
		return meshData;
	}


}





























#endif