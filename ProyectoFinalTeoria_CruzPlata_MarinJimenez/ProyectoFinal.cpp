#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include <camera.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor *monitors;

void getResolution(void);
float CamaraX = 0.0f,
CamaraY = 20.0f,
CamaraZ = 450.0f;

// camera
Camera camera(glm::vec3(CamaraX, CamaraY, CamaraZ));
float MovementSpeed = 10.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

int i = 0;
float x = 0.0f,
y = 0.0f,
z = 0.0f;
glm::vec3 CamaraPersona(x, y, z);

float tiempoLuz = 0.0f;
bool bandera = true;
int cambiobox = 0;
bool terceraPersona = true,
guardado = false,
finish = false;

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void cambioCamara(void) {
	if (terceraPersona == false and finish == false) {
		if (guardado == false) {
			CamaraX = camera.Position.x;
			CamaraY = camera.Position.y;
			CamaraZ = camera.Position.z;
			CamaraPersona = camera.Position;
			guardado = true;
		}
		if (CamaraX < 0.0f) 
			CamaraX += 2.0f;
		if (CamaraX > 5.0f)
			CamaraX -= 2.0f;
		if (CamaraZ < 0.0f)
			CamaraZ += 2.0f;
		if (CamaraZ > 5.0f)
			CamaraZ -= 2.0f;
		if (CamaraY < 350.0f)
			CamaraY += 1.0f;
		if (CamaraX<=5.0f and CamaraX>=0.0f and CamaraZ<=5.0f and CamaraZ>=0.0f and CamaraY >= 350.0f)
			finish = true;
		camera.Position = glm::vec3(CamaraX, CamaraY, CamaraZ);
	}
	if (terceraPersona == true and guardado == true) {
		if (CamaraX < CamaraPersona.x)
			CamaraX += 1.0f;
		if (CamaraX > CamaraPersona.x)
			CamaraX -= 1.0f;
		if (CamaraZ < CamaraPersona.z)
			CamaraZ += 1.0f;
		if (CamaraZ > CamaraPersona.z)
			CamaraZ -= 1.0f;
		if (CamaraY > CamaraPersona.y)
			CamaraY -= 1.0f;
		if (CamaraX <= CamaraPersona.x and CamaraX >= CamaraPersona.x and CamaraZ <= CamaraPersona.z and CamaraZ >= CamaraPersona.z and CamaraY <= CamaraPersona.y){
			guardado = false;
			finish = false;
		}
		camera.Position = glm::vec3(CamaraX, CamaraY, CamaraZ);
	}
}

