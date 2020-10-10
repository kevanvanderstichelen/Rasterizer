#include "TriangleMesh.h"

TriangleMesh::TriangleMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indexBuffer, Texture* texture, const float rotationSpeed)
	:Object{ texture }
	, m_WorldVertices{ vertices }
	, m_IndexBuffer{ indexBuffer }
	, m_RotationSpeed { rotationSpeed }

{
	m_SignedArea.resize(indexBuffer.size());
}

const bool TriangleMesh::Intersect(const FPoint2& pixel, float& depthZ, RGBColor& color, Vertex& V0, Vertex& V1, Vertex& V2, FVector3& n, int idx, const FVector3& viewDirection) const
{
	//Check if the pixel is in the triangle & calculate weights for texture coordinates
	FVector2 pointToSide = pixel - V0.pos.xy;
	FVector2 edge = V1.pos.xy - V0.pos.xy;
	float crossPointEdge = Cross(pointToSide, edge);
	if (crossPointEdge < 0.f) return false;
	const float signedArea = Cross(V2.pos.xy - V0.pos.xy, V1.pos.xy - V0.pos.xy);
	const float W2 = crossPointEdge / signedArea;

	pointToSide = pixel - V1.pos.xy;
	edge = V2.pos.xy - V1.pos.xy;
	crossPointEdge = Cross(pointToSide, edge);
	if (crossPointEdge < 0.f) return false;
	const float W0 = crossPointEdge / signedArea;

	pointToSide = pixel - V2.pos.xy;
	edge = V0.pos.xy - V2.pos.xy;
	crossPointEdge = Cross(pointToSide, edge);
	if (crossPointEdge < 0.f) return false;
	const float W1 = crossPointEdge / signedArea;

	//Interpolated variables (depth, normal, uv's)
	depthZ = 1.f / (((1.f / V0.pos.z) * W0) + ((1.f / V1.pos.z) * W1) + ((1.f / V2.pos.z) * W2));
	const float depthW = 1.f / (((1.f / V0.pos.w) * W0) + ((1.f / V1.pos.w) * W1) + ((1.f / V2.pos.w) * W2));
	const FVector2 uvInterpolated = (((V0.uv / V0.pos.w) * W0) + ((V1.uv / V1.pos.w) * W1) + ((V2.uv / V2.pos.w) * W2)) * depthW;
	const FVector3 tInterpolated = (((V0.tangent / V0.pos.w) * W0) + ((V1.tangent / V1.pos.w) * W1) + ((V2.tangent / V2.pos.w) * W2)) * depthW;
	n = (((V0.normal / V0.pos.w) * W0) + ((V1.normal / V1.pos.w) * W1) + ((V2.normal / V2.pos.w) * W2)) * depthW;
	//Final texture color
	color = GetTexture()->Sample(uvInterpolated, n, tInterpolated, viewDirection);

	return true;
}

void TriangleMesh::Update(float elapsedSec)
{
	//Update Position/Normal/Tangents of the mesh when rotating
	for (Vertex& v : m_WorldVertices)
	{
		v.pos.xyz = FPoint3(MakeRotationY(m_RotationSpeed * elapsedSec) * FVector3(v.pos.xyz));
		v.normal = MakeRotationY(m_RotationSpeed * elapsedSec) * v.normal;
		v.tangent = MakeRotationY(m_RotationSpeed * elapsedSec) * v.tangent;
	}
}

const std::vector<Vertex>& TriangleMesh::GetWorldVertices() const
{
	return m_WorldVertices;
}

const std::vector<unsigned int>& TriangleMesh::GetIndexBuffers() const
{
	return m_IndexBuffer;
}

