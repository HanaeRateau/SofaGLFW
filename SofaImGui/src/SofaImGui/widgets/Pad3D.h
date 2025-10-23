#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS // import math operators
#include <imgui.h>

namespace ImGui
{
	bool Pad3D(char const* label, char const* labelX, char const* labelY, char const* labelZ, 
		double* p_valueX, double* p_valueY, double* p_valueZ, 
		const double* p_minX, const double* p_maxX, 
		const double* p_minY, const double* p_maxY,
		const double* p_minZ, const double* p_maxZ);
}