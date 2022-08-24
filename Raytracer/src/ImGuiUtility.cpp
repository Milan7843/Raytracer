#include "ImGuiUtility.h"

void ImGuiUtility::drawHelpMarker(const char* desc)
{
	// Draw the help marker after whatever has already been drawn on this line
	ImGui::SameLine();

	// Gray text [?]
	ImGui::TextDisabled("[?]");

	// Drawing the help marker on hover
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
