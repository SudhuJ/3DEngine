#pragma once

#include "context/context.h"

struct ResourceWindow : IWidget {
    FLOW_INLINE ResourceWindow(GuiContext* context) :IWidget(context) {
        m_IconImage.Load("resources/icons/asset.png");
        m_Icon = (ImTextureID)m_IconImage.getID();
    }

    FLOW_INLINE void onShow(GuiContext* context) override {
        if(ImGui::Begin(ICON_FA_FOLDER_OPEN "\tResources"))
		{
			int nCols = (ImGui::GetContentRegionAvail().x/ASSET_SIZE) + 1;
			int columnCounter = 1;
			int rowCounter = 1;

			context->AssetView([&] (auto* asset) {
				if(columnCounter++ <= rowCounter * nCols)
					ImGui::SameLine();
				else
					rowCounter++;

				// show asset icon
				bool isClicked = ImGui::ImageButtonEx(asset->UID,
				m_Icon, ImVec2(ASSET_SIZE, ASSET_SIZE), ImVec2(0, 1),
				ImVec2(1, 0), ImVec4(0, 0, 0, 1), ImVec4(1, 1, 1, 1));
			});
        }
        ImGui::End();
    }

    private:
        texture2D m_IconImage;
        ImTextureID m_Icon;
        AssetID m_Selected;
};
