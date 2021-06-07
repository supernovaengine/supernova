#!/usr/bin/env python

# /*
# (c) 2021 Eduardo Doria.
# */

import click
import subprocess

@click.command()
@click.option('--lang', '-l', required=True, type=click.Choice(['glsl330', 'glsl100', 'glsl300es', 'hlsl4', 'hlsl5'], case_sensitive=False), help="Target shader language")
def generate(lang):   

    subprocess.check_call(["bin/supershader", "--lang", lang, "--vert", "shaders/sky.vert", "--frag", "shaders/sky.frag", "--output", "../../project/assets/shaders/sky_glsl330" ])
    subprocess.check_call(["bin/supershader", "--lang", lang, "--vert", "shaders/mesh.vert", "--frag", "shaders/mesh.frag", "--output", "../../project/assets/shaders/mesh_UltUv1Vc4_glsl330", "--defines", "HAS_UV_SET1;MATERIAL_UNLIT;HAS_VERTEX_COLOR_VEC4"])

if __name__ == '__main__':
    generate()