void animate(void)
{

	if (tiempoLuz >= 0.0f and tiempoLuz < 1.5f and bandera == true) {
		tiempoLuz += 0.0005f;
		if (tiempoLuz >= 1.2f)
			cambiobox = 1;  //Dia
	}
	if (tiempoLuz >= 1.5f and bandera == true) {
		bandera = false;
		tiempoLuz -= 0.0005f;
	}
	if (tiempoLuz >= 0.0f and tiempoLuz < 1.5f and bandera == false) {
		tiempoLuz -= 0.0005f;
		if (tiempoLuz <= 0.7f)
			cambiobox = 2;  //Noche
	}
	if (tiempoLuz <= 0.0f and bandera == false) {
		bandera = true;
		tiempoLuz += 0.0005f;
	}
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");


	//Aqui se colocan las imagenes que iran en el skybox

	vector<std::string> faces1  //Amanecer
	{
		"resources/skybox/AnochecerRight.jpg",
		"resources/skybox/AnochecerLeft.jpg",
		"resources/skybox/AnochecerTop.jpg",
		"resources/skybox/AnochecerBottom.jpg",
		"resources/skybox/AnochecerFront.jpg",
		"resources/skybox/AnochecerBack.jpg"
	};

	vector<std::string> faces2  //Dia
	{
		"resources/skybox/DiaRight.jpg",
		"resources/skybox/DiaLeft.jpg",
		"resources/skybox/DiaTop.jpg",
		"resources/skybox/DiaBottom.jpg",
		"resources/skybox/DiaFront.jpg",
		"resources/skybox/DiaBack.jpg"
	};

	vector<std::string> faces4  //Noche
	{
		"resources/skybox/NocheRight.jpg",
		"resources/skybox/NocheLeft.jpg",
		"resources/skybox/NocheTop.jpg",
		"resources/skybox/NocheBottom.jpg",
		"resources/skybox/NocheFront.jpg",
		"resources/skybox/NocheBack.jpg"
	};

	Skybox skybox1 = Skybox(faces1);
	Skybox skybox2 = Skybox(faces2);
	Skybox skybox3 = Skybox(faces4);

	
	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox1", 0);
	skyboxShader.setInt("skybox2", 0);
	skyboxShader.setInt("skybox3", 0);

	// load models
	// -----------
	Model piso("resources/objects/piso/piso.obj");
	Model escenario_M("resources/objects/piso/textura_escenario.obj");
	Model casaModerna("resources/objects/CasaModerna/Farm_house.obj");
	Model calle_M("resources/objects/piso/calle.obj");
	Model calleBanqueta_M("resources/objects/piso/calles_banquetas_unosolo.obj");
	Model paredes_M("resources/objects/paredes/paredes.obj");
	Model paredes2_M("resources/objects/paredes/paredes2.obj");
	Model allbotebasura_M("resources/objects/mobiliario/all_botesbasura.obj");
	Model allbotebasura2_M("resources/objects/mobiliario/all_botesbasura2.obj");
	Model allbancas_M("resources/objects/bancas/all_bancas.obj");
	Model caseta_M("resources/objects/caseta/caseta.obj");
	Model casa_M("resources/objects/casa/all_casa.obj");
	Model casa("resources/objects/mansion/new_house.obj");
	Model tienda_M("resources/objects/tienda/tienda.obj");
	Model restaurante_M("resources/objects/restaurante/comida.obj");
	Model lampara_M("resources/objects/poste/all_lamparas.obj");
	Model parada_M("resources/objects/paradero/parada.obj");
	Model alberca_M("resources/objects/alberca/alberca.obj");
	Model sillasAlberca_M("resources/objects/sillas/sillas_alberca.obj");
	//Model arbol1_M("resources/objects/plantas/Tree_OBJ.obj");

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.use();
		animate();
		cambioCamara();

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		//Fuente de luz direccional (Trata de ser una fuente de luz parecida al sol)
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.0f+tiempoLuz, 0.0f+tiempoLuz, 0.0f+tiempoLuz));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

		//Fuentes de luz posicionales (Replicar los focos, lamparas, etc)
		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);    // La linear y la constant dicen que tanto se mueve la luz
		staticShader.setFloat("pointLight[0].quadratic", 0.032f); // intensidad de la luz

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);
		
		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(0.6f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);


		//Animaci�n del personaje
		/*model = glm::translate(glm::mat4(1.0f), glm::vec3(-40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.2f));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		animacionPersonaje.Draw(animShader);*/

		// -------------------------------------------------------------------------------------------------------------------------
		// Segundo Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------

		/*model = glm::translate(glm::mat4(1.0f), glm::vec3(40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", model);
		ninja.Draw(animShader);*/

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		/*model = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		casaModerna.Draw(staticShader);*/

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		//piso.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		escenario_M.Draw(staticShader);

		/*model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 450.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		calle_M.Draw(staticShader);*/

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		calleBanqueta_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		paredes_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		paredes2_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		allbotebasura_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		allbotebasura2_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		allbancas_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		caseta_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		casa_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 150.0f));
		model = glm::scale(model, glm::vec3(0.3f,0.4f,0.3f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		casa.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		tienda_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		restaurante_M.Draw(staticShader);
		
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		lampara_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		parada_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		alberca_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		sillasAlberca_M.Draw(staticShader);
		// -------------------------------------------------------------------------------------------------------------------------
		// Caja Transparente --- Siguiente Pr�ctica
		// -------------------------------------------------------------------------------------------------------------------------
		/*glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -70.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		staticShader.setMat4("model", model);
		nube.Draw(staticShader);
		glEnable(GL_BLEND);*/
		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		
		if (cambiobox == 0) {
			skybox1.Draw(skyboxShader, view, projection, camera);
		}
		if (cambiobox == 1) {
			skybox1.Terminate();
			skybox2.Draw(skyboxShader, view, projection, camera);
		}
		if (cambiobox == 2) {
			skybox2.Terminate();
			skybox3.Draw(skyboxShader, view, projection, camera);
		}
		

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox1.Terminate();
	skybox2.Terminate();
	skybox3.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (terceraPersona == true)
			camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){ 
		if (terceraPersona == true)
			camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);

	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;

	//Camara aerea y tercera persona
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		terceraPersona = false;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		terceraPersona = true;

	
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (terceraPersona == true) {
		if (guardado == true) {
			camera.Pitch = 0.0f;
			camera.Yaw = -90.0f;
		}
		else
			camera.ProcessMouseMovement(xoffset, 0);
	}
	if (terceraPersona == false)
		camera.ProcessMouseMovement(xoffset, -90);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}