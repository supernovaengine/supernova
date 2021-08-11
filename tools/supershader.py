#!/usr/bin/env python

# /*
# (c) 2021 Eduardo Doria.
# */

import os
import sys
import subprocess
import click
import platform

def get_define(property):
    if property == 'Ult':
        return 'MATERIAL_UNLIT'
    elif property == 'Uv1':
        return 'HAS_UV_SET1'
    elif property == 'Uv2':
        return 'HAS_UV_SET2'
    elif property == 'Puc':
        return 'USE_PUNCTUAL'
    elif property == 'Shw':
        return 'USE_SHADOWS'
    elif property == 'Nor':
        return 'HAS_NORMALS'
    elif property == 'Nmp':
        return 'HAS_NORMAL_MAP'
    elif property == 'Tan':
        return 'HAS_TANGENTS'
    elif property == 'Vc3':
        return 'HAS_VERTEX_COLOR_VEC3'
    elif property == 'Vc4':
        return 'HAS_VERTEX_COLOR_VEC4'
    else:
        sys.exit('Not found value for property: '+property)

def get_vert(shaderType):
    return 'shaderlib/'+shaderType+'.vert'

def get_frag(shaderType):
    return 'shaderlib/'+shaderType+'.frag'

def get_output(shader, project, lang):
    outpath = os.path.join(project, "assets/shaders", shader+"_"+lang)
    return outpath

def get_default_shaders():
    s =  "mesh_Uv1PucNorNmpTanVc4;"
    s += "mesh_Uv1PucShwNorNmpTanVc4;"
    s += "mesh_Uv1PucNorVc4;"
    s += "mesh_Uv1PucShwNorVc4;"
    s += "mesh_UltUv1Vc4;"
    s += "depth;"
    s += "sky"
    return s

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

@click.command()
@click.option('--shaders', '-s', default=get_default_shaders(), help="Target shader language, seperated by ';'")
@click.option('--lang', '-l', required=True, help="Target shader language")
@click.option('--project', '-p', default='../../project', type=click.Path(), help="Source root path of project files")
@click.option('--max-lights', '-ml', default=6, type=int, help="Value of MAX_LIGHTS macro")
@click.option('--max-shadowsmap', default=6, type=int, help="Value of MAX_SHADOWSMAP macro")
@click.option('--max-shadowscubemap', default=1, type=int, help="Value of MAX_SHADOWSCUBEMAP macro")
@click.option('--max-shadowcascades', default=4, type=int, help="Value of MAX_SHADOWCASCADES macro")
def generate(shaders, lang, project, max_lights, max_shadowsmap, max_shadowscubemap, max_shadowcascades):

    shadersList = [x.strip() for x in shaders.split(';')]

    for shader in shadersList:

        print('Generating', shader, 'for', lang)

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

        while properties != '':
            if len(defines) > 0:
                defines += ';'
            defines += get_define(properties[:3])
            properties = properties[3:]

        vert = get_vert(shaderType)
        frag = get_frag(shaderType)
        output = get_output(shader, project, lang)

        #print(get_bin_exec(), "--lang", lang, "--vert", vert, "--frag", frag, "--output", output, "--defines", defines)
        command = subprocess.run([get_bin_exec(), "--lang", lang, "--vert", vert, "--frag", frag, "--output", output, "--defines", defines], capture_output=True)

        sys.stdout.buffer.write(command.stdout)
        sys.stderr.buffer.write(command.stderr)
        if command.returncode != 0:
            sys.exit(command.returncode)

if __name__ == '__main__':
    generate()