#pragma once
#include <raylib.h>
#include <raymath.h>
class CRayTrace
{
public:
	static bool CheckLineOfSight(Vector3 vecStartPos, Vector3 vecEndPos);
};