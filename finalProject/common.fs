#version 430 core

in vec4 fColor;
in vec2 tex;

out vec4 color;

uniform sampler2D texChange;

void main(void)
{
   
color = fColor;
//vec4 texColor = texture( texChange, tex );

}
