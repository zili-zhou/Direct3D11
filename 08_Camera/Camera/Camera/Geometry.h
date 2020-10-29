#ifndef GEOMETRY_H
#define GEOMETRY_H

#include"Vertex.h"
#include<vector>
#include<map>
#include<string>



namespace Geometry
{
	//网格类
	template<class VertexType=VertexPosNormalTexture,class IndexType=DWORD>
	struct MeshData
	{
		std::vector<VertexType> vertexVec;//顶点数组
		std::vector<IndexType> indexVec;  //索引数组
		
		MeshData() 
		{	// 需检验索引类型合法性
			//索引可以是WORD(unsigned short 2bytes)或者DWORD(unsigned long 4bytes)
			//static_assert关键字，用来实现编译期间的断言，叫静态断言,static_assert（常量表达式，要提示的字符串）；
			//如果第一个参数常量表达式的值为false，会产生一条编译错误，错误位置就是该static_assert语句所在行，第二个参数就是错误提示字符串
			//is_unsigned<T>::value, value: true if T is an unsigned arithmetic type , false otherwise
			static_assert(sizeof(IndexType) == 2 || sizeof(IndexType) == 4 , "The size of IndexType must be 2 bytes or 4 bytes!");
			static_assert(std::is_unsigned<IndexType>::value, "IndexType must be unsigned integer!");
		}
	};

