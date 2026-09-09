// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include "imgui.h"
#include "external/IconsFontAwesome6.h"
#include "Vector2.h"

namespace fs = std::filesystem;

namespace doriax::editor{
    class Widgets{

    private:
        inline static std::string shortenPath(const fs::path& path, float maxWidth) {
            std::string fullPath = path.string();
        
            ImVec2 fullPathSize = ImGui::CalcTextSize(fullPath.c_str());
            if (fullPathSize.x <= maxWidth) {
                return fullPath;
            }
        
            std::string filename = path.filename().string();
            std::string parentPath = path.parent_path().string();
        
            ImVec2 filenameSize = ImGui::CalcTextSize(filename.c_str());
            if (filenameSize.x > maxWidth) {
                std::string truncatedFilename = filename;
                while (!truncatedFilename.empty() && ImGui::CalcTextSize((truncatedFilename + "...").c_str()).x > maxWidth) {
                    truncatedFilename.pop_back();
                }
                return truncatedFilename + "...";
            }
        
            float remainingWidth = maxWidth - filenameSize.x - ImGui::CalcTextSize("/").x; // Space for '/' separator
        
            if (parentPath != ".") {
                std::string truncatedParentPath = parentPath;
                while (!truncatedParentPath.empty() && ImGui::CalcTextSize((truncatedParentPath + ".../").c_str()).x > remainingWidth) {
                    truncatedParentPath.pop_back();
                }
        
                return truncatedParentPath + ".../" + filename;
            } else {
                return filename;
            }
        }

    public:
        // Square/compact icon button whose glyph is always centered. A plain
        // button is drawn at the requested size and the icon is painted on top,
        // centered, so it never clips or shifts when the button is narrower than
        // the glyph plus frame padding. `strId` should be label-less (e.g. "##foo").
        inline static bool iconButton(const char* strId, const char* icon, const ImVec2& size){
            ImVec2 pos = ImGui::GetCursorScreenPos();
            bool pressed = ImGui::Button(strId, size);
            ImVec2 textSize = ImGui::CalcTextSize(icon);
            ImGui::GetWindowDrawList()->AddText(
                ImVec2(pos.x + (size.x - textSize.x) * 0.5f, pos.y + (size.y - textSize.y) * 0.5f),
                ImGui::GetColorU32(ImGuiCol_Text), icon);
            return pressed;
        }

        // Image draws for textures that may not be created yet. A null texture id
        // does not unbind anything, so the quad keeps sampling what the previous
        // draw command bound (the font atlas) instead of staying empty. The item
        // is still reserved so layout and drop targets don't move.
        inline static void image(ImTextureID texture, const ImVec2& size, ImU32 emptyColor = 0){
            if (texture != ImTextureID{}){
                ImGui::Image(texture, size);
                return;
            }
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImGui::Dummy(size);
            if ((emptyColor & IM_COL32_A_MASK) != 0){
                ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), emptyColor);
            }
        }

        inline static void addImage(ImDrawList* drawList, ImTextureID texture, const ImVec2& pMin, const ImVec2& pMax){
            if (texture == ImTextureID{}) return;
            drawList->AddImage(texture, pMin, pMax);
        }

        inline static void addImageRounded(ImDrawList* drawList, ImTextureID texture, const ImVec2& pMin, const ImVec2& pMax, const ImVec2& uvMin, const ImVec2& uvMax, ImU32 col, float rounding, ImDrawFlags flags){
            if (texture == ImTextureID{}) return;
            drawList->AddImageRounded(texture, pMin, pMax, uvMin, uvMax, col, rounding, flags);
        }

        inline static void pathDisplay(const char* id, fs::path path, const Vector2& size = Vector2::ZERO, fs::path basePath = fs::path()){
            ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));
            ImGui::BeginChild(id, ImVec2(size.x, size.y), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            
            std::string subPath = path.string();
            if (!basePath.empty()){
                if (subPath.find(basePath.string()) == 0) {
                    subPath = subPath.substr(basePath.string().length());
                    if (subPath.empty()){
                        subPath = "/";
                    }
                }
            }else{
                subPath = subPath.empty() ? "<No path selected>" : subPath;
            }

            std::string shortenedPath = shortenPath(subPath, ImGui::GetContentRegionAvail().x);
        
            ImGui::SetCursorPosY(ImGui::GetStyle().FramePadding.y);
            ImGui::Text("%s", ((shortenedPath == ".") ? "" : shortenedPath).c_str());
        
            ImGui::EndChild();
            if (!path.empty()){
                ImGui::SetItemTooltip("%s", path.string().c_str());
            }
            ImGui::PopStyleColor();
        }
    };

}