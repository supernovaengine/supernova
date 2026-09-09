// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "CopyFileCmd.h"

#include "util/Util.h"

using namespace doriax;

editor::CopyFileCmd::CopyFileCmd(Project* project, std::vector<std::string> sourceFiles, std::string currentDirectory, std::string targetDirectory, bool copy){
    this->project = project;
    for (const auto& sourceFile : sourceFiles) {
        FileCopyData fdata;
        fdata.filename = fs::path(sourceFile).filename();
        fdata.sourceDirectory = fs::path(currentDirectory);
        fdata.targetDirectory = fs::path(targetDirectory);

        this->files.push_back(fdata);
    }
    this->copy = copy;
}

editor::CopyFileCmd::CopyFileCmd(Project* project, std::vector<std::string> sourcePaths, std::string targetDirectory, bool copy){
    this->project = project;
    for (const auto& sourcePath : sourcePaths) {
        FileCopyData fdata;
        fdata.filename = fs::path(sourcePath).filename();
        fdata.sourceDirectory = fs::path(sourcePath).remove_filename();
        fdata.targetDirectory = fs::path(targetDirectory);

        this->files.push_back(fdata);
    }
    this->copy = copy;
}

bool editor::CopyFileCmd::execute(){
    bool touchedShaderSource = false;

    for (const auto& fdata : files) {
        fs::path sourceFs = fdata.sourceDirectory / fdata.filename;
        fs::path destFs = fdata.targetDirectory / fdata.filename;
        try {
            if (fs::exists(sourceFs)) {
                bool isDir = fs::is_directory(sourceFs);
                std::string extension = sourceFs.extension().string();
                // Copies count too: a .glsl arriving in a directory a fork resolves
                // against changes which file wins for it.
                touchedShaderSource |= isDir || Util::isShaderFile(extension);

                if (isDir) {
                    if (copy){
                        fs::copy(sourceFs, destFs, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
                    }else{
                        fs::rename(sourceFs, destFs);
                        if (project) {
                            project->remapMaterialFilePath(sourceFs, destFs);
                            project->remapSceneFilePath(sourceFs, destFs);
                            project->remapEntityBundleFilePath(sourceFs, destFs);
                            project->remapScriptFilePath(sourceFs, destFs);
                            project->remapAssetFilePath(sourceFs, destFs);
                            project->remapShaderFilePath(sourceFs, destFs);
                        }
                    }
                } else {
                    if (copy){
                        fs::copy(sourceFs, destFs, fs::copy_options::overwrite_existing);
                    }else{
                        fs::rename(sourceFs, destFs);
                        if (project) {
                            if (Util::isMaterialFile(extension)) {
                                project->remapMaterialFilePath(sourceFs, destFs);
                            }
                            if (Util::isSceneFile(extension)) {
                                project->remapSceneFilePath(sourceFs, destFs);
                            }
                            if (Util::isBundleFile(extension)) {
                                project->remapEntityBundleFilePath(sourceFs, destFs);
                            }
                            if (Util::isScriptFile(extension)) {
                                project->remapScriptFilePath(sourceFs, destFs);
                            }
                            if (Util::isAssetFile(extension)) {
                                project->remapAssetFilePath(sourceFs, destFs);
                            }
                            if (Util::isShaderFile(extension)) {
                                project->remapShaderFilePath(sourceFs, destFs);
                            }
                        }
                    }
                }
            }
        } catch (const fs::filesystem_error& e) {
            printf("Error: Moving/Copying %s: %s\n", sourceFs.string().c_str(), e.what());
            if (project && touchedShaderSource)
                project->invalidateCustomShaders();
            return false;
        }
    }

    if (project && touchedShaderSource)
        project->invalidateCustomShaders();

    return true;
}

void editor::CopyFileCmd::undo(){
    bool touchedShaderSource = false;

    for (const auto& fdata : files) {
        fs::path sourceFs = fdata.targetDirectory / fdata.filename;
        fs::path destFs = fdata.sourceDirectory / fdata.filename;
        try {
            if (fs::exists(sourceFs)) {
                bool isDir = fs::is_directory(sourceFs);
                std::string extension = sourceFs.extension().string();
                // Copies count too: a .glsl arriving in a directory a fork resolves
                // against changes which file wins for it.
                touchedShaderSource |= isDir || Util::isShaderFile(extension);

                if (isDir) {
                    if (copy) {
                        fs::remove_all(sourceFs);
                    }else{
                        fs::rename(sourceFs, destFs);
                        if (project) {
                            project->remapMaterialFilePath(sourceFs, destFs);
                            project->remapSceneFilePath(sourceFs, destFs);
                            project->remapEntityBundleFilePath(sourceFs, destFs);
                            project->remapScriptFilePath(sourceFs, destFs);
                            project->remapAssetFilePath(sourceFs, destFs);
                            project->remapShaderFilePath(sourceFs, destFs);
                        }
                    }
                } else {
                    if (copy) {
                        fs::remove(sourceFs);
                    }else{
                        fs::rename(sourceFs, destFs);
                        if (project) {
                            if (Util::isMaterialFile(extension)) {
                                project->remapMaterialFilePath(sourceFs, destFs);
                            }
                            if (Util::isSceneFile(extension)) {
                                project->remapSceneFilePath(sourceFs, destFs);
                            }
                            if (Util::isBundleFile(extension)) {
                                project->remapEntityBundleFilePath(sourceFs, destFs);
                            }
                            if (Util::isScriptFile(extension)) {
                                project->remapScriptFilePath(sourceFs, destFs);
                            }
                            if (Util::isAssetFile(extension)) {
                                project->remapAssetFilePath(sourceFs, destFs);
                            }
                            if (Util::isShaderFile(extension)) {
                                project->remapShaderFilePath(sourceFs, destFs);
                            }
                        }
                    }
                }
            }
        } catch (const fs::filesystem_error& e) {
            printf("Error: Undo moving/Copying %s: %s\n", sourceFs.string().c_str(), e.what());
        }
    }

    if (project && touchedShaderSource)
        project->invalidateCustomShaders();
}

bool editor::CopyFileCmd::mergeWith(editor::Command* otherCommand){
    return false;
}