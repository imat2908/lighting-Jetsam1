#version 430

in vec3 vertPos;
in vec3 N;
in vec3 lightPos;

// complete to a full phong shading
layout( location = 0 ) out vec4 FragColour;

uniform vec3 eyePos;
struct Reflectiveness
{
	//reflectiveness of the object
	vec3 Kd;
	vec3 Ks;
	vec3 Ka;
	//shininess of the object
	float shine;
};
uniform Reflectiveness Material;

struct LightIntensity
{
	//intensity of lighting types
	vec3 Ld;
	vec3 Ls;
	vec3 La;
};
uniform LightIntensity light;


float Attenuation(vec3 L)
{
	float aC=0.7f;//constant
	float aL=0.4f;//linear
	float aQ=0.4f;//quadratic
	float dist=length(L);

	return (1/aC+(aL*dist)+(aQ*dist*dist));
}

vec3 ambient()
{
	return Material.Ka*light.La; //ambient light
}
vec3 specularLight(vec3 camPos,vec3 reflectedVector)
{
	vec3 radiance=Material.Ks*light.Ls;//strength of the specular light
	float cosAngle=max(dot(reflectedVector,camPos),0.0);//angle of the light
	
	return radiance*pow(cosAngle,Material.shine);
}
vec4 diffuseLight(vec3 L) //diffuse Light
{
   //calculate Diffuse Light Intensity making sure it is not negative and is clamped 0 to 1  
   vec4 Id = vec4(light.Ld,1.0) * max(dot(N,L), 0.0);// because the vec4 is a homogenuious vector and the vec3 is the light vector
   Id = clamp(Id, 0.0, 1.0); // The clamp function fixes an object to a point, it is needed in this instance to cast the diffuse light rays from objects 

  return vec4(Material.Kd,1.0) * Id;
}

vec4 toonShading(vec3 L)
{
	vec4 Id;//sets id variable

	float cosine=dot(N,L); //dot product of normal and light
	//quantisation levels
	if(cosine > 0.8f && cosine < 1.f) Id=vec4(light.Ld,1.0f)*0.8f;
	if(cosine > 0.6f && cosine < 0.8f) Id=vec4(light.Ld,1.0f)*0.6f;
	if(cosine > 0.4f && cosine < 0.6f) Id=vec4(light.Ld,1.0f)*0.4f;
	if(cosine > 0.2f && cosine < 0.4f) Id=vec4(light.Ld,1.0f)*0.2f;
	if(cosine > 0.0f && cosine < 0.2f) Id=vec4(light.Ld,1.0f)*0.0f;

	return vec4(Material.Kd,1.0f)*Id; //returns gradients
}




void main() {

   //Calculate the light vector
   vec3 L = normalize(lightPos - vertPos);  //Standardizing the light vector to the vertical position of the camera
    
	//Attenuation
   float attenuation=Attenuation(L);
   //ambient light
   vec3 AmbLight=ambient();
   //Diffuse Light
  // vec4 DiffLight=diffuseLight(L);
   vec4 DiffLight=toonShading(L);
   vec3 CamPos=normalize( eyePos-vertPos);
   vec3 reflectedRay=reflect(-L,N);
   vec3 SpecLight=specularLight(CamPos,reflectedRay);

   //Multiply the Reflectivity by the Diffuse intensity
   FragColour =  DiffLight + vec4(AmbLight*attenuation,1.0)+vec4(SpecLight*attenuation,1.0);

}
