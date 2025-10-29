#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <map>


enum Pad3DAxis
{
	X_AXIS,
	Y_AXIS,
	Z_AXIS
};

namespace ImGui
{
	class Pad3D
	{
	public:
		Pad3D();

		Pad3D(const char* label, const char* labelX, const char* labelY, const char* labelZ,
			double* p_valueX, double* p_valueY, double* p_valueZ,
			const double* p_minX, const double* p_maxX,
			const double* p_minY, const double* p_maxY,
			const double* p_minZ, const double* p_maxZ);


		bool showPad3D();


	protected:
		const char* m_label = "";
		ImVec2 m_mousePosPad = ImVec2(-1.0f, -1.0f);
		ImRect m_grabBBX, m_grabBBY, m_grabBBZ, m_grabXY;
		const char* m_axis[3] = { "X", "Y", "Z" };
		std::map<const char*, const char*> m_mappedAxis = { {"X", "X"}, {"Y", "Y"}, {"Z", "Z"} };
		std::map<const char*, double*> m_values = { {"X", nullptr}, {"Y", nullptr}, {"Z", nullptr} };
		std::map<const char*, const double*> m_minValues = { {"X", nullptr}, {"Y", nullptr}, {"Z", nullptr} };
		std::map<const char*, const double*> m_maxValues = { {"X", nullptr}, {"Y", nullptr}, {"Z", nullptr} };
		std::map<const char*, ImRect> m_grabBBs = { {"X", ImRect()}, {"Y", ImRect()}, {"Z", ImRect()}};

		bool show1DPadSlider(char const* label, double* p_value, const double* p_min, const double* p_max, const ImRect& bb, const ImRect& containerBB, ImRect& grabBB, const ImGuiID& id, ImGuiWindow* window, ImGuiSliderFlags flags=NULL);
		void changeAxis(const char* axisLabel, int axisIndex);
		int getAxisIndex(const char* axis) { return std::distance(m_axis, std::find(m_axis, m_axis + 3, axis)); }
	};
}