	// 创建球体网格函数，levels和slices越大，精度越高。
	template<class VertexType=VertexPosNormalTexture, class IndexType=DWORD>
	MeshData<VertexType, IndexType> CreateSphere(float radius = 2.0f, UINT levels = 20, UINT slices = 20, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	//创建立方体网格函数
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateBox(float width = 2.0f, float height = 2.0f, float depth = 2.0f,const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	// 创建圆柱体网格函数，slices越大，精度越高
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateCylinder(float radius = 1.5f, float height = 3.0f, UINT slices=20 ,
		UINT stacks=10, float texU=1.0f,float texV=1.0f, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	// 创建只有圆柱体侧面的网格函数，slices越大，精度越高
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateCylinderNoCap(float radius = 1.5f, float height = 3.0f, UINT slices = 20,
		UINT stacks = 10, float texU = 1.0f, float texV = 1.0f, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });
	
	// 创建圆锥体网格函数，slices越大，精度越高
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateCone(float radius = 1.5f, float height = 3.0f, UINT slices = 20, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	// 创建圆锥体侧面网格函数，slices越大，精度越高
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateConeNoCap(float radius = 1.5f, float height = 3.0f, UINT slices = 20, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	// 创建一个指定NDC屏幕区域的面(默认全屏)
	template<class VertexType=VertexPosTex, class IndexType=DWORD>
	MeshData<VertexType, IndexType> Create2DShow(float centerX=0.0f, float centerY=0.0f, float scaleX=1.0f, float scaleY=1.0f, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });//scale指的是水平方向屏幕的一半长度

	template<class VertexType = VertexPosTex, class IndexType = DWORD>
	MeshData<VertexType, IndexType> Create2DShow(const DirectX::XMFLOAT2& center, const DirectX::XMFLOAT2& scale,const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	//创建一个平面
	template<class VertexType=VertexPosNormalTexture , class IndexType=DWORD>
	MeshData<VertexType, IndexType> CreatePlane(const DirectX::XMFLOAT2& planeSize, const DirectX::XMFLOAT2& maxTexCoord = { 1.0f, 1.0f }, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });

	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreatePlane(float width = 10.0f, float height = 10.0f, float texU =1.0f, float texV = 1.0f, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });


}

namespace Geometry
{
	namespace Internal
	{
		// 以下结构体和函数仅供内部实现使用

		//内部实现中需要依赖VertexData产生完整的顶点数据，并根据Vertex.h中定义的多种顶点类型决定输出哪些数据到对应的顶点类型
		struct VertexData
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT4 tangent;
			DirectX::XMFLOAT4 color;
			DirectX::XMFLOAT2 texture;
		};

		// 根据目标顶点类型选择性将数据插入
		template<class VertexType>
		inline void InsertVertexElement(VertexType& vertexDes, VertexData& vertexSrc)//VertexData是source,复制到目标VertexType
		{
			static std::string semanticName;// 元素语义,应与顶点类型中的inputLayout中描述一致
			//建立每个元素语义与其内存偏移量的映射map，内存偏移量只需记录起始/末尾偏移量的pair<>,size_t是无符号int。https://blog.csdn.net/sevenjoin/article/details/81937695
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

	//几何体实现

	
	//该函数会创建包含24个顶点的数组(立方体一个顶点重复3次，但法向量不相同)，以及一个含36个索引的数组：
	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	inline MeshData<VertexType, IndexType> CreateBox(float width, float height, float depth, const DirectX::XMFLOAT4& color)
	{
		using namespace DirectX;

		MeshData<VertexType, IndexType> meshData;//创建一个meshData对象
		meshData.vertexVec.resize(24);//顶点数组包含24个顶点

		std::vector<Internal::VertexData> BoxVertexVec(24);

		float x = width / 2.0f; float y = height / 2.0f; float z = depth / 2.0f;

		//逐顶点设立位置坐标
		//右面（+X面）
		BoxVertexVec[0].position = XMFLOAT3(x, -y, -z);
		BoxVertexVec[1].position = XMFLOAT3(x, y, -z);
		BoxVertexVec[2].position = XMFLOAT3(x, y, z);
		BoxVertexVec[3].position = XMFLOAT3(x, -y, z);

		//左面（-X面）
		BoxVertexVec[4].position = XMFLOAT3(-x, -y, z);
		BoxVertexVec[5].position = XMFLOAT3(-x, y, z);
		BoxVertexVec[6].position = XMFLOAT3(-x, y, -z);
		BoxVertexVec[7].position = XMFLOAT3(-x, -y, -z);

		//顶面（+Y面）
		BoxVertexVec[8].position = XMFLOAT3(-x, y, -z);
		BoxVertexVec[9].position = XMFLOAT3(-x, y, z);
		BoxVertexVec[10].position = XMFLOAT3(x, y, z);
		BoxVertexVec[11].position = XMFLOAT3(x, y, -z);

		//底面（-Y面）
		BoxVertexVec[12].position = XMFLOAT3(x, -y, -z);
		BoxVertexVec[13].position = XMFLOAT3(x, -y, z);
		BoxVertexVec[14].position = XMFLOAT3(-x, -y, z);
		BoxVertexVec[15].position = XMFLOAT3(-x, -y, -z);

		//后面（+Z面）
		BoxVertexVec[16].position = XMFLOAT3(x, -y, z);
		BoxVertexVec[17].position = XMFLOAT3(x, y, z);
		BoxVertexVec[18].position = XMFLOAT3(-x, y, z);
		BoxVertexVec[19].position = XMFLOAT3(-x, -y, z);

		//正面（-Z面）
		BoxVertexVec[20].position = XMFLOAT3(-x, -y, -z);
		BoxVertexVec[21].position = XMFLOAT3(-x, y, -z);
		BoxVertexVec[22].position = XMFLOAT3(x, y, -z);
		BoxVertexVec[23].position = XMFLOAT3(x, -y, -z);

		//每个面取1个顶点设置法线，切线和颜色（一个面四个顶点的法线切线相同）
		for (UINT i = 0; i < 4; i++)
		{
			//右面（+X面）
			BoxVertexVec[i].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
			BoxVertexVec[i].tangent = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
			BoxVertexVec[i].color = color;

			//左面（-X面）
			BoxVertexVec[i+4].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			BoxVertexVec[i+4].tangent = XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);
			BoxVertexVec[i+4].color = color;

			//上面（+Y面）
			BoxVertexVec[i + 8].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			BoxVertexVec[i + 8].tangent = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			BoxVertexVec[i + 8].color = color;

			//底面（-Y面）
			BoxVertexVec[i + 12].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
			BoxVertexVec[i + 12].tangent = XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f);
			BoxVertexVec[i + 12].color = color;

			//后面（+Z面）
			BoxVertexVec[i + 16].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			BoxVertexVec[i + 16].tangent = XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f);
			BoxVertexVec[i + 16].color = color;

			//正面（-Z面）
			BoxVertexVec[i + 20].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			BoxVertexVec[i + 20].tangent = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			BoxVertexVec[i + 20].color = color;
		}

		//每个面设置纹理坐标，每个面纹理相同
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
			0,1,2,2,3,0,//右面（+X面）
			4,5,6,6,7,4,//左面（-X面）
			8,9,10,10,11,8,//上面（+Y面）
			12,13,14,14,15,12,//底面（-Y面）
			16,17,18,18,19,16,//后面（+Z面）
			20,21,22,22,23,20//正面（-Z面）
		};
		
