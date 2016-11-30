#include "oglt_mesh.h"

oglt::Mesh::Mesh()
{
	startIndex = 0;
	size = 0;
	materialIndex = 0;
	mtlMapMode = NONE;
}

oglt::Mesh::~Mesh()
{
}

void oglt::Mesh::sortTrianglesByMtl()
{
}
