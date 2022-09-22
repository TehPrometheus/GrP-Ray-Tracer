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
			/* 
				float	A{ Vector3::Dot(ray.direction,ray.direction) },
						B{ Vector3::Dot(2 * ray.direction,(ray.origin - sphere.origin)) },
						C{ Vector3::Dot((ray.origin - sphere.origin),(ray.origin - sphere.origin)) - Square(sphere.radius) },
						D{ Square(B) - 4 * A * C };

				float	t_0{ (-B - sqrtf(D)) / (2 * A) };

				Vector3 hitPoint{};
				Vector3 hitNormal{};

				if (D > 0)
				{
					hitRecord.didHit = true;
					hitRecord.origin = ray.origin;
					if (t_0 > ray.min && t_0 < ray.max)
					{
						hitRecord.t = t_0;
						hitPoint = ray.origin + t_0 * ray.direction;
						hitNormal = hitPoint - sphere.origin;
						hitRecord.normal = hitNormal.Normalized();
						return true;
					}
					else
					{
						float t_1{ (-B + sqrtf(D)) / (2 * A) };
						if (t_1 > ray.min && t_1 < ray.max)
						{
							hitRecord.t = t_1;
							hitPoint = ray.origin + t_1 * ray.direction;
							hitNormal = hitPoint - sphere.origin;
							hitRecord.normal = hitNormal.Normalized();
							return true;
						}
					}
				}
				else
				{
					hitRecord.didHit = false;
					return false;
				}

			*/

			// Geometric Solution
			Vector3 hitNormal{};
			Vector3 L{ sphere.origin - ray.origin };
			float dp{ Vector3::Dot(L,ray.direction) };
			float od_squared{ L.SqrMagnitude() - Square(dp) };

			if (od_squared < Square(sphere.radius))
			{
				float t_ca{ sqrtf(Square(sphere.radius) - od_squared) };
				hitRecord.didHit = true;
				hitRecord.origin = ray.origin;
				hitRecord.t = dp - t_ca;
				hitNormal = L + hitRecord.t * ray.direction; // looks weird, is more performant
				hitRecord.normal = hitNormal.Normalized();
			}
			else
			{
				hitRecord.didHit = false;
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
			//todo W1
			assert(false && "No Implemented Yet!");
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
			//todo W5
			assert(false && "No Implemented Yet!");
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
			assert(false && "No Implemented Yet!");
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
			//todo W3
			assert(false && "No Implemented Yet!");
			return {};
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3
			assert(false && "No Implemented Yet!");
			return {};
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