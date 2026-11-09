#pragma once

static const CVertex RectVerts[] = 
{
	//							Position			Normal					UV				Color
	/*	0	*/	CVertex(		Vec3(0, 0, 0),		Vec3(0.0, 0.0, -1.0),	0.0, 1.0,		Color(1.0f, 1.0f, 1.0f, 1.0f)),
	/*	1	*/	CVertex(		Vec3(1, 0, 0),		Vec3(0.0, 0.0, -1.0),	1.0, 1.0,		Color(1.0f, 1.0f, 1.0f, 1.0f)),
	/*	2	*/	CVertex(		Vec3(1, 1, 0),		Vec3(0.0, 0.0, -1.0),	1.0, 0.0,		Color(1.0f, 1.0f, 1.0f, 1.0f)),
	/*	3	*/	CVertex(		Vec3(0, 1, 0),		Vec3(0.0, 0.0, -1.0),	0.0, 0.0,		Color(1.0f, 1.0f, 1.0f, 1.0f)),
};

static const WORD RectIndices[] =
{
 	0, 1, 3,
 	3, 1, 2,

	//1, 2, 4,
	//2, 5, 4,

	//3, 4, 6,
	//4, 7, 6,

	//4, 5, 7,
	//5, 8, 7
};