TriangleMesh* TriangleMesh::LoadFromFile(const std::string& fileName, const FVector3& pos, Texture* texture, const float rotationSpeed)
{
	std::ifstream file{ fileName };
	std::vector<Vertex> vertices;
	std::vector<FVector2> tempUv;
	std::vector<FVector3> tempNorm;
	std::vector<FPoint3> tempPos;
	std::vector<unsigned int> indexBuffer;

	std::regex regex_V("^v\\s*(\\-?\\d*\\.\\d*) (\\-?\\d*\\.?\\d*) (\\-?\\d*\\.?\\d*)");
	std::regex regex_F("^f (\\d*)\\S(\\d*)\\S(\\d*) (\\d*)\\S(\\d*)\\S(\\d*) (\\d*)\\S(\\d*)\\S(\\d*) ");
	std::regex regex_VT("^vt (\\d\\.\\d+) (\\d\\.\\d+) \\S*");
	std::regex regex_VN("^vn (-?\\d*.\\d*) (-?\\d*.\\d*) (-?\\d*.\\d*)");
	std::smatch match;
	
	//Counter to check if vertexBuffer size is smaller than available VN or UV coords
	int counter = 0;

	if (!file.eof())
	{
		std::string line;

		while (std::getline(file, line))
		{
			if (line.empty()) continue;

			//Get values of vertex position
			if (regex_match(line, regex_V))
			{
				if (regex_search(line, match, regex_V))
				{
					tempPos.push_back(FPoint3(std::stof(match[1].str()) + pos.x, std::stof(match[2].str()) + pos.y, std::stof(match[3].str()) + pos.z));
					continue;
				}
			}

			//Get values of uv coordinates
			if (regex_match(line, regex_VT))
			{
				if (regex_search(line, match, regex_VT))
				{
					tempUv.push_back(FVector2(std::stof(match[1].str()), std::stof(match[2].str())));
					continue;
				}
			}

			//Get values of vertex normals
			if (regex_match(line, regex_VN))
			{
				if (regex_search(line, match, regex_VN))
				{
					tempNorm.push_back(FVector3(std::stof(match[1].str()), std::stof(match[2].str()), std::stof(match[3].str())));
					continue;
				}
			}

			//Get values of faces
			if (regex_match(line, regex_F))
			{
				if (regex_search(line, match, regex_F))
				{
					
					Vertex one;
					one.pos = tempPos[unsigned(std::stoi(match[1].str()) - 1)];
					one.uv = tempUv[unsigned(std::stoi(match[2].str()) - 1)];
					one.normal = tempNorm[unsigned(std::stoi(match[3].str()) - 1)];
					Vertex two;
					two.pos = tempPos[unsigned(std::stoi(match[4].str()) - 1)];
					two.uv = tempUv[unsigned(std::stoi(match[5].str()) - 1)];
					two.normal = tempNorm[unsigned(std::stoi(match[6].str()) - 1)];
					Vertex three;
					three.pos = tempPos[unsigned(std::stoi(match[7].str()) - 1)];
					three.uv = tempUv[unsigned(std::stoi(match[8].str()) - 1)];
					three.normal = tempNorm[unsigned(std::stoi(match[9].str()) - 1)];

					vertices.push_back(one);
					indexBuffer.push_back(counter);
					counter++;
					vertices.push_back(two);
					indexBuffer.push_back(counter);
					counter++;
					vertices.push_back(three);
					indexBuffer.push_back(counter);
					counter++;

					continue;
				}
			}

		}
	}

	for (uint32_t i = 0; i < indexBuffer.size(); i += 3)
	{
		uint32_t index0 = indexBuffer[i];
		uint32_t index1 = indexBuffer[i + 1];
		uint32_t index2 = indexBuffer[i + 2];

		const FPoint3& p0 = FPoint3(vertices[index0].pos);
		const FPoint3& p1 = FPoint3(vertices[index1].pos);
		const FPoint3& p2 = FPoint3(vertices[index2].pos);
		const FVector2& uv0 = vertices[index0].uv;
		const FVector2& uv1 = vertices[index1].uv;
		const FVector2& uv2 = vertices[index2].uv;

		const FVector3 edge0 = p1 - p0;
		const FVector3 edge1 = p2 - p0;
		const FVector2 diffX = FVector2(uv1.x - uv0.x, uv2.x - uv0.x);
		const FVector2 diffY = FVector2(uv1.y - uv0.y, uv2.y - uv0.y);
		float r = 1.f / Cross(diffX, diffY);

		FVector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
		vertices[index0].tangent += tangent;
		vertices[index1].tangent += tangent;
		vertices[index2].tangent += tangent;
	}

	for (auto& v : vertices)
	{
		v.tangent = GetNormalized(Reject(v.tangent, v.normal));
	}


	return new TriangleMesh(vertices, indexBuffer, texture, rotationSpeed);
}