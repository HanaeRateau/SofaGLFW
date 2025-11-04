#define IMGUI_DEFINE_MATH_OPERATORS // import math operators
#include "MovePad.h"
#include "Buttons.h"

#include <IconsFontAwesome6.h>
#include <iostream>
#include <string>
#include <format>

namespace ImGui
{
	MovePad::MovePad()
	{
		m_label = "";
	}

	MovePad::MovePad(const char* label, const char* labelX, const char* labelY, const char* labelZ,
		double* p_valueX, double* p_valueY, double* p_valueZ,
		const double* p_minX, const double* p_maxX,
		const double* p_minY, const double* p_maxY,
		const double* p_minZ, const double* p_maxZ)
	{

		m_label = label;
		m_mappedAxis["PadH"] = labelX;
		m_mappedAxis["PadV"] = labelY;
		m_mappedAxis["Slider"] = labelZ;

		m_minValues["PadH"] = *p_minX;
		m_minValues["PadV"] = *p_minY;
		m_minValues["Slider"] = *p_minZ;

		m_maxValues["PadH"] = *p_maxX;
		m_maxValues["PadV"] = *p_maxY;
		m_maxValues["Slider"] = *p_maxZ;

		m_values["PadH"] = p_valueX;
		m_values["PadV"] = p_valueY;
		m_values["Slider"] = p_valueZ;
	}


	/**
	* This widget is composed of a 2D pad and a vertical slider for the remaining dimension
	**/
	bool MovePad::showPad3D()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID idXY = window->GetID(m_label);
		ImGuiID idX = window->GetID(idXY);
		ImGuiID idY = window->GetID(idX);
		ImGuiID idZ = window->GetID(idY);

		// TODO: Move those to style
		float grabThickness = style.ScrollbarSize / 5.f;
		float grabRadius = grabThickness * 2.5f;
		double downScale = 0.8f;
		double dragX_placement = 0.1f;
		double dragY_placement = 0.1f;
		double dragX_thickness = grabThickness;
		double dragY_thickness = grabThickness;
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
		const double w = std::min(window->WorkRect.GetWidth(), ImGui::GetWindowSize().y);
		const auto slidersRegionWidth = GetFrameHeight() * 8;


		const ImRect total_bb(window->DC.CursorPos, ImVec2(window->WorkRect.Max.x, window->DC.CursorPos.y + w - slidersRegionWidth));
		//ImGui::RenderFrame(total_bb.Min, total_bb.Max, GetColorU32(ImGuiCol_Button), true, g.Style.FrameRounding);
		
		const ImVec2 containerSize = ImVec2(total_bb.GetWidth() * downScale, total_bb.GetHeight() - GetFrameHeight()*2.5);
		const ImRect frame_bb(total_bb.GetCenter() - ImVec2( containerSize.x/2.0 , containerSize.y / 2.0 ),
			total_bb.GetCenter() + ImVec2(containerSize.x / 2.0, containerSize.y / 2.0));
		//ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_ButtonHovered), true, g.Style.FrameRounding);

		int padSize = frame_bb.GetWidth() - slidersRegionWidth;
		padSize = std::min(frame_bb.GetHeight(), std::max((float)padSize, frame_bb.GetWidth() - slidersRegionWidth));
		auto padwidth = padSize + slidersRegionWidth + GetFrameHeight();
		const ImRect frame_bb_drag(frame_bb.GetCenter() - ImVec2(padwidth/2. - GetFrameHeight(), padSize / 2.),
			frame_bb.GetCenter() - ImVec2(padwidth / 2. - GetFrameHeight(), padSize / 2.) + ImVec2(padSize, padSize));

		const ImRect frame_bb_dragX(ImVec2(frame_bb_drag.Min.x, frame_bb_drag.Max.y+style.FramePadding.y * 2),
			ImVec2(frame_bb_drag.Max.x, frame_bb_drag.Max.y + style.FramePadding.y * 2 + dragX_thickness));
		const ImRect frame_bb_dragY(ImVec2(frame_bb_drag.Max.x+style.FramePadding.x * 2, frame_bb_drag.Min.y),
			ImVec2(frame_bb_drag.Max.x + style.FramePadding.x * 2 + dragY_thickness, frame_bb_drag.Max.y));
		const ImRect frame_bb_dragZ(ImVec2(frame_bb_dragY.Max.x + slidersRegionWidth/2., frame_bb_drag.Min.y),
			ImVec2(frame_bb_dragY.Max.x + slidersRegionWidth/2. + dragY_thickness, frame_bb_drag.Max.y));

		double fXLimit = fCursorOff / frame_bb_drag.GetWidth();
		double fYLimit = fCursorOff / frame_bb_drag.GetHeight();


		ImGui::ItemSize(total_bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(total_bb, idXY, &frame_bb, 0))
			return false;		

