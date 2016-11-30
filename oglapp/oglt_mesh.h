#pragma once

#include "std_util.h"
#include "oglt_util.h"
#include "oglt_material.h"
#include <glm/glm.hpp>

namespace oglt {
	enum MaterialMappingMode {
		BY_POLYGON, ALL_SAME, NONE
	};

	struct Triangle {
		uint vertexIndices[3];
		uint startIndex;
		uint materialIndex;
	};

	class Mesh {
	public:
		Mesh();
		~Mesh();

		void sortTrianglesByMtl();

		uint startIndex;
		uint size;
		uint materialIndex;
		vector<Triangle> triangles;
		vector<Material> materials;

		MaterialMappingMode mtlMapMode;
	private:
	};
}