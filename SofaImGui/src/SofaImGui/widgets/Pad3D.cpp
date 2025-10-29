#define IMGUI_DEFINE_MATH_OPERATORS // import math operators
#include "Pad3D.h"

#include <IconsFontAwesome6.h>
#include <iostream>

namespace ImGui
{
	Pad3D::Pad3D()
	{
		m_label = "";
	}

	Pad3D::Pad3D(const char* label, const char* labelX, const char* labelY, const char* labelZ,
		double* p_valueX, double* p_valueY, double* p_valueZ,
		const double* p_minX, const double* p_maxX,
		const double* p_minY, const double* p_maxY,
		const double* p_minZ, const double* p_maxZ)
	{
		if (m_mappedAxis.find(labelX) == m_mappedAxis.end() || m_mappedAxis.find(labelY) == m_mappedAxis.end() || m_mappedAxis.find(labelZ) == m_mappedAxis.end())
		{
			throw std::invalid_argument("Labels must be X, Y or Z");
			return;
		}
		m_label = label;
		m_mappedAxis["X"] = labelX;
		m_mappedAxis["Y"] = labelY;
		m_mappedAxis["Z"] = labelZ;

		m_minValues["X"] = p_minX;
		m_minValues["Y"] = p_minY;
		m_minValues["Z"] = p_minZ;

		m_maxValues["X"] = p_maxX;
		m_maxValues["Y"] = p_maxY;
		m_maxValues["Z"] = p_maxZ;

		m_values["X"] = p_valueX;
		m_values["Y"] = p_valueY;
		m_values["Z"] = p_valueZ;
	}


	/**
	* This widget is composed of a 2D pad and a vertical slider for the remaining dimension
	**/
	bool Pad3D::showPad3D()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		IM_ASSERT((ImU64*)p_minX < (ImU64*)p_maxX);
		IM_ASSERT((ImU64*)p_minY < (ImU64*)p_maxY);


		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID idXY = window->GetID(m_label);
		ImGuiID idX = window->GetID(idXY);
		ImGuiID idY = window->GetID(idX);
		ImGuiID idZ = window->GetID(idY);

		// TODO: Move those to style
		double downScale = 0.75f;
		double dragX_placement = 0.1f;
		double dragY_placement = 0.1f;
		double dragX_thickness = 8.0f;
		double dragY_thickness = 8.0f;
		double border_thickness = 2.0f;
		double line_thickness = 2.0f;
		double text_lerp_x = 0.5f;
		double text_lerp_y = 0.5f;
		double cursor_radius = 4.0f;
		int cursor_segments = 4;
		ImVec4 vBlue(91.0f / 255.0f, 194.0f / 255.0f, 231.0f / 255.0f, 1.0f); // TODO: choose from style
		ImVec4 vOrange(255.0f / 255.0f, 128.0f / 255.0f, 64.0f / 255.0f, 1.0f); // TODO: choose from style
		ImU32 uBlue = ImGui::GetColorU32(ImGuiCol_FrameBgActive);
		ImU32 uOrange = ImGui::GetColorU32(ImGuiCol_Button);
		double fCursorOff = 16.0f;
		const double w = std::min(ImGui::GetWindowSize().x * downScale, 500.0);

		const ImRect total_bb(window->DC.CursorPos, ImVec2(window->WorkRect.Max.x, window->DC.CursorPos.y + w));
		const ImRect frame_bb(total_bb.Min + ImVec2((total_bb.GetWidth() - w) / 2.0f, 0.0f), window->DC.CursorPos + ImVec2((total_bb.GetWidth() - w) / 2.0f, 0.0f) + ImVec2(w, w));
		const ImRect frame_bb_drag(frame_bb.Min, frame_bb.Min + ImVec2(w * downScale, w * downScale));
		const ImRect frame_bb_dragX(ImVec2(frame_bb_drag.Min.x, ImLerp(frame_bb_drag.Max.y, frame_bb.Max.y, dragX_placement)),
			ImVec2(frame_bb_drag.Max.x, ImLerp(frame_bb_drag.Max.y, frame_bb.Max.y, dragX_placement) + dragX_thickness));
		const ImRect frame_bb_dragY(ImVec2(ImLerp(frame_bb_drag.Max.x, frame_bb.Max.x, dragY_placement), frame_bb_drag.Min.y),
			ImVec2(ImLerp(frame_bb_drag.Max.x, frame_bb.Max.x, dragY_placement) + dragY_thickness, frame_bb_drag.Max.y));
		const ImRect frame_bb_dragZ(ImVec2(frame_bb.Max.x - dragY_thickness, frame_bb_drag.Min.y),
			ImVec2(frame_bb.Max.x, frame_bb_drag.Max.y));