		return meshData;

	}
	

	template<class VertexType = VertexPosNormalTexture, class IndexType = DWORD>
	MeshData<VertexType, IndexType> CreateSphere(float radius, UINT levels, UINT slices, const DirectX::XMFLOAT4& color)//levels球的水平层数，slices每一层的顶点数
	{
		using namespace DirectX;

		MeshData<VertexType, IndexType> meshData;

		UINT vertexcount = 2 + (levels - 1) * (slices + 1);//levels-1层切片，每层切片slices+1个顶点（起点终点纹理坐标不一致，算两个）
		//UINT vertexcount = 2 + (levels ) * (slices );
		UINT indexcount = 6 * (levels - 1) * slices;//南北极点共slices个索引，第一层最后一层slices个顶点各5次索引，剩下的levels-3层的中间层，每个顶点6次索引
		meshData.vertexVec.resize(vertexcount);
		meshData.indexVec.resize(indexcount);

		Internal::VertexData vertexData;//创建一个顶点数据
		IndexType vertex = 0, index = 0;//顶点的编号和索引的编号

		//球坐标系
		float phi = 0.0f;//垂直方向的转交
		float theta = 0.0f;//水平方向的转角
		float per_phi = XM_PI / levels;//每变化一层垂直方向的转角
		float per_theta = XM_2PI / slices;//水平圆每变化一条边水平方向的转角
		float x, y, z;

		//放入顶点数据(北极点)
		vertexData = { XMFLOAT3(0.0f,radius,0.0f),XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(0.0f,0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		for (UINT i = 1; i < levels; i++)
		{
			phi = per_phi * i;
			// 需要slices + 1个顶点是因为 起点和终点需为同一点，但纹理坐标值不一致
			for (UINT j = 0; j <= slices; j++)
			{
				//球坐标系
				theta = per_theta * j;
				x = radius * sinf(phi) * cosf(theta);
				y = radius * cosf(phi);
				z = radius * sinf(phi) * sinf(theta);
				// 计算出局部坐标、法向量、Tangent向量和纹理坐标
				//XMFLOATX类型要先转换成Vector才能参与向量运算，运算完再转回XMFLOATX
				XMFLOAT3 pos = XMFLOAT3(x, y, z);
				XMFLOAT3 normal;
				XMStoreFloat3(&normal, XMVector3Normalize(XMLoadFloat3(&pos)));//球上顶点的法线就是顶点坐标减去球心的标准化向量
				//放入该顶点数据
				vertexData.position = pos;
				vertexData.normal = normal;
				vertexData.color = color;
				vertexData.tangent = XMFLOAT4(-sinf(theta), 0.0f, cosf(theta), 1.0f);
				vertexData.texture = XMFLOAT2(theta / XM_2PI, phi / XM_PI);
				//vertexData = { pos,normal,XMFLOAT4(-sinf(theta),0.0f,cosf(theta),1.0f),XMFLOAT2(theta / XM_2PI,phi / XM_PI) };
				Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);
			}
		}

		//放入底端数据（南极点）
		vertexData = { XMFLOAT3(0.0f,-radius,0.0f),XMFLOAT3(0.0f,-1.0f,0.0f),XMFLOAT4(-1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(0.0f,1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//放入索引数据,index++

		//第一层切片和北极点组成的多边形棱锥，每片三角形顺时针存入索引。
		//从北极点索引是0开始，切片第一个顶点的索引既是1(第一片三角形0，2，1)，也是slices+1（最后一片三角形0，slices+1,slices），起点终点纹理坐标不一致
		if (levels > 1)//有分层
		{
			for (UINT j = 1; j <= slices; j++)
			{
				meshData.indexVec[index++] = 0;
				meshData.indexVec[index++] = j % (slices + 1) + 1;
				meshData.indexVec[index++] = j;
			}
		}

		//中间两层切片组成的多边形台，上下边对应顶点组成的四边形分为两个三角形，顺时针依次遍历其索引。第n层实际只遍历了n-1个切片，每遍历一个切片，增加slices+1个索引
		if (levels > 2)
		{
			for (UINT i = 1; i < levels - 1; i++)
			{
				for (UINT j = 1; j <= slices; j++)
				{
					//四边形中右上方三角形
					meshData.indexVec[index++] = (i - 1) * (slices + 1) + j;
					meshData.indexVec[index++] = (i - 1) * (slices + 1) + j % (slices + 1) + 1;
					meshData.indexVec[index++] = i * (slices + 1) + j % (slices + 1) + 1;
					//四边形中左下方三角形
					meshData.indexVec[index++] = i * (slices + 1) + j % (slices + 1) + 1;
					meshData.indexVec[index++] = i * (slices + 1) + j;
					meshData.indexVec[index++] = (i - 1) * (slices + 1) + j;
				}
			}
		}

		//最后一层切片和南极点组成的多边形棱锥，每片三角形顺时针存入索引。
		//从中间层遍历到的最后一层切片开始到南极点，此时i=levels-2
		if (levels > 1)//有分层
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
		//每层切片slices+1个顶点，stacks层有stacks+1个切片
		//第一个最后一个切片，slieces个顶点每个索引3次，剩下的stcks-1个切片，slieces个顶点每个索引6次
		UINT vertexCount = (slices + 1) * (stacks + 1);
		UINT indexCount = 6 * stacks * slices;
		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		float halfHeight = height / 2.0f;//局部坐标原点设于桶中间
		float theta = 0.0f;//水平方向的转交
		float per_theta = XM_2PI / slices;//一层中每遍历一个顶点，水平方向转过的加角度
		float stackHeight = height / stacks;//层高

		Internal::VertexData vertexData;
		IndexType vertex = 0, index = 0;

		//自底向上铺设侧面端点
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

		//自底向上铺设侧面索引
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


	//无底（顶）盘圆柱基础上再加两层圆盘作为顶，底。
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

		float halfHeight = height / 2.0f;//局部坐标原点设于桶中间
		float theta = 0.0f;//水平方向的转交
		float per_theta = XM_2PI / slices;//一层中每遍历一个顶点，水平方向转过的加角度

		Internal::VertexData vertexData;
		IndexType vertex = (slices + 1) * (stacks + 1); //顶点起始量=无底圆柱的顶点总量
		IndexType index = 6 * stacks * slices; //索引起始量=无底圆柱的索引总量
		IndexType offset = vertex;

		//放入顶端圆心
		vertexData = { XMFLOAT3(0.0f,halfHeight,0.0f),XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(0.5f,0.5f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//放入顶端圆上各点
		for (UINT j = 0; j <= slices; j++)
		{
			theta = j * per_theta;
			float u = theta / XM_2PI;
			float v = 0.5f;
			vertexData = { XMFLOAT3(radius * cosf(theta),halfHeight,radius * sinf(theta)),XMFLOAT3(0.0f,1.0f,0.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),
			color,XMFLOAT2(u ,v ) };
			Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);
		}

		//放入底端圆心
		vertexData = { XMFLOAT3(0.0f,-halfHeight,0.0f),XMFLOAT3(0.0f,-1.0f,0.0f),XMFLOAT4(-1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(0.5f,0.5f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//放入底端圆上各点
		for (UINT j = 0; j <= slices; j++)
		{
			theta = j * per_theta;
			float u = theta / XM_2PI;
			float v = 0.5f;
			vertexData = { XMFLOAT3(radius * cosf(theta),-halfHeight,radius * sinf(theta)),XMFLOAT3(0.0f,-1.0f,0.0f),XMFLOAT4(-1.0f,0.0f,0.0f,1.0f),
			color,XMFLOAT2(u ,v) };
			Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);
		}

		//顶部三角形的索引,同球的北极层
		for (UINT i = 1; i <= slices; i++)
		{
			meshData.indexVec[index++] = offset;//圆心
			meshData.indexVec[index++] = offset + i % (slices + 1) + 1;
			meshData.indexVec[index++] = offset + i ;
		}

		//底部三角形的索引,同球的南极层
		offset += slices + 2;//一个圆盘算上圆心一共slices+2个
		for (UINT i = 1; i <= slices; i++)
		{
			meshData.indexVec[index++] = offset;//圆心
			meshData.indexVec[index++] = offset + i  ;
			meshData.indexVec[index++] = offset + i % (slices + 1)+1;
		}
		return meshData;
	}


	//该函数不包含底部的圆形面，注意圆锥的尖端因为各自所处的三角形法向量不一致，需要创建和底部圆相同的数目的顶点
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
		float length = sqrtf(height * height + radius * radius);//圆锥斜边长度
		UINT vertex = 0;
		UINT index = 0;

		Internal::VertexData vertexData;

		//放入圆锥尖端顶点(每个顶点包含不同的法向量和切线向量)
		for (UINT i = 0; i < slices; i++)
		{
			theta = i * per_theta + per_theta / 2.0f;
			vertexData = { XMFLOAT3(0.0f,halfHeight,0.0f), XMFLOAT3(radius * cosf(theta) / length, height / length, radius * sinf(theta) / length),
				XMFLOAT4(-sinf(theta),0.0f,cosf(theta),1.0f),color,XMFLOAT2(0.5f,0.5f) };
			Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);
		}

		//放入圆锥底面的顶点
		for (UINT i = 0; i < slices; i++)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(radius*cosf(theta),-halfHeight,radius*sinf(theta)), XMFLOAT3(radius * cosf(theta) / length, height / length, radius * sinf(theta) / length),
				XMFLOAT4(-sinf(theta),0.0f,cosf(theta),1.0f),color,XMFLOAT2(cosf(theta) / 2 + 0.5f, sinf(theta) / 2 + 0.5f) };
			Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);
		}

		//放入索引
		for (UINT i = 0; i < slices; i++)
		{
			meshData.indexVec[index++] = i;//锥顶各索引
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

		//放入底面圆的顶点
		for (UINT i = 0; i < slices; i++)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(radius*cosf(theta),-halfHeight,radius*sinf(theta)), XMFLOAT3(0.0f, -halfHeight, 0.0f),
				XMFLOAT4(-1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(cosf(theta) / 2 + 0.5f, sinf(theta) / 2 + 0.5f) };
			Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);
		}
		//圆心
		vertexData = { XMFLOAT3(0.0f, -halfHeight, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),
			  XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.5f, 0.5f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//放入索引
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

		//屏幕左下角
		vertexData = { XMFLOAT3(centerX - scaleX,centerY - scaleY,0.0f),XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(0.0f,1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//屏幕左上角
		vertexData = { XMFLOAT3(centerX - scaleX,centerY + scaleY,0.0f),XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(0.0f,0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//屏幕右上角
		vertexData = { XMFLOAT3(centerX + scaleX,centerY + scaleY,0.0f),XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),color,XMFLOAT2(1.0f,0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vertex++], vertexData);

		//屏幕右下角
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