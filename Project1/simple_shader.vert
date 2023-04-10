#version 450

//one for each corner of the triangle
//we don't normally hardcode the values of the vertex shader.we pass in values using a vertex buffer. 
vec2 positions[3] = {
	vec2(0.0,0.5),
	vec2(0.5,0.5),
	vec2(-0.5,0.5)
}; 
//executed once per vertex 
void main(){
	//gets input from the input assembler stage
	//value assigned to obtain an output
	//4D vector maps to the output image 
	//z value is 0
	//everything divided by 1.0. useful later. 
	gl_Position = vec4(positions[gl_VertexIndex],0.0, 1.0);
}