		// Show sliders
		// PadH
		window->DC.CursorPos = (ImVec2(frame_bb_dragX.Min.x - GetFrameHeight() - style.FramePadding.x , frame_bb_dragX.GetCenter().y - GetFrameHeight()/2));
		if (Button(ICON_FA_ARROWS_LEFT_RIGHT"##PadH", ImVec2(GetFrameHeight(), GetFrameHeight())))
		{
			m_flippedAxis["PadH"] = !m_flippedAxis["PadH"];
		}
		show1DPadSlider(m_mappedAxis["PadH"], m_values["PadH"],
			(m_flippedAxis["PadH"])?&m_maxValues["PadH"]:& m_minValues["PadH"], 
			(m_flippedAxis["PadH"])?&m_minValues["PadH"]:& m_maxValues["PadH"], frame_bb_dragX, total_bb, m_grabBBX, idX, window);

		// PadV
		window->DC.CursorPos = (ImVec2(frame_bb_dragY.GetCenter().x - GetFrameHeight()/2, frame_bb_dragY.Min.y - GetFrameHeight() - style.FramePadding.y));
		if (Button(ICON_FA_ARROWS_UP_DOWN"##PadV", ImVec2(GetFrameHeight(), GetFrameHeight())))
		{
			m_flippedAxis["PadV"] = !m_flippedAxis["PadV"];
		}
		show1DPadSlider(m_mappedAxis["PadV"], m_values["PadV"], 
			(m_flippedAxis["PadV"]) ? &m_maxValues["PadV"] : &m_minValues["PadV"],
			(m_flippedAxis["PadV"]) ? &m_minValues["PadV"] : &m_maxValues["PadV"], frame_bb_dragY, total_bb, m_grabBBY, idY, window, ImGuiSliderFlags_Vertical);

