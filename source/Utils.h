#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1 DONE
			// Analytical Solution
				//float	A{ Vector3::Dot(ray.direction,ray.direction) },
				//		B{ Vector3::Dot(2 * ray.direction,(ray.origin - sphere.origin)) },
				//		C{ Vector3::Dot((ray.origin - sphere.origin),(ray.origin - sphere.origin)) - Square(sphere.radius) },
				//		D{ Square(B) - 4 * A * C };

				//float	t_0{ (-B - sqrtf(D)) / (2 * A) };

				//Vector3 hitPoint{};
				//Vector3 hitNormal{};

				//if (D > 0)
				//{
				//	hitRecord.didHit = true;
				//	if (t_0 > ray.min && t_0 < ray.max)
				//	{
				//		hitRecord.t = t_0;
				//		hitRecord.origin = ray.origin + t_0 * ray.direction;
				//		hitPoint = ray.origin + t_0 * ray.direction;
				//		hitNormal = hitPoint - sphere.origin;
				//		hitRecord.normal = hitNormal.Normalized();
				//		hitRecord.materialIndex = sphere.materialIndex;
				//		return true;
				//	}
				//	else
				//	{
				//		float t_1{ (-B + sqrtf(D)) / (2 * A) };
				//		if (t_1 > ray.min && t_1 < ray.max)
				//		{
				//			hitRecord.t = t_1;
				//			hitRecord.origin = ray.origin + t_1 * ray.direction;
				//			hitPoint = ray.origin + t_1 * ray.direction;
				//			hitNormal = hitPoint - sphere.origin;
				//			hitRecord.normal = hitNormal.Normalized();
				//			hitRecord.materialIndex = sphere.materialIndex;
				//			return true;
				//		}
				//	}
				//}
				//else
				//{
				//	hitRecord.didHit = false;
				//	return false;
				//}
			

			// Geometric Solution
			Vector3 hitNormal{};
			Vector3 L{ sphere.origin - ray.origin };
			float dp{ Vector3::Dot(L,ray.direction) };

			float od_squared{ L.SqrMagnitude() - (dp*dp) };

			if (od_squared < (sphere.radius * sphere.radius) )
			{
				float t_ca{ sqrtf(Square(sphere.radius) - od_squared) };
				if ((dp - t_ca) > ray.min && (dp - t_ca) < ray.max)
				{
					if (!ignoreHitRecord)
					{
						hitRecord.didHit = true;
						hitRecord.t = dp - t_ca;
						hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
						hitNormal = hitRecord.origin - sphere.origin;
						hitRecord.materialIndex = sphere.materialIndex;
						hitRecord.normal = hitNormal.Normalized();
					}
					return true;

				}

				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}


			
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1 DONE
			float t{ Vector3::Dot((plane.origin - ray.origin),plane.normal) / Vector3::Dot(ray.direction,plane.normal)};
			if (t > ray.min && t < ray.max)
			{
				hitRecord.didHit = true;
				hitRecord.materialIndex = plane.materialIndex;
				hitRecord.normal = plane.normal;
				hitRecord.origin = ray.origin + t * ray.direction;
				hitRecord.t = t;
				return true;

			}
			hitRecord.didHit = false;
			return false;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5 DONE
			constexpr float epsilon{ 0.001f };
			const float dot{ Vector3::Dot(ray.direction,triangle.normal) };


			//check if we hit the plane the triangle lives in
			if (abs(dot) >= epsilon)
			{
				const Vector3 L{triangle.v0 - ray.origin};
				const float t{Vector3::Dot(L, triangle.normal) / Vector3::Dot(ray.direction, triangle.normal)};
				if (t > ray.min && t < ray.max)
				{
					const Vector3 p {ray.origin + t * ray.direction};

					//check if we hit the triangle
					Vector3 edge {triangle.v1 - triangle.v0};
					Vector3 pointToSide{ p - triangle.v0 };

					//First edge check
					if (Vector3::Dot(triangle.normal,Vector3::Cross(edge,pointToSide)) > 0.f)
					{
						edge = triangle.v2 - triangle.v1;
						pointToSide =  p - triangle.v1;
						//Second edge check
						if (Vector3::Dot(triangle.normal, Vector3::Cross(edge, pointToSide)) > 0.f)
						{
							edge = triangle.v0 - triangle.v2;
							pointToSide = p - triangle.v2;
							//Third edge check
							if (Vector3::Dot(triangle.normal, Vector3::Cross(edge, pointToSide)) > 0.f)
							{
								//check culling mode
								switch (triangle.cullMode)
								{
								case TriangleCullMode::NoCulling:
									if (ignoreHitRecord == false)
									{
										hitRecord.didHit = true;
										hitRecord.normal = triangle.normal;
										hitRecord.materialIndex = triangle.materialIndex;
										hitRecord.origin = ray.origin + t * ray.direction;
										hitRecord.t = t;
									}
									break;
								case TriangleCullMode::FrontFaceCulling:
									if (ignoreHitRecord == true)
									{
										if (dot < 0)
										{
											return true;
										}
										if (dot > 0)
										{
											return false;
										}
									}
									if (ignoreHitRecord == false)
									{
										if (dot < 0)
										{
											return false;
										}
										if (dot > 0)
										{
											hitRecord.didHit = true;
											hitRecord.normal = triangle.normal;
											hitRecord.materialIndex = triangle.materialIndex;
											hitRecord.origin = ray.origin + t * ray.direction;
											hitRecord.t = t;
											return true;
										}
									}
									break;
								case TriangleCullMode::BackFaceCulling:
									if (ignoreHitRecord == true)
									{
										if (dot < 0)
										{
											return true;
										}
										if (dot > 0)
										{
											return false;
										}
									}
									if (ignoreHitRecord == false)
									{
										if (dot < 0)
										{
											hitRecord.didHit = true;
											hitRecord.normal = triangle.normal;
											hitRecord.materialIndex = triangle.materialIndex;
											hitRecord.origin = ray.origin + t * ray.direction;
											hitRecord.t = t;
											return true;
										}
										if (dot > 0)
										{
											return false;
										}
									}
									break;
								default:
									break;
								}
								return true;
							}
						}
					}
				}
			}
			hitRecord.didHit = false;
			return false;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5 
			Triangle triangle{};
			int normalIdx{};

			triangle.cullMode		= mesh.cullMode;
			triangle.materialIndex	= mesh.materialIndex;

			for (size_t index = 0; index < mesh.indices.size(); ++index)
			{
				triangle.v0				= mesh.transformedPositions[mesh.indices[  index]];
				triangle.v1				= mesh.transformedPositions[mesh.indices[++index]];
				triangle.v2				= mesh.transformedPositions[mesh.indices[++index]];
				triangle.normal			= mesh.transformedNormals[normalIdx];
				++normalIdx;

				if (HitTest_Triangle(triangle, ray, hitRecord, ignoreHitRecord))
				{
					return true;
				}

			}
			return false;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			//todo W2 DONE
			if (light.type != LightType::Directional) 
			{
				return { light.origin - origin };
			}
			return {};
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3 DONE
			if (light.type == LightType::Point)
			{
				return (light.color * (light.intensity / ((light.origin - target).SqrMagnitude())));
			}
			else if (light.type == LightType::Directional)
			{
				return (light.color * (light.intensity));
			}
			else
			{
				return {};
			}
		}

		inline float GetLambertCosine(const Vector3& surfaceNormal, const Vector3& lightDirectionNormalized)
		{
			const float temp{ Vector3::Dot(surfaceNormal, lightDirectionNormalized) };
			constexpr float epsilon{ 0.01f }; //might need to tweak this value
			if (temp > epsilon)
			{
				return temp;
			}
			else
			{
				return 0.f;
			}
		}


	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}