		double fXLimit = fCursorOff / frame_bb_drag.GetWidth();
		double fYLimit = fCursorOff / frame_bb_drag.GetHeight();

		const ImVec2 curPos = window->DC.CursorPos;

		ImGui::ItemSize(total_bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(total_bb, idXY, &frame_bb, 0))
			return false;


		// Show sliders
		show1DPadSlider(m_mappedAxis["X"], m_values["X"], m_minValues["X"], m_maxValues["X"], frame_bb_dragX, total_bb, m_grabBBX, idX, window);
		show1DPadSlider(m_mappedAxis["Y"], m_values["Y"], m_minValues["Y"], m_maxValues["Y"], frame_bb_dragY, total_bb, m_grabBBY, idY, window, ImGuiSliderFlags_Vertical);
		ImGui::SameLine();
		show1DPadSlider(m_mappedAxis["Z"], m_values["Z"], m_minValues["Z"], m_maxValues["Z"], frame_bb_dragZ, total_bb, m_grabBBZ, idZ, window, ImGuiSliderFlags_Vertical);
		

		// #region PAD
		bool hovered = ImGui::ItemHoverable(frame_bb_drag, idXY, g.LastItemData.ItemFlags);

		bool clicked = hovered && ImGui::IsMouseClicked(0, ImGuiInputFlags_None, idXY);
		bool make_active = (clicked || g.NavActivateId == idXY);
		if (make_active && clicked)
			ImGui::SetKeyOwner(ImGuiKey_MouseLeft, idXY);

		if (make_active)
		{
			ImGui::SetActiveID(idXY, window);
			ImGui::SetFocusID(idXY, window);
			ImGui::FocusWindow(window);
			g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
		}

