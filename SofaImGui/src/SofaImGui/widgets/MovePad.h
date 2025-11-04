#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <map>

namespace ImGui
{
	class MovePad
	{
	public:
		MovePad();

		MovePad(const char* label, const char* labelX, const char* labelY, const char* labelZ,
			double* p_valueX, double* p_valueY, double* p_valueZ,
			const double* p_minX, const double* p_maxX, 
			const double* p_minY, const double* p_maxY,
			const double* p_minZ, const double* p_maxZ);


		bool showPad3D();
		void setBounds(const char* axis, const double& min, const double& max) { m_minValues[getMappedAxis(axis)] = min; m_maxValues[getMappedAxis(axis)] = max; }


	protected:
		const char* m_label = "";
		ImVec2 m_mousePosPad = ImVec2(-1.0f, -1.0f);
		ImRect m_grabBBX, m_grabBBY, m_grabBBZ, m_grabXY;
		const char* m_axis[3] = { "X", "Y", "Z" };
		std::map<const char*, bool> m_flippedAxis = { {"PadH", false}, {"PadV", false}, {"Slider", false} };
		std::map<const char*, const char*> m_mappedAxis = { {"PadH", "X"}, {"PadV", "Y"}, {"Slider", "Z"} };
		std::map<const char*, double*> m_values = { {"PadH", nullptr}, {"PadV", nullptr}, {"Slider", nullptr} };
		std::map<const char*, double> m_minValues = { {"PadH", 0.0}, {"PadV", 0.0}, {"Slider", 0.0} };
		std::map<const char*, double> m_maxValues = { {"PadH", 0.0}, {"PadV", 0.0}, {"Slider", 0.0} };
		std::map<const char*, ImRect> m_grabBBs = { {"PadH", ImRect()}, {"PadV", ImRect()}, {"Slider", ImRect()}};

		bool show1DPadSlider(char const* label, double* p_value, const double* p_min, const double* p_max, const ImRect& bb, const ImRect& containerBB, ImRect& grabBB, const ImGuiID& id, ImGuiWindow* window, ImGuiSliderFlags flags=NULL);
		void swapAxis(const char* axisLabel, int axisIndex);
		const char* getMappedAxis(const char* axis);
	};
}