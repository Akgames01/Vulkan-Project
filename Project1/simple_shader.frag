//fragment shader file. 

#version 450 
//layout qualifier. 
//for now location 0, to be used as an output with the out qualifier
layout(location = 0) out vec4 outColor;
void main(){
	//unlike the vertex shader, there isn't a built-in output variable.
	//each component being red, green, blue and alpha channels.
	//runs on a per pixel basis.
	outColor = vec4(1.0,0.0,0.0,1.0); 

}