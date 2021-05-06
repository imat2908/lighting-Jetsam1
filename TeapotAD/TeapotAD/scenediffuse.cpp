
#include "scenediffuse.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::cerr;
using std::endl;

#include "defines.h"

using glm::vec3;


#include <gtc/matrix_transform.hpp>
#include <gtx/transform2.hpp>


namespace imat2908
{
/////////////////////////////////////////////////////////////////////////////////////////////
// Default constructor
/////////////////////////////////////////////////////////////////////////////////////////////
SceneDiffuse::SceneDiffuse()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Initialise the scene
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::initScene(QuatCamera camera)
{
    //|Compile and link the shader  
	compileAndLinkShader();

    gl::Enable(gl::DEPTH_TEST);

 	//Set up the lighting
	setLightParams(camera);


	//Create the plane to represent the ground
	plane = new VBOPlane(100.0,100.0,100,100);



	//A matrix to move the teapot lid upwards
	glm::mat4 lid = glm::mat4(1.0);
	//lid *= glm::translate(vec3(0.0,1.0,0.0));

	//Create the teapot with translated lid
	teapot = new VBOTeapot(16,lid);


}

/////////////////////////////////////////////////////////////////////////////////////////////
//Update not used at present
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::update( float t )
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Set up the lighting variables in the shader
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::setLightParams(QuatCamera camera)
{

	vec3 worldLight = vec3(10.0f,10.0f,10.0f);
   
	
	prog.setUniform("light.Ld", 0.9f, 0.9f, 0.9f);//Diffuse light
	prog.setUniform("light.Ls", 0.3f, 0.3f, 0.3f);//Specular light
	prog.setUniform("light.La", 0.3f, 0.3f, 0.3f);//Ambient Light
	
	prog.setUniform("LightPosition", worldLight );
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Render the scene
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::render(QuatCamera camera)
{
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

	//First deal with the plane to represent the ground

	//Initialise the model matrix for the plane
	model = mat4(1.0f);
	//Set the matrices for the plane although it is only the model matrix that changes so could be made more efficient
    setMatrices(camera);
	//Set the plane's material properties in the shader and render
	prog.setUniform("Material.Kd", 0.51f, 1.0f, 0.49f); // Diffuse Reflectiveness of the plane
	prog.setUniform("Material.Ka", 0.51f, 1.0f, 0.49f); //ambient reflectiveness of the plane
	prog.setUniform("Material.Ks", 0.1f, 0.1f, 0.1f); // specular reflectiveness if the plane

	plane->render();// renders the plane and the reflections off of it



	//Now set up the teapot 
	 model = mat4(1.0f);
	 setMatrices(camera);
	 //Set the Teapot material properties in the shader and render
	 prog.setUniform("Material.Kd", 0.46f, 0.29f, 0.f); //	Diffuse reflectiveness of the teapot
	 prog.setUniform("Material.Ka", 0.46f, 0.29f, 0.0f); // ambient reflectiveness of the teapot
	 prog.setUniform("Material.Ks", 0.29f, 0.29f, 0.29f); // specular reflectiveness of the teapot
	 prog.setUniform("Material.shine", 125.f); //shininess of the teapot
	 teapot->render(); //renders the teapot and the reflections off of it
	
}


/////////////////////////////////////////////////////////////////////////////////////////////
//Send the MVP matrices to the GPU
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::setMatrices(QuatCamera camera)
{

    mat4 mv = camera.view() * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", camera.projection() * mv);
	mat3 normMat = glm::transpose(glm::inverse(mat3(model)));// What does this line do?
	prog.setUniform("M", model);
	prog.setUniform("V", camera.view() );
	prog.setUniform("P", camera.projection() );

	
}

/////////////////////////////////////////////////////////////////////////////////////////////
// resize the viewport
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::resize(QuatCamera camera,int w, int h)
{
    gl::Viewport(0,0,w,h);
    width = w;
    height = h;
	camera.setAspectRatio((float)w/h);

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compile and link the shader
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneDiffuse::compileAndLinkShader()
{
   
	try {
    	prog.compileShader("Shaders/phong.vert");
    	prog.compileShader("Shaders/phong.frag");
    	prog.link();
    	prog.validate();
    	prog.use();
    } catch(GLSLProgramException & e) {
 		cerr << e.what() << endl;
 		exit( EXIT_FAILURE );
    }
}

}