		// Draw frame
		ImU32 frame_col = ImGui::GetColorU32(g.ActiveId == idXY ? ImGuiCol_FrameBgHovered : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		ImGui::RenderNavCursor(frame_bb_drag, idXY);
		ImGui::RenderFrame(frame_bb_drag.Min, frame_bb_drag.Max, frame_col, true, g.Style.FrameRounding);

		// Slider behavior
		double zero = 0.0f;
		double one = 1.0f;
		bool value_changedX = false;
		bool value_changedY = false;
		bool value_changedZ = false;

		if (ImGui::IsKeyPressed(ImGuiKey_LeftCtrl, false)) 
		{
			m_mousePosPad = ImGui::GetIO().MousePos; // save mouse position when pressing ctrl
			ImGui::TeleportMousePos(m_grabBBZ.GetCenter());

		}
		else if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
		{
			value_changedZ = ImGui::SliderBehavior(frame_bb_drag, idXY, ImGuiDataType_Double, m_values["Z"], m_minValues["Z"], m_maxValues["Z"], NULL, ImGuiSliderFlags_NoInput | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Vertical, &m_grabBBZ);
		}
		else
		{
			if (m_mousePosPad.x > 0)
			{
				ImGui::TeleportMousePos(ImVec2(m_grabXY.GetCenter().x, m_grabXY.GetCenter().y));
				m_mousePosPad = ImVec2(-1, -1);
			}

			value_changedX = ImGui::SliderBehavior(frame_bb_drag, idXY, ImGuiDataType_Double, m_values["X"], m_minValues["X"], m_maxValues["X"], NULL, ImGuiSliderFlags_NoInput | ImGuiSliderFlags_NoRoundToFormat, &m_grabXY);
			ImRect tempGrab(m_grabXY);
			value_changedY = ImGui::SliderBehavior(frame_bb_drag, idXY, ImGuiDataType_Double, m_values["Y"], m_minValues["Y"], m_maxValues["Y"], NULL, ImGuiSliderFlags_NoInput | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Vertical, &m_grabXY);
			m_grabXY.Min.x = tempGrab.Min.x;
			m_grabXY.Max.x = tempGrab.Max.x;
			if (value_changedX || value_changedY)
				ImGui::MarkItemEdited(idXY);
		}

		ImDrawList* pDrawList = window->DrawList;

		float s_delta_x = *m_maxValues["X"] - *m_minValues["X"];
		float s_delta_y = *m_maxValues["Y"] - *m_minValues["Y"];
		float fScaleX = (*m_values["X"] - *m_minValues["X"]) / s_delta_x;
		float fScaleY = 1.0f - ((*m_values["Y"] - *m_minValues["Y"]) / s_delta_y);
		ImVec2 vCursorPos((frame_bb_drag.Max.x - frame_bb_drag.Min.x) * fScaleX + frame_bb_drag.Min.x, (frame_bb_drag.Max.y - frame_bb_drag.Min.y) * fScaleY + frame_bb_drag.Min.y);

		char const* formatX = ImGui::DataTypeGetInfo(ImGuiDataType_Double)->PrintFmt;
		char const* formatY = ImGui::DataTypeGetInfo(ImGuiDataType_Double)->PrintFmt;

		// Cursor
		pDrawList->AddCircleFilled(vCursorPos, cursor_radius, uBlue, cursor_segments);

		// Vertical Line
		if (fScaleY > 2.0f * fYLimit)
			pDrawList->AddLine(ImVec2(vCursorPos.x, frame_bb_drag.Min.y + fCursorOff), ImVec2(vCursorPos.x, vCursorPos.y - fCursorOff), uOrange, line_thickness);
		if (fScaleY < 1.0f - 2.0f * fYLimit)
			pDrawList->AddLine(ImVec2(vCursorPos.x, frame_bb_drag.Max.y - fCursorOff), ImVec2(vCursorPos.x, vCursorPos.y + fCursorOff), uOrange, line_thickness);

		// Horizontal Line
		if (fScaleX > 2.0f * fXLimit)
			pDrawList->AddLine(ImVec2(frame_bb_drag.Min.x + fCursorOff, vCursorPos.y), ImVec2(vCursorPos.x - fCursorOff, vCursorPos.y), uOrange, line_thickness);
		if (fScaleX < 1.0f - 2.0f * fYLimit)
			pDrawList->AddLine(ImVec2(frame_bb_drag.Max.x - fCursorOff, vCursorPos.y), ImVec2(vCursorPos.x + fCursorOff, vCursorPos.y), uOrange, line_thickness);

		// Borders::Right
		pDrawList->AddCircleFilled(ImVec2(frame_bb_drag.Max.x, vCursorPos.y), 2.0f, uOrange, 3);
		// Handle Right::Y
		if (fScaleY > fYLimit)
			pDrawList->AddLine(ImVec2(frame_bb_drag.Max.x, frame_bb_drag.Min.y), ImVec2(frame_bb_drag.Max.x, vCursorPos.y - fCursorOff), uBlue, border_thickness);
		if (fScaleY < 1.0f - fYLimit)
			pDrawList->AddLine(ImVec2(frame_bb_drag.Max.x, frame_bb_drag.Max.y), ImVec2(frame_bb_drag.Max.x, vCursorPos.y + fCursorOff), uBlue, border_thickness);
		// Borders::Top
		pDrawList->AddCircleFilled(ImVec2(vCursorPos.x, frame_bb_drag.Min.y), 2.0f, uOrange, 3);
		if (fScaleX > fXLimit)
			pDrawList->AddLine(ImVec2(frame_bb_drag.Min.x, frame_bb_drag.Min.y), ImVec2(vCursorPos.x - fCursorOff, frame_bb_drag.Min.y), uBlue, border_thickness);
		if (fScaleX < 1.0f - fXLimit)
			pDrawList->AddLine(ImVec2(frame_bb_drag.Max.x, frame_bb_drag.Min.y), ImVec2(vCursorPos.x + fCursorOff, frame_bb_drag.Min.y), uBlue, border_thickness);
		// Borders::Left
		pDrawList->AddCircleFilled(ImVec2(frame_bb_drag.Min.x, vCursorPos.y), 2.0f, uOrange, 3);
		if (fScaleY > fYLimit)
			pDrawList->AddLine(ImVec2(frame_bb_drag.Min.x, frame_bb_drag.Min.y), ImVec2(frame_bb_drag.Min.x, vCursorPos.y - fCursorOff), uBlue, border_thickness);
		if (fScaleY < 1.0f - fYLimit)
			pDrawList->AddLine(ImVec2(frame_bb_drag.Min.x, frame_bb_drag.Max.y), ImVec2(frame_bb_drag.Min.x, vCursorPos.y + fCursorOff), uBlue, border_thickness);
		// Borders::Bottom
		pDrawList->AddCircleFilled(ImVec2(vCursorPos.x, frame_bb_drag.Max.y), 2.0f, uOrange, 3);
		// Handle Bottom::X
		if (fScaleX > fXLimit)
			pDrawList->AddLine(ImVec2(frame_bb_drag.Min.x, frame_bb_drag.Max.y), ImVec2(vCursorPos.x - fCursorOff, frame_bb_drag.Max.y), uBlue, border_thickness);
		if (fScaleX < 1.0f - fXLimit)
			pDrawList->AddLine(ImVec2(frame_bb_drag.Max.x, frame_bb_drag.Max.y), ImVec2(vCursorPos.x + fCursorOff, frame_bb_drag.Max.y), uBlue, border_thickness);
	

		// #endregion PAD

		window->DC.CursorPosPrevLine = total_bb.Max;
		window->DC.CursorPos = total_bb.Max;

		return value_changedX || value_changedY || value_changedZ;
	}


