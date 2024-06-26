#!/usr/bin/env python

# /*
# (c) 2021 Eduardo Doria.
# */

import os
import sys
import subprocess
import click
import platform
import glob
import base64

def get_define(property):
    if property == 'Ult':
        return 'MATERIAL_UNLIT'
    elif property == 'Tex':
        return 'HAS_TEXTURE'
    elif property == 'Uv1':
        return 'HAS_UV_SET1'
    elif property == 'Uv2':
        return 'HAS_UV_SET2'
    elif property == 'Puc':
        return 'USE_PUNCTUAL'
    elif property == 'Shw':
        return 'USE_SHADOWS'
    elif property == 'Pcf':
        return 'USE_SHADOWS_PCF'
    elif property == 'Nor':
        return 'HAS_NORMALS'
    elif property == 'Nmp':
        return 'HAS_NORMAL_MAP'
    elif property == 'Tan':
        return 'HAS_TANGENTS'
    elif property == 'Ftx':
        return 'HAS_FONTATLAS_TEXTURE'
    elif property == 'Vc3':
        return 'HAS_VERTEX_COLOR_VEC3'
    elif property == 'Vc4':
        return 'HAS_VERTEX_COLOR_VEC4'
    elif property == 'Txr':
        return 'HAS_TEXTURERECT'
    elif property == 'Fog':
        return 'HAS_FOG'
    elif property == 'Ski':
        return 'HAS_SKINNING'
    elif property == 'Mta':
        return 'HAS_MORPHTARGET'
    elif property == 'Mnr':
        return 'HAS_MORPHNORMAL'
    elif property == 'Mtg':
        return 'HAS_MORPHTANGENT'
    elif property == 'Ter':
        return 'HAS_TERRAIN'
    else:
        sys.exit('Not found value for property: '+property)

def get_vert(shaderType):
    return 'shaderlib/'+shaderType+'.vert'

def get_frag(shaderType):
    return 'shaderlib/'+shaderType+'.frag'

def get_binary_shader_dir():
    return os.path.join("binshaders")

def get_header_output(engine_root):
    return os.path.join(engine_root, "engine", "shaders")

def get_sbs_output(shader, lang):
    shadersdir = get_binary_shader_dir()
    if not os.path.exists(shadersdir):
        print('Creating shaders dir:', shadersdir)
        os.makedirs(shadersdir)
    outpath = os.path.join(shadersdir, shader+"_"+lang)
    return outpath

def get_default_shaders():
    s =  "mesh_Uv1PucNorNmpTanVc4;"
    s += "mesh_Uv1PucNorNmpTanVc4Fog;"
    s += "mesh_Uv1PucNorNmpTanVc4Ski;"
    s += "mesh_Uv1PucNorNmpTanVc4FogSki;"
    s += "mesh_Uv1PucNorNmpTan;"
    s += "mesh_Uv1PucNorNmpTanFog;"
    s += "mesh_Uv1PucShwPcfNorNmpTanVc4;"
    s += "mesh_Uv1PucShwPcfNorNmpTanVc4Fog;"
    s += "mesh_Uv1PucShwPcfNorNmpTan;"
    s += "mesh_Uv1PucShwPcfNorNmpTanFog;"
    s += "mesh_Uv1PucNorVc4;"
    s += "mesh_Uv1PucNorVc4Fog;"
    s += "mesh_Uv1PucNor;"
    s += "mesh_Uv1PucNorFog;"
    s += "mesh_Uv1PucNorVc4Txr;"
    s += "mesh_Uv1PucShwPcfNorVc4;"
    s += "mesh_Uv1PucShwPcfNorVc4Fog;"
    s += "mesh_Uv1PucShwPcfNor;"
    s += "mesh_Uv1PucShwPcfNorSki;"
    s += "mesh_Uv1PucShwPcfNorFog;"
    s += "mesh_Uv1PucShwPcfNorFogSki;"
    s += "mesh_UltUv1Vc4;"
    s += "mesh_UltUv1Vc4Fog;"
    s += "mesh_Ult;"
    s += "mesh_UltUv1;"
    s += "mesh_Uv1PucNorTer;"
    s += "mesh_Uv1PucShwPcfNorTer;"
    s += "mesh_Uv1PucShwPcfNorFogTer;"
    s += "mesh_UltUv1Ter;"
    s += "mesh_UltSki;"
    s += "mesh_UltUv1Fog;"
    s += "mesh_UltUv1Ski;"
    s += "mesh_UltVc4Ski;"
    s += "mesh_UltUv1Vc4Txr;"
    s += "mesh_UltUv1Vc4TxrFog;"
    s += "mesh_UltMtaMnrMtg;"
    s += "mesh_PucNorVc4Ski;"
    s += "mesh_PucShwPcfNor;"
    s += "mesh_PucShwPcfNorSki;"
    s += "mesh_PucShwPcfNorVc4;"
    s += "mesh_PucShwPcfNorVc4Ski;"
    s += "mesh_PucShwPcfNorTanMtaMnr;"
    s += "mesh_PucShwPcfNorTanMtaMnrMtg;"
    s += "mesh_PucShwPcfNorTanFogMtaMnr;"
    s += "mesh_PucShwPcfNorTanFogMtaMnrMtg;"
    s += "depth;"
    s += "depth_Ski;"
    s += "depth_Mta;"
    s += "depth_MtaMnr;"
    s += "depth_MtaMnrMtg;"
    s += "depth_Ter;"
    s += "sky;"
    s += "ui_Vc4;"
    s += "ui_TexVc4;"
    s += "ui_Ftx;"
    s += "points_Vc4;"
    s += "points_TexVc4;"
    s += "points_TexVc4Txr;"
    s += "lines_Vc4"
    return s

