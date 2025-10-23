#include "Pad3D.h"

#include <imgui_internal.h>
#include <iostream>

namespace ImGui
{
	/**
	* This widget is composed of a 2D pad and a vertical slider for the remaining dimension
	**/
	bool Pad3D(char const* label, char const* labelX, char const* labelY, char const* labelZ,
				double* p_valueX, double* p_valueY, double* p_valueZ, 
				const double* p_minX, const double* p_maxX, 
				const double* p_minY, const double* p_maxY, 
				const double* p_minZ, const double* p_maxZ)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		IM_ASSERT((ImU64*)p_minX < (ImU64*)p_maxX);
		IM_ASSERT((ImU64*)p_minY < (ImU64*)p_maxY);

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		ImGuiID idX = window->GetID(id);
		ImGuiID idY = window->GetID(idX);
		const double w = ImGui::CalcItemWidth()/2;

		ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		// TODO: Move those to style
		double downScale = 0.75f;
		double dragX_placement = 0.75f;
		double dragY_placement = 0.75f;
		double dragX_thickness = 8.0f;
		double dragY_thickness = 8.0f;
		double border_thickness = 2.0f;
		double line_thickness = 2.0f;
		double text_lerp_x = 0.5f;
		double text_lerp_y = 0.5f;
		double cursor_radius = 4.0f;
		int cursor_segments = 4;
		//ImVec4 vBlue( 70.0f / 255.0f, 102.0f / 255.0f, 230.0f / 255.0f, 1.0f ); // TODO: choose from style
		ImVec4 vBlue(91.0f / 255.0f, 194.0f / 255.0f, 231.0f / 255.0f, 1.0f); // TODO: choose from style
		ImVec4 vOrange(255.0f / 255.0f, 128.0f / 255.0f, 64.0f / 255.0f, 1.0f); // TODO: choose from style
		ImU32 uBlue = ImGui::GetColorU32(ImGuiCol_FrameBgActive);
		ImU32 uOrange = ImGui::GetColorU32(ImGuiCol_Button);
		double fCursorOff = 16.0f;

		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, w));
		const ImRect frame_bb_drag(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w * downScale, w * downScale));
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(0.0f, label_size.y + style.FramePadding.y));
		const ImRect frame_bb_dragX(ImVec2(frame_bb_drag.Min.x, ImLerp(frame_bb_drag.Max.y, frame_bb.Max.y, dragX_placement)),
			ImVec2(frame_bb_drag.Max.x, ImLerp(frame_bb_drag.Max.y, frame_bb.Max.y, dragX_placement) + dragX_thickness));
		const ImRect frame_bb_dragY(ImVec2(ImLerp(frame_bb_drag.Max.x, frame_bb.Max.x, dragY_placement), frame_bb_drag.Min.y),
			ImVec2(ImLerp(frame_bb_drag.Max.x, frame_bb.Max.x, dragY_placement) + dragY_thickness, frame_bb_drag.Max.y));

		double fXLimit = fCursorOff / frame_bb_drag.GetWidth();
		double fYLimit = fCursorOff / frame_bb_drag.GetHeight();

		const ImVec2 curPos = window->DC.CursorPos;

		ImGui::ItemSize(total_bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(total_bb, id, &frame_bb, 0))
			return false;

		bool hovered = ImGui::ItemHoverable(frame_bb_drag, id, g.LastItemData.ItemFlags);

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

		// Draw frame
		ImU32 frame_col = ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgHovered : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		ImGui::RenderNavCursor(frame_bb_drag, id);
		ImGui::RenderFrame(frame_bb_drag.Min, frame_bb_drag.Max, frame_col, true, g.Style.FrameRounding);

		// Slider behavior
		ImRect grab_bbX;
		ImRect grab_bbY;
		double zero = 0.0f;
		double one = 1.0f;
		bool value_changedX = ImGui::SliderBehavior(frame_bb_drag, id, ImGuiDataType_Double, p_valueX, p_minX, p_maxX, NULL, ImGuiSliderFlags_NoInput | ImGuiSliderFlags_NoRoundToFormat, &grab_bbX);
		bool value_changedY = ImGui::SliderBehavior(frame_bb_drag, id, ImGuiDataType_Double, p_valueY, p_minY, p_maxY, NULL, ImGuiSliderFlags_NoInput | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Vertical, &grab_bbY);
		if (value_changedX || value_changedY)
			ImGui::MarkItemEdited(id);

		// Bottom slider
		//if (!ImGui::ItemAdd(total_bb, idX, &frame_bb_dragX, 0))
		//	return false;

		//hovered = ImGui::ItemHoverable(frame_bb_dragX, idX, g.LastItemData.ItemFlags);

		//clicked = hovered && ImGui::IsMouseClicked(0, ImGuiInputFlags_None, idX);
		//make_active = (clicked || g.NavActivateId == idX);
		//if (make_active && clicked)
		//	ImGui::SetKeyOwner(ImGuiKey_MouseLeft, idX);

		//if (make_active)
		//{
		//	ImGui::SetActiveID(idX, window);
		//	ImGui::SetFocusID(idX, window);
		//	ImGui::FocusWindow(window);
		//	g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
		//}
		//frame_col = ImGui::GetColorU32(g.ActiveId == idX ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		//bool value_changedXS = ImGui::SliderBehavior(frame_bb_dragX, idX, ImGuiDataType_Double, p_valueX, p_minX, p_maxX, NULL, ImGuiSliderFlags_NoInput | ImGuiSliderFlags_NoRoundToFormat, &grab_bbX);
		//if (value_changedX)
		//	ImGui::MarkItemEdited(idX);

		//ImGui::RenderNavCursor(frame_bb_dragX, idX);
		//ImGui::RenderFrame(frame_bb_dragX.Min, frame_bb_dragX.Max, frame_col, true, g.Style.FrameRounding);

		// Right slider
		//if (!ImGui::ItemAdd(total_bb, idY, &frame_bb_dragY, 0))
		//	return false;

		//hovered = ImGui::ItemHoverable(frame_bb_dragY, idY, g.LastItemData.ItemFlags);

		//clicked = hovered && ImGui::IsMouseClicked(0, ImGuiInputFlags_None, idY);
		//make_active = (clicked || g.NavActivateId == idY);
		//if (make_active && clicked)
		//	ImGui::SetKeyOwner(ImGuiKey_MouseLeft, idY);

		//if (make_active)
		//{
		//	ImGui::SetActiveID(idY, window);
		//	ImGui::SetFocusID(idY, window);
		//	ImGui::FocusWindow(window);
		//	g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
		//}
		//frame_col = ImGui::GetColorU32(g.ActiveId == idY ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);

		//ImGui::RenderNavCursor(frame_bb_dragY, idY);
		//ImGui::RenderFrame(frame_bb_dragY.Min, frame_bb_dragY.Max, frame_col, true, g.Style.FrameRounding);

		//bool value_changedYS = ImGui::SliderBehavior(frame_bb_dragY, idY, ImGuiDataType_Double, p_valueY, p_minY, p_maxY, NULL, ImGuiSliderFlags_NoInput | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Vertical, &grab_bbY);

		if (label_size.x > 0.0f)
			ImGui::RenderText(ImVec2(frame_bb_drag.Min.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

		// Remaining dimension slider
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, GetColorU32(ImGuiCol_FrameBgActive));
		{
			if (!ImGui::ItemAdd(total_bb, idY, &frame_bb_dragY, 0))
				return false;

			hovered = ImGui::ItemHoverable(frame_bb_dragY, idY, g.LastItemData.ItemFlags);

			clicked = hovered && ImGui::IsMouseClicked(0, ImGuiInputFlags_None, idY);
			make_active = (clicked || g.NavActivateId == idY);
			if (make_active && clicked)
				ImGui::SetKeyOwner(ImGuiKey_MouseLeft, idY);

			if (make_active)
			{
				ImGui::SetActiveID(idY, window);
				ImGui::SetFocusID(idY, window);
				ImGui::FocusWindow(window);
				g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
			}
			frame_col = ImGui::GetColorU32(g.ActiveId == idY || hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBgHovered);

			ImGui::RenderNavCursor(frame_bb_dragY, idY);
			ImGui::RenderFrame(frame_bb_dragY.Min, frame_bb_dragY.Max, frame_col, true, g.Style.FrameRounding);

			// Render grab
			ImRect grab_bbZ;
			bool value_changedYS = ImGui::SliderBehavior(frame_bb_dragY, idY, ImGuiDataType_Double, p_valueZ, p_minZ, p_maxZ, NULL, ImGuiSliderFlags_NoInput | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Vertical, &grab_bbZ);	
			window->DrawList->AddRectFilled(grab_bbZ.Min, grab_bbZ.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

			ImGui::RenderText(ImVec2(frame_bb_dragY.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), labelZ);
		}
		ImGui::PopStyleColor();

		ImDrawList* pDrawList = window->DrawList;

		float s_delta_x = *p_maxX - *p_minX;
		float s_delta_y = *p_maxY - *p_minY;
		float fScaleX = (*p_valueX - *p_minX) / s_delta_x;
		float fScaleY = 1.0f - ((*p_valueY - *p_minY) / s_delta_y);
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
		//pDrawList->AddNgonFilled(ImVec2(frame_bb_dragY.GetCenter().x, vCursorPos.y), dragY_thickness * 0.5f, uOrange, 4);
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
		//pDrawList->AddNgonFilled(ImVec2(vCursorPos.x, frame_bb_dragX.GetCenter().y), dragX_thickness * 0.5f, uOrange, 4);
		if (fScaleX > fXLimit)
			pDrawList->AddLine(ImVec2(frame_bb_drag.Min.x, frame_bb_drag.Max.y), ImVec2(vCursorPos.x - fCursorOff, frame_bb_drag.Max.y), uBlue, border_thickness);
		if (fScaleX < 1.0f - fXLimit)
			pDrawList->AddLine(ImVec2(frame_bb_drag.Max.x, frame_bb_drag.Max.y), ImVec2(vCursorPos.x + fCursorOff, frame_bb_drag.Max.y), uBlue, border_thickness);

		// Add Text
		char pBufferX[64];
		char pBufferY[64];
		ImGui::DataTypeFormatString(pBufferX, IM_ARRAYSIZE(pBufferX), ImGuiDataType_Double, p_valueX, formatX);
		ImGui::DataTypeFormatString(pBufferY, IM_ARRAYSIZE(pBufferY), ImGuiDataType_Double, p_valueY, formatY);

		ImU32 const uTextCol = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]);

		ImGui::SetWindowFontScale(0.75f);

		ImVec2 const vXSize = ImGui::CalcTextSize(labelX);
		ImVec2 const vYSize = ImGui::CalcTextSize(labelY);

		pDrawList->AddText(
			ImVec2(
				ImClamp(vCursorPos.x - vXSize.x * 0.5f, frame_bb_drag.Min.x, frame_bb_drag.Min.x + frame_bb_drag.GetWidth() - vXSize.x),
				ImLerp(frame_bb_drag.Max.y, frame_bb_dragX.Min.y - vXSize.y, text_lerp_x)),
			uTextCol,
			labelX);
		pDrawList->AddText(
			ImVec2(ImLerp(frame_bb_drag.Max.x, frame_bb_dragY.Min.x - vYSize.x, text_lerp_y),
				ImClamp(vCursorPos.y - vXSize.y * 0.5f, frame_bb_drag.Min.y, frame_bb_drag.Min.y + frame_bb_drag.GetHeight() - vYSize.y)),
			uTextCol,
			labelY);

		ImGui::SetWindowFontScale(1.0f);

		return value_changedX || value_changedY; // || value_changedXS || value_changedYS;
	}
}