	bool Pad3D::show1DPadSlider(char const* label, double* p_value, const double* p_min, const double* p_max, const ImRect& bb, const ImRect& containerBB, ImRect& grabBB, const ImGuiID& id, ImGuiWindow* window, ImGuiSliderFlags flags)
	{
		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		ImU32 const uTextCol = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]);

		ImGui::PushStyleColor(ImGuiCol_SliderGrab, GetColorU32(ImGuiCol_FrameBgActive));

		if (!ImGui::ItemAdd(containerBB, id, &bb, 0))
			return false;

		bool value_change = false;

		bool hovered = ImGui::ItemHoverable(bb, id, g.LastItemData.ItemFlags);

		bool clicked = hovered && ImGui::IsMouseClicked(0, ImGuiInputFlags_None, id);
		bool make_active = (clicked || g.NavActivateId == id);
		if (make_active && clicked)
			ImGui::SetKeyOwner(ImGuiKey_MouseLeft, id);

		if (make_active)
		{
			ImGui::SetActiveID(id, window);
			ImGui::SetFocusID(id, window);
			ImGui::FocusWindow(window);
			g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
		}
		ImU32 frame_col1 = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);

		ImGui::RenderNavCursor(bb, id);
		ImGui::RenderFrame(bb.Min, bb.Max, frame_col1, true, g.Style.FrameRounding);

		// Render grab
		value_change = ImGui::SliderBehavior(bb, id, ImGuiDataType_Double, p_value, p_min, p_max, NULL, ImGuiSliderFlags_NoInput | ImGuiSliderFlags_NoRoundToFormat | flags, &grabBB);
		window->DrawList->AddRectFilled(grabBB.Min, grabBB.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

		// Add Buttons
		window->DC.CursorPos = ((flags & ImGuiSliderFlags_Vertical) == ImGuiSliderFlags_Vertical) ? ImVec2(grabBB.Max.x + style.FramePadding.x, grabBB.GetCenter().y) : ImVec2(grabBB.GetCenter().x, grabBB.Max.y);
		bool showOtherAxis = false;
		std::cout << " Resulting axis " << m_mappedAxis["X"] << ", " << m_mappedAxis["Y"] << ", " << m_mappedAxis["Z"] << std::endl;
		if (ImGui::Button(label))
			showOtherAxis = true;

		auto idPopup = "##ChangeAxis" + std::string(label);
		if (showOtherAxis)
		{
			ImGui::OpenPopup(idPopup.c_str());
		}

		if (ImGui::BeginPopup(idPopup.c_str()))
		{
			for (int i = 0; i < 3; i++)
			{
				if (m_axis[i] != label)
					if (Selectable(m_axis[i]))
						changeAxis(label, i);
			}
			ImGui::EndPopup();
		}

		ImGui::PopStyleColor();
		return value_change;
	}


	void Pad3D::changeAxis(const char* axisLabel, int axisIndex)
	{
		auto swappingAxis = m_axis[axisIndex];
		auto index = getAxisIndex(axisLabel);

		if (index < 3 && index >= 0)
		{
			auto axis = m_axis[index];
			std::cout << " Swapping axis " << axis << " with " << swappingAxis << axisIndex << std::endl;
			std::cout << " axis state " << m_mappedAxis["X"] << ", " << m_mappedAxis["Y"] << ", " << m_mappedAxis["Z"] << std::endl;

			// swap the values
			auto tmpValue = m_minValues[swappingAxis];
			m_minValues[swappingAxis] = m_minValues[axisLabel];
			m_minValues[axisLabel] = tmpValue;

			tmpValue = m_maxValues[swappingAxis];
			m_maxValues[swappingAxis] = m_maxValues[axisLabel];
			m_maxValues[axisLabel] = tmpValue;

			auto tmp = m_values[swappingAxis];
			m_values[swappingAxis] = m_values[axis];
			m_values[axis] = tmp;


			// swap the axisLabel and axisIndex axis
			auto tmpAxis = m_mappedAxis[swappingAxis];
			m_mappedAxis[swappingAxis] = m_mappedAxis[axis];
			m_mappedAxis[axis] = tmpAxis;

			std::cout << " Resulting axis " << m_mappedAxis["X"] << ", " << m_mappedAxis["Y"] << ", " << m_mappedAxis["Z"] << std::endl;
		}

	}
}