def get_default_langs():
    l =  "glsl430;"
    l += "glsl300es;"
    l += "msl21ios;"
    l += "msl21macos;"
    l += "hlsl5"
    return l

def get_bin_exec():
    plt = platform.system()

    execname = "supershader"
    binpath = "bin"
    outpath = os.path.join(binpath, execname)

    if plt == "Windows":
        outpath = os.path.join(binpath, "windows", execname + ".exe")
    elif plt == "Linux":
        outpath = os.path.join(binpath, "linux", execname)
    elif plt == "Darwin":
        outpath = os.path.join(binpath, "macos", execname)
    else:
        print("Unidentified system")
        sys.exit(1)
    
    return outpath

def create_c_header(engine_root):
    filesshaderlist =  {}

    for file in list(glob.glob(os.path.join("binshaders", '*.sbs'))):
        shadername = os.path.splitext(os.path.basename(file))[0]

        shadersplit = shadername.split('_')
        lang = shadersplit[len(shadersplit)-1]

        filesshaderlist.setdefault(lang,[]).append(file)


    for lang in filesshaderlist:
        shaderslist = []

        headerpath = os.path.join(get_header_output(engine_root), lang+".h")

        print("Creating:", headerpath);

        headerfile = open(headerpath, "w")
        headerfile.write("#ifndef SHADER_"+lang+"_h")
        headerfile.write("\n")
        headerfile.write("#define SHADER_"+lang+"_h")
        headerfile.write("\n\n")
        headerfile.write("#include <string>")
        headerfile.write("\n\n")

        filesshaderlist[lang].sort()

        for file in filesshaderlist[lang]:
            sbsfile = open(file, "rb")
            encodedShader = base64.b64encode(sbsfile.read()).decode("utf-8")

            shadername = os.path.splitext(os.path.basename(file))[0]
            shaderslist.append(shadername)

            stringSize = 100
            encodedShaderChunks = [encodedShader[i:i+stringSize] for i in range(0, len(encodedShader), stringSize)]

            headerfile.write("static const std::string ")
            headerfile.write(shadername)
            headerfile.write(" = ")
            for encodedShaderChunk in encodedShaderChunks:
                headerfile.write("\n")
                headerfile.write("    \"" + encodedShaderChunk + "\"")
            headerfile.write(";")
            headerfile.write("\n")


        headerfile.write("\n")
        first = True
        headerfile.write("std::string getBase64Shader(std::string name) {")
        for shadername in shaderslist:
            headerfile.write("\n")
            if first:
                headerfile.write("    if (name == \"" + shadername + "\") {")
            else:
                headerfile.write("    } else if (name == \"" + shadername + "\") {")
            headerfile.write("\n")
            headerfile.write("        return "+shadername+";")
            first = False

        if not first:
            headerfile.write("\n")
            headerfile.write("    }")

        headerfile.write("\n")
        headerfile.write("    return \"\";")
        headerfile.write("\n")
        headerfile.write("}")
        headerfile.write("\n")
        headerfile.write("\n")

        headerfile.write("#endif //SHADER_"+lang+"_h")
        headerfile.write("\n")

        headerfile.close()

@click.command()
@click.option('--shaders', '-s', default=get_default_shaders(), help="Target shader type, seperated by ';'")
@click.option('--langs', '-l', default=get_default_langs(), required=True, help="Target shader language, seperated by ';'")
@click.option('--engine-root', '-r', default='..', type=click.Path(), help="Source root path of Supernova")
@click.option('--verbose/--no-verbose', '-v', default=False, help="Output more information")
@click.option('--max-lights', '-ml', default=6, type=int, help="Value of MAX_LIGHTS macro")
@click.option('--max-shadowsmap', default=6, type=int, help="Value of MAX_SHADOWSMAP macro")
@click.option('--max-shadowscubemap', default=1, type=int, help="Value of MAX_SHADOWSCUBEMAP macro")
@click.option('--max-shadowcascades', default=4, type=int, help="Value of MAX_SHADOWCASCADES macro")
def generate(shaders, langs, engine_root, verbose, max_lights, max_shadowsmap, max_shadowscubemap, max_shadowcascades):

    shadersList = [x.strip() for x in shaders.split(';')]
    langsList = [x.strip() for x in langs.split(';')]

    for lang in langsList:

        for shader in shadersList:

            splitShader = shader.split('_')
            shaderType = splitShader[0]
            properties = ''
            if len(splitShader) >= 2:
                properties = splitShader[1]

            defines = ''
            defines += 'MAX_LIGHTS='+str(max_lights)
            defines += ';MAX_SHADOWSMAP='+str(max_shadowsmap)
            defines += ';MAX_SHADOWSCUBEMAP='+str(max_shadowscubemap)
            defines += ';MAX_SHADOWCASCADES='+str(max_shadowcascades)
            defines += ';MAX_BONES='+str(70)

            while properties != '':
                if len(defines) > 0:
                    defines += ';'
                defines += get_define(properties[:3])
                properties = properties[3:]

            vert = get_vert(shaderType)
            frag = get_frag(shaderType)
            output = get_sbs_output(shader, lang)

            print('Generating', shader, 'for', lang)

            if verbose:
                print(get_bin_exec(), "--lang", lang, "--vert", vert, "--frag", frag, "--output", output, "--output-type", "binary", "--defines", "'"+defines+"'")
            command = subprocess.run([get_bin_exec(), "--lang", lang, "--vert", vert, "--frag", frag, "--output", output, "--output-type", "binary", "--defines", defines], capture_output=True)

            sys.stdout.buffer.write(command.stdout)
            sys.stderr.buffer.write(command.stderr)
            if command.returncode != 0:
                sys.exit(command.returncode)

    create_c_header(engine_root)


if __name__ == '__main__':
    generate()