		// Slider
		window->DC.CursorPos = (ImVec2(frame_bb_dragZ.GetCenter().x - GetFrameHeight()/2, frame_bb_dragZ.Min.y - GetFrameHeight() - style.FramePadding.y));
		if (Button(ICON_FA_ARROWS_UP_DOWN"##Slider", ImVec2(GetFrameHeight(), GetFrameHeight())))
		{
			m_flippedAxis["Slider"] = !m_flippedAxis["Slider"];
		}
		show1DPadSlider(m_mappedAxis["Slider"], m_values["Slider"], 
			(m_flippedAxis["Slider"]) ? &m_maxValues["Slider"] : &m_minValues["Slider"],
			(m_flippedAxis["Slider"]) ? &m_minValues["Slider"] : &m_maxValues["Slider"], frame_bb_dragZ, total_bb, m_grabBBZ, idZ, window, ImGuiSliderFlags_Vertical);

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
			value_changedZ = ImGui::SliderBehavior(frame_bb_drag, idXY, ImGuiDataType_Double, m_values["Slider"], 
				(m_flippedAxis["Slider"]) ? &m_maxValues["Slider"] : &m_minValues["Slider"],
				(m_flippedAxis["Slider"]) ? &m_minValues["Slider"] : &m_maxValues["Slider"], NULL, ImGuiSliderFlags_NoInput | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Vertical, &m_grabBBZ);
		}
		else
		{
			if (m_mousePosPad.x > 0)
			{
				ImGui::TeleportMousePos(ImVec2(m_grabXY.GetCenter().x, m_grabXY.GetCenter().y));
				m_mousePosPad = ImVec2(-1, -1);
			}

			value_changedX = ImGui::SliderBehavior(frame_bb_drag, idXY, ImGuiDataType_Double, m_values["PadH"], 
				(m_flippedAxis["PadH"]) ? &m_maxValues["PadH"] : &m_minValues["PadH"], 
				(m_flippedAxis["PadH"]) ? &m_minValues["PadH"] : &m_maxValues["PadH"], NULL, ImGuiSliderFlags_NoInput | ImGuiSliderFlags_NoRoundToFormat, &m_grabXY);
			ImRect tempGrab(m_grabXY);
			value_changedY = ImGui::SliderBehavior(frame_bb_drag, idXY, ImGuiDataType_Double, m_values["PadV"], 
				(m_flippedAxis["PadV"]) ? &m_maxValues["PadV"] : &m_minValues["PadV"], 
				(m_flippedAxis["PadV"]) ? &m_minValues["PadV"] : &m_maxValues["PadV"], NULL, ImGuiSliderFlags_NoInput | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Vertical, &m_grabXY);
			m_grabXY.Min.x = tempGrab.Min.x;
			m_grabXY.Max.x = tempGrab.Max.x;
			if (value_changedX || value_changedY)
				ImGui::MarkItemEdited(idXY);
		}

		ImDrawList* pDrawList = window->DrawList;

		float s_delta_x = m_maxValues["PadH"] - m_minValues["PadH"];
		float s_delta_y = m_maxValues["PadV"] - m_minValues["PadV"];
		float fScaleX = (*m_values["PadH"] - m_minValues["PadH"]) / s_delta_x;
		float fScaleY = 1.0f - ((*m_values["PadV"] - m_minValues["PadV"]) / s_delta_y);
		ImVec2 vCursorPos(m_grabBBX.GetCenter().x, m_grabBBY.GetCenter().y);

		char const* formatX = ImGui::DataTypeGetInfo(ImGuiDataType_Double)->PrintFmt;
		char const* formatY = ImGui::DataTypeGetInfo(ImGuiDataType_Double)->PrintFmt;

		// Cursor
		//pDrawList->AddCircleFilled(vCursorPos, cursor_radius, uBlue, cursor_segments);
		window->DrawList->AddCircleFilled(vCursorPos, grabRadius * 1.2f, GetColorU32(g.ActiveId == idXY ? ImGuiCol_SliderGrabActive : ImGuiCol_Button));
		window->DrawList->AddCircleFilled(vCursorPos, grabRadius, GetColorU32(ImGuiCol_SliderGrab));

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
		
		window->DC.CursorPos = frame_bb_dragX.GetBL() + ImVec2(0., GetFrameHeight());
		PushStyleColor(ImGuiCol_Text, GetColorU32(ImGuiCol_TextDisabled));
		Text("Press Ctrl to move the third dimension");
		PopStyleColor();

		window->DC.CursorPosPrevLine = total_bb.Max;
		window->DC.CursorPos = total_bb.Max;

		NewLine();



		return value_changedX || value_changedY || value_changedZ;
	}


	bool MovePad::show1DPadSlider(char const* label, double* p_value, const double* p_min, const double* p_max, const ImRect& bb, const ImRect& containerBB, ImRect& grabBB, const ImGuiID& id, ImGuiWindow* window, ImGuiSliderFlags flags)
	{
		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		ImU32 const uTextCol = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]);
		float grabThickness = style.ScrollbarSize / 5.f;
		float grabRadius = grabThickness * 2.5f;

		if (!ImGui::ItemAdd(containerBB, id, &bb, 0))
			return false;

		bool value_change = false;

		ImRect expandedBB(bb.Min-ImVec2(grabRadius, grabRadius), bb.Max+ ImVec2(grabRadius, grabRadius));
		bool hovered = ImGui::ItemHoverable( expandedBB, id, g.LastItemData.ItemFlags);

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
		window->DrawList->AddCircleFilled(grabBB.GetCenter(), grabRadius * 1.2f, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_Button));
		window->DrawList->AddCircleFilled(grabBB.GetCenter(), grabRadius, GetColorU32(ImGuiCol_SliderGrab));

		// Add Button
		window->DC.CursorPos = ((flags & ImGuiSliderFlags_Vertical) == ImGuiSliderFlags_Vertical) ? ImVec2(grabBB.Max.x + 2*style.FramePadding.x, grabBB.GetCenter().y - GetFrameHeight() / 2.0f) : ImVec2(grabBB.GetCenter().x-GetFrameHeight()/2.0f, grabBB.Max.y);
		bool showOtherAxis = false;

		PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		PushStyleColor(ImGuiCol_ButtonText, GetColorU32(ImGuiCol_Text));
		ImGui::AlignTextToFramePadding();
		const ImVec4& color = ImVec4(label=="X"?1.0f:0.0f, label == "Y" ? 1.0f : 0.0f, label == "Z" ? 1.0f : 0.0f, 1.0f);
		ImVec2 size(1.0f, ImGui::GetFrameHeight	()/2.);
		ImGui::GetWindowDrawList()->AddRectFilled(window->DC.CursorPos + ImVec2(0.0f, size.y / 2.),
			window->DC.CursorPos + ImVec2(0.0f, size.y / 2.) +size,
			ImGui::GetColorU32(color), ImGuiStyleVar_FrameRounding); // draw colored axis line in before button
		
		if (ImGui::Button((std::string(label)+ " " + ICON_FA_CARET_DOWN).c_str(), ImVec2(GetFrameHeight(), GetFrameHeight())))
		{
			showOtherAxis = true;
		}
		PopStyleColor(4);
		SameLine();
		window->DC.CursorPos -= ImVec2(style.FramePadding.x, 0.);
		Text(std::format("{:.3f}", *p_value).c_str());

		// Add popup
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
						swapAxis(label, i);
			}
			ImGui::EndPopup();
		}

		return value_change;
	}


	void MovePad::swapAxis(const char* axisLabel, int axisIndexToSwap)
	{
		auto axis = getMappedAxis(axisLabel);
		auto swappingAxis = getMappedAxis(m_axis[axisIndexToSwap]);

		//swap the values
		auto tmpValue = m_minValues[swappingAxis];
		m_minValues[swappingAxis] = m_minValues[axis];
		m_minValues[axis] = tmpValue;

		tmpValue = m_maxValues[swappingAxis];
		m_maxValues[swappingAxis] = m_maxValues[axis];
		m_maxValues[axis] = tmpValue;

		auto tmp = m_values[swappingAxis];
		m_values[swappingAxis] = m_values[axis];
		m_values[axis] = tmp;


		// swap the axisLabel and axisIndex axis
		auto tmpAxis = m_mappedAxis[swappingAxis];
		m_mappedAxis[swappingAxis] = m_mappedAxis[axis];
		m_mappedAxis[axis] = tmpAxis;
	}


	const char* MovePad::getMappedAxis(const char* axis)
	{
		for (auto [key, value] : m_mappedAxis)
		{
			if (value == axis)
				return key;
		}
	}
}