#pragma once

#include <imgui.h>
#include <imgui_internal.h>

namespace ImGui
{
	class Pad3D
	{
	public:
		bool showPad3D(char const* label, char const* labelX, char const* labelY, char const* labelZ,
			double* p_valueX, double* p_valueY, double* p_valueZ,
			const double* p_minX, const double* p_maxX,
			const double* p_minY, const double* p_maxY,
			const double* p_minZ, const double* p_maxZ);

	protected:
		ImVec2 m_mousePosPad = ImVec2(-1.0f, -1.0f);
		ImRect m_grabBBX, m_granBBY, m_grabBBZ;
	};
}