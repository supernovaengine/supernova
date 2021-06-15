#!/usr/bin/env python

# /*
# (c) 2021 Eduardo Doria.
# */

import os
import sys
import subprocess
import click

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

@click.command()
@click.option('--shaders', '-s', required=True, help="Target shader language, seperated by ';'")
@click.option('--lang', '-l', required=True, type=click.Choice(['glsl330', 'glsl100', 'glsl300es', 'hlsl4', 'hlsl5'], case_sensitive=False), help="Target shader language")
@click.option('--project', '-p', default='../../project', type=click.Path(), help="Source root path of project files")
@click.option('--max-lights', '-ml', default=6, type=int, help="Value of MAX_LIGHTS macro")
def generate(shaders, lang, project, max_lights):

    shadersList = [x.strip() for x in shaders.split(';')]

    for shader in shadersList:

        print('Generating', shader, 'for', lang)

        splitShader = shader.split('_')
        shaderType = splitShader[0]
        properties = ''
        if len(splitShader) >= 2:
            properties = splitShader[1]

        defines = 'MAX_LIGHTS='+str(max_lights)

        while properties != '':
            if len(defines) > 0:
                defines += ';'
            defines += get_define(properties[:3])
            properties = properties[3:]

        vert = get_vert(shaderType)
        frag = get_frag(shaderType)
        output = get_output(shader, project, lang)

        #print("bin/supershader", "--lang", lang, "--vert", vert, "--frag", frag, "--output", output, "--defines", defines)
        command = subprocess.run(["bin/supershader", "--lang", lang, "--vert", vert, "--frag", frag, "--output", output, "--defines", defines], capture_output=True)

        sys.stdout.buffer.write(command.stdout)
        sys.stderr.buffer.write(command.stderr)
        if command.returncode != 0:
            sys.exit(command.returncode)

if __name__ == '__main__':
    generate()