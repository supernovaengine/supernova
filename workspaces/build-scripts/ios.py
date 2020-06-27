#!/usr/bin/env python

# /*
# (c) 2020 Eduardo Doria.
# */

import os
import sys
import subprocess
import stat
import click
import uuid
from shutil import rmtree

def resolve_path(rel_path):
    return os.path.abspath(os.path.join(os.path.dirname(__file__), rel_path))

def rmtree_silent(root):
    def remove_readonly_handler(fn, root, excinfo):
        if fn is os.rmdir:
            if os.path.isdir(root): # if exists
                os.chmod(root, stat.S_IWRITE) # make writable
                os.rmdir(root)
        elif fn is os.remove:
            if os.path.isfile(root): # if exists
                os.chmod(root, stat.S_IWRITE) # make writable
                os.remove(root)
    rmtree(root, onerror=remove_readonly_handler)

def makedirs_silent(root):
    try:
        os.makedirs(root)
    except OSError: # mute if exists
        pass

##
# Based on Steve Robinson comment in https://gitlab.kitware.com/cmake/cmake/-/issues/19588
# https://gitlab.com/ssrobins/sdl2-example/blob/cmake_issues_19588/add_xcode_folder_reference.py
##
def add_folder_reference(project, folder_path, target):
    
    with open(project) as f:
        project_data = f.read()
        
    folder = os.path.basename(folder_path)
    
    id1 = str(uuid.uuid4().hex)
    id2 = str(uuid.uuid4().hex)
    
    pbxbuildfile_header = '/* Begin PBXBuildFile section */'
    project_data = project_data.replace(
        pbxbuildfile_header,
        '{0}\n\t\t{1} /* {2} in Resources */ = {{isa = PBXBuildFile; fileRef = {3} /* {2} */; }};'.format(pbxbuildfile_header, id1, folder, id2),
    )

    if folder.endswith(".xcassets"):
        last_known_file_type = "folder.assetcatalog"
    else:
        last_known_file_type = "folder"
    
    pbxfilereference_header = '/* Begin PBXFileReference section */'
    project_data = project_data.replace(
        pbxfilereference_header,
        '{0}\n\t\t{1} /* {2} */ = {{isa = PBXFileReference; lastKnownFileType = {3}; name = {2}; path = {4}; sourceTree = "<group>"; }};'.format(pbxfilereference_header, id2, folder, last_known_file_type, folder_path)
    )
    
    # Get <RESOURCES_PBXGROUP_ID> for 'Resources' in <TARGET>:
    # <PBXGROUP_ID> /* <TARGET> */ = {
    #        isa = PBXGroup;
    #        children = (
    #            <RESOURCES_PBXGROUP_ID> /* Resources */,
    index_pbx_group = project_data.find('/* {0} */ = {{\n\t\t\tisa = PBXGroup;'.format(target))
    index_resources_child = project_data.find('/* Resources */', index_pbx_group)
    index_start_resources_pbxgroup = project_data.rfind('\t', 0, index_resources_child)
    resources_pbxgroup_id = project_data[index_start_resources_pbxgroup:index_resources_child].strip()
    
    pbxgroup_section = '{0} /* Resources */ = {{\n\t\t\tisa = PBXGroup;\n\t\t\tchildren = ('.format(resources_pbxgroup_id)
    project_data = project_data.replace(
        pbxgroup_section,
        '{0}\n\t\t\t\t{1} /* {2} */,'.format(pbxgroup_section, id2, folder)
    )

    pbxbuildphase_section = 'isa = PBXResourcesBuildPhase;\n\t\t\tbuildActionMask = 2147483647;\n\t\t\tfiles = ('
    project_data = project_data.replace(
        pbxbuildphase_section,
        '{0}\n\t\t\t\t{1} /* {2} in Resources */,'.format(pbxbuildphase_section, id1, folder)
    )
        
    with open(project, "w") as f:
        f.write(project_data) 

@click.command()
@click.option('--project', '-p', default='project', help="Source path of project files")
@click.option('--root-dir', '-r', default='../..', help="Source path of Suepernova lib")
@click.option('--build/--no-build', default=False, help="Build or no build generated Xcode project")
def build_ios(project, root_dir, build):

    projectSource = project
    supernovaRoot = root_dir

    build_dir = resolve_path("build_ios")
    if not os.path.isdir(build_dir):
        rmtree_silent(build_dir)
    makedirs_silent(build_dir)
    os.chdir(build_dir)

    system_output = "Xcode"
    system_name = "iOS"
    OSX_SDK="iphoneos"
    build_config = "Debug"
    build_sdk = "iphonesimulator"

    subprocess.run([
    "cmake",
    "-DPROJECT_SOURCE="+projectSource,
    "-DCMAKE_SYSTEM_NAME="+system_name,
    "-DCMAKE_OSX_SYSROOT="+OSX_SDK,
    "-G",system_output,
    os.path.join("..",supernovaRoot,"platform","ios")
    ]).check_returncode()

    xcode_project = os.path.join("Supernova.xcodeproj", "project.pbxproj")
    assets_path = os.path.join(supernovaRoot, projectSource, "assets")
    lua_path = os.path.join(supernovaRoot, projectSource, "lua")

    add_folder_reference(xcode_project, assets_path, "supernova-ios")
    add_folder_reference(xcode_project, lua_path, "supernova-ios")

    if build:
        subprocess.run([
        "cmake",
        "--build", build_dir,
        "--config", build_config,
        "--",
        "-sdk", build_sdk
        ]).check_returncode()


if __name__ == '__main__':
    build_ios()