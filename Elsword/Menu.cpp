#include "pch.h"

namespace Base
{
	void Menu::Draw()
	{
		if (g_GameVariables->m_ShowMenu)
			MainMenu();
	}

	void Menu::MainMenu()
	{
		// Get the current time
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);

		// Convert current time to a string
		char buffer[80];
		struct tm timeinfo;
		localtime_s(&timeinfo, &now_c);
		std::strftime(buffer, sizeof(buffer), "%m/%d/%Y", &timeinfo);

		ImGui::Begin("[ELSWORD] ZX LOADER (PREVIEW)", &g_GameVariables->m_ShowMenu);
		{
			ImGui::Text("BUILD VERSION: v1.0");
			ImGui::Text("BUILD DATE: %s", buffer);

			ImGui::Spacing();
			ImGui::Checkbox("DUMP", &g_GameData->bDUMP);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button("Choose File"))
			{
				IGFD::FileDialogConfig config;
				config.path = ".\\Script";
				config.countSelectionMax = 1;
				config.flags = ImGuiFileDialogFlags_Modal;
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".lua", config);
			}			

			if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					g_GameVariables->g_FilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
					g_GameVariables->g_FileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
				}

				ImGuiFileDialog::Instance()->Close();
			}

			ImGui::Spacing();
			ImGui::Text("File: %s", g_GameVariables->g_FileName.c_str());

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button("INJECT LUA", ImVec2(ImGui::GetContentRegionAvail().x, 20))) g_GameData->bINJECT = true;
			if (ImGui::Button("UNHOOK DLL", ImVec2(ImGui::GetContentRegionAvail().x, 20))) g_GameVariables->m_DetachDll = true;

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("Status: %s", g_GameVariables->g_Status.c_str());
		}
		ImGui::End();
	}
}