#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h> //main
#include <stdlib.h>
#include <glm/glm.hpp>					//camera y model
#include <glm/gtc/matrix_transform.hpp> //camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include <camera.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h> //Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
//#include <irrKlang.h>

using namespace std;

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);


// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

void getResolution(void);
float CamaraX = 0.0f;  //Funciona para cuando cambiamos de camara de tercera persona a aerea.
float CamaraY = 20.0f; //Las coordenadas de CamaraX,Y,Z sirven para cambiar de posición a la camara.
float CamaraZ = 450.0f;

float PersonaX1 = 0.0f; //Se utiliza para que nuestro personaje principal rote con respecto a la camara.
float PersonaX2 = 0.0f; //PersonaX1,2 va a mover a nuestro personaje dependiendo de como movamos el mouse

float rotacion = 180.0f; //Es la variable para rotar al personaje
float PosX = 0.0f;		 // Sirve para mover al personaje cuando presionamos una tecla
float PosY = 450.0f;


// camera
Camera camera(glm::vec3(CamaraX, CamaraY, CamaraZ));
float MovementSpeed = 5.0f;		//Sirve para saber que tan rapido se mueve la camara
float lastX = SCR_WIDTH / 2.0f; //Funciona en conjunto al mouse para mover la camara
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double deltaTime = 0.0f,
	   lastFrame = 0.0f;

//Lighting
float Noche = 1.0f;
float Noche2 = 0.0f;
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);
glm::vec3 posicionCarro(0.0f, 0.0f, 0.0f);

glm::vec3 CamaraPersona(0.0f, 0.0f, 0.0f); //En esta variable vamos a guardar la ultima posición del personaje
										   //Guardarlo va a servir para que cuando volvamos de la camara aerea sepamos donde se quedó el personaje

float tiempoLuz = 0.0f;		//Contador para saber el tiempo que durara cada skyBox y hacer más tenue la luz con el paso del tiempo
bool bandera = true;		//La bandera servirá para saber cuando llegamos a una hora del dia e ir de regreso
int cambiobox = 0;			// Lo utilizamos cuando queremos cambiar de skyBox (depende de tiempoLuz)
bool terceraPersona = true; //Nos va a sevir para cuando presionemos un tecla y cambiemos de tipo de camara
bool guardado = false;		//Nos indica cuando ya hemos guardado la posición del personaje al cambiar de camara
bool finish = false;		//Sirve porque al cambiar de camara iremos lentamente a la posición indicada,
							//la variable nos indicará cuando lleguemos a la meta

//Rotacion puertas
float rotaciondoor1 = 0.0f;
float rotaciondoor2 = 180.0f;
float cont;
bool banderadoor = false;

//Rotacion columpio
float rotacionswing = 0.0f;
float cont2 = 45.0f;
float cont3 = -42.0f;
int banderaswing = 0;


//Animacion carro
bool Inicio = false;

//Variable animacion piña
int estadospin = 1;
float posXpin = -220.0f;
float posYpin = 40.0f;
float posZpin = -37.0f;
glm::vec3 movepin = glm::vec3(0.0f, 0.0f, 0.0f);

float orienta = 0.0f;
float angulo = 90.0f;
float giroLlantas = 0.0f;
float movAuto_x = -400.0f;
float movAuto_y = 1.0f;
float movAuto_z = 450.0f;
bool circuito = false;

//variables para niño
int estadosnino = 0;
float posXnino = -342.0f;
float posZnino = 62.0f;
float rotanino = 0.0f;
glm::vec3 movenino = glm::vec3(0.0f, 0.0f, 0.0f);

//Keyframes (Manipulación y dibujo)
float movNube_x = 0.0f;
float movNube_y = 0.0f;
float movNube_z = 0.0f;

float movNube_xInc = 0.0f;
float movNube_yInc = 0.0f;
float movNube_zInc = 0.0f;


#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame {
	//Variables para GUARDAR Key Frames
	float movNube_x; //Variable para PosicionX
	float movNube_y; //Variable para PosicionY
	float movNube_z; //Variable para PosicionZ
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 9; //introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void) {
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].movNube_x = movNube_x;
	KeyFrame[FrameIndex].movNube_y = movNube_y;
	KeyFrame[FrameIndex].movNube_z = movNube_z;

	FrameIndex++;
}

void resetElements(void) {
	movNube_x = KeyFrame[0].movNube_x;
	movNube_y = KeyFrame[0].movNube_y;
	movNube_z = KeyFrame[0].movNube_z;
}

void interpolation(void) {
	movNube_xInc = (KeyFrame[playIndex + 1].movNube_x - KeyFrame[playIndex].movNube_x) / i_max_steps;
	movNube_yInc = (KeyFrame[playIndex + 1].movNube_y - KeyFrame[playIndex].movNube_y) / i_max_steps;
	movNube_zInc = (KeyFrame[playIndex + 1].movNube_z - KeyFrame[playIndex].movNube_z) / i_max_steps;
}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}


/*
	Cuando presionemos la tecla para cambiar de camara de tercera persona vamos a entrar en esta función
	Esta función funciona como si fuera un while, va a seguir aumentando sus coordenadas hasta llegar a la meta
*/
void cambioCamara(void) {
	//Cuando cambaimos a la camara aerea y aún no estamos en la meta entramos al if
	if (terceraPersona == false and finish == false) {
		//Si aún no hemos guardado las coordenadas de nuestro personaje los vamos a guardar
		if (guardado == false) {
			CamaraX = camera.Position.x;
			CamaraY = camera.Position.y;
			CamaraZ = camera.Position.z;
			CamaraPersona = camera.Position; //Nos servirá para hacer otro while hasta ir detrás de nuestro personaje
			guardado = true;				 //Indicamos que se guardó la ultima posición
		}
		//While (Nos dirijiremos a las coordenadas (0,350,0)
		if (CamaraX < 0.0f)
			CamaraX += 2.0f;
		if (CamaraX > 5.0f)
			CamaraX -= 2.0f;
		if (CamaraZ < 0.0f)
			CamaraZ += 2.0f;
		if (CamaraZ > 5.0f)
			CamaraZ -= 2.0f;
		if (CamaraY < 350.0f)
			CamaraY += 2.0f;
		//-------------------------------------------------------
		//Cuando llegamos a las coordenadas, vamos a cambiar la variable finish
		if (CamaraX <= 5.0f and CamaraX >= 0.0f and CamaraZ <= 5.0f and CamaraZ >= 0.0f and CamaraY >= 350.0f)
			finish = true;
		//Vamos a estar cambiando la posición de la camara a lo largo del while
		camera.Position = glm::vec3(CamaraX, CamaraY, CamaraZ);
	}

	//Si tenemos unas coordenadas guardadas y la tercera persona está activada entramos al if
	if (terceraPersona == true and guardado == true) {
		//Entramos al while y aumentaremos o diminuiremos las coordenadas de la camara hasta llegar a la ultima posición guardada
		if (CamaraX < CamaraPersona.x)
			CamaraX += 2.0f;
		if (CamaraX > CamaraPersona.x)
			CamaraX -= 2.0f;
		if (CamaraZ < CamaraPersona.z)
			CamaraZ += 2.0f;
		if (CamaraZ > CamaraPersona.z)
			CamaraZ -= 2.0f;
		if (CamaraY > CamaraPersona.y)
			CamaraY -= 2.0f;
		//------------------------------------------------------
		// Si llegamos a las coordenadas deseadas le decimos que ya no estamos guardando nada y que terminamos
		if (CamaraX <= CamaraPersona.x and CamaraX >= CamaraPersona.x and CamaraZ <= CamaraPersona.z and CamaraZ >= CamaraPersona.z and CamaraY <= CamaraPersona.y) {
			guardado = false;
			finish = false;
		}
		//Modificamos la posición de la camara a lo largo del while
		camera.Position = glm::vec3(CamaraX, CamaraY, CamaraZ);
	}
}


/*
	Esta función funciona para hacer que la luz cambie a lo largo del tiempo y también indica cuando se cambiará de skybox
*/


void animate(void) {
	//Cuando la luz va de 0 a 1.5 vamos a tener la bandera de true, la luz ira incrementando de 0.0005 en 0.0005
	if (tiempoLuz == 0.0f) {
		//PlaySound("night2.wav", NULL, SND_SYNC);
	}
	if (tiempoLuz >= 0.0f and tiempoLuz < 1.5f and bandera == true) {
		tiempoLuz += 0.001f;
		//Cuando la variable tiempoLuz sea igual a 1.2 el skybox va a cambiar
		if (tiempoLuz >= 1.2f) {
			if (cambiobox != 1) {
				//PlaySound("night2.wav", NULL, SND_SYNC);
				cambiobox = 1; //Dia
			}
			Noche = 0.0f;
		}
	}
	//Cuando la variable llegue a 1.5 la bandera va a cambiar a false
	if (tiempoLuz >= 1.5f and bandera == true) {
		bandera = false;
		tiempoLuz -= 0.001f;
	}
	//Cuando la bandera cambia ira de manera inversa, de 1.5 a 0
	if (tiempoLuz >= 0.0f and tiempoLuz < 1.5f and bandera == false) {
		tiempoLuz -= 0.001f;
		//Cuando la variable sea 0.7 el skybox cambiará a la noche
		if (tiempoLuz <= 0.7f) {
			if (cambiobox != 2) {
				//PlaySound("night2.wav", NULL, SND_SYNC);
				cambiobox = 2; //Noche
			}
			Noche = 1.0f;
		}
	}
	//Cuando llegue a 0 la bandera va a cambiar y se repetirá todo el ciclo
	if (tiempoLuz <= 0.0f and bandera == false) {
		bandera = true;
		tiempoLuz += 0.001f;
	}

	if (play) {
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2) //end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else {
			//Draw animation
			movNube_x += movNube_xInc;
			movNube_y += movNube_yInc;
			movNube_z += movNube_zInc;

			i_curr_steps++;
		}
	}
}

void animacionCarro(void) {
	if (Inicio == true) {
		if (circuito == false) {
			if (angulo == 90.0f) { //Sig Coordenada (-51,1,450)
				if (movAuto_x < -51)
					movAuto_x += 2.0f;
				if (movAuto_x >= -51)
					angulo = 180.0f;
			}
			if (angulo == 180.0f and rotaciondoor1 > 90) {
				if (movAuto_z > 278)
					movAuto_z -= 1.0f;
				if (movAuto_z == 278)
					angulo = 200.0f;
			}
			if (angulo == 200.0f) {
				if (movAuto_x > -107)
					movAuto_x -= 1.0f;
				if (movAuto_z > 82)
					movAuto_z -= 3.0f;
				if (movAuto_x == -107 and movAuto_z == 82)
					angulo = 180.0f;
			}
			if (movAuto_x == -107 and movAuto_z > 38) {
				if (movAuto_z > 38)
					movAuto_z -= 1.0f;
				if (movAuto_z == 38)
					angulo = 105.0f;
			}
			if (angulo == 105) {
				if (movAuto_x < 96)
					movAuto_x += 1.0f;
				if (movAuto_z > -51)
					movAuto_z -= 0.5f;
				if (movAuto_x == 96 and movAuto_z == -51) {
					circuito = true;
					angulo = 90;
				}
			}
		}
		if (circuito == true) {
			if (angulo == 90) {
				if (movAuto_x < 307)
					movAuto_x += 1.0f;
				if (movAuto_x == 307)
					angulo = 180;
			}
			if (angulo == 180) {
				if (movAuto_z > -310)
					movAuto_z -= 1.0f;
				if (movAuto_z == -310)
					angulo = 270;
			}
			if (angulo == 270) {
				if (movAuto_x > 56)
					movAuto_x -= 1.0f;
				if (movAuto_x == 56)
					angulo = 0;
			}
			if (angulo == 0) {
				if (movAuto_z < -51)
					movAuto_z += 1.0f;
				if (movAuto_z == -51)
					angulo = 90;
			}
		}
	}
}

void animatedoor(void) {
	if (rotaciondoor1 < 90.0f and banderadoor == true) {
		rotaciondoor1 += 0.3f;
	}
	if (rotaciondoor2 > 105.0f and banderadoor == true) {
		rotaciondoor2 -= 0.3f;
	}
	if (rotaciondoor1 > -0.1f and banderadoor == false) {
		rotaciondoor1 -= 0.3f;
	}
	if (rotaciondoor2 < 180.0f and banderadoor == false) {
		rotaciondoor2 += 0.3f;
	}
}

void animatecolumpio(void) {
	switch (banderaswing) {
	case 1:
		if (rotacionswing < cont2)
			rotacionswing += 3.0f;
		else
			banderaswing = 2;
		break;
	case 2:
		if (rotacionswing > 0.0f)
			rotacionswing -= 3.0f;
		else
			banderaswing = 3;
		break;
	case 3:
		if (rotacionswing > cont3)
			rotacionswing -= 3.0f;
		else
			banderaswing = 4;
		break;
	case 4:
		if (rotacionswing < 0.0f)
			rotacionswing += 3.0f;
		else
			banderaswing = 5;
		break;
	case 5:
		cont2 -= 3.0f;
		cont3 += 3.0f;
		if (cont2 == 0.0f or cont3 == 0.0f) {
			cont2 = 45.0f;
			cont3 = -42.0f;
			banderaswing = 0;
		}
		else
			banderaswing = 1;
		break;
	default:
		break;
	}
}


void animatepin(void) {
	if (true) {
		switch (estadospin) {
		case 1:
			if (posZpin < -30.0f)
				posZpin += 0.1f;
			else
				estadospin = 2;
			break;
		case 2:
			if (posXpin > -227.0f and posZpin > -37.0f) {
				posXpin -= 0.1f;
				posZpin -= 0.1f;
				posYpin -= 0.08f;
			}
			else
				estadospin = 3;
			break;
		case 3:
			if (posXpin < -220.0f and posZpin > -44.0f) {
				posXpin += 0.1f;
				posZpin -= 0.1f;
				posYpin -= 0.08f;
			}
			else
				estadospin = 4;
			break;
		case 4:
			if (posXpin < -213.0f and posZpin < -37.0f) {
				posXpin += 0.1f;
				posZpin += 0.1f;
				posYpin -= 0.07f;
			}
			else
				estadospin = 5;
			break;
		case 5:
			if (posXpin > -220.0f and posZpin < -30.0f) {
				posXpin -= 0.1f;
				posZpin += 0.1f;
				posYpin -= 0.07f;
			}
			else
				estadospin = 6;
			break;
		case 6:
			if (posZpin > -37.0f) {
				posZpin -= 0.1f;
			}
			else
				estadospin = 7;
			break;
		case 7:
			if (posYpin < 30.0f) {
				posYpin += 0.1f;
			}
			else
				estadospin = 8;
			break;
		case 8:
			posXpin = -220.0f;
			posYpin = 40.0f;
			posZpin = -37.0f;
			estadospin = 1;
			break;

		default:
			break;
		}
	}
	movepin = glm::vec3(posXpin, posYpin, posZpin);
}

void animatenino(void) {
	if (true) {
		switch (estadosnino) {
		case 1:
			rotanino = 0.0f;
			if (posXnino < -125.0f)
				posXnino += 2.0f;
			else
				estadosnino = 2;
			break;
		case 2:
			rotanino = 20.0f;
			if (posXnino < 120.0f and posZnino > -30 ) 
			{
				posXnino += 1.5f;
				posZnino -= 0.6f;
			}
			else
				estadosnino = 3;
			break;

		case 3:
			rotanino = 0.0f;
			if (posXnino < 332.0f ) {
				posXnino += 1.5f;
			}
			else
				estadosnino = 4;
			break;

		case 4:
			rotanino = 90.0f;
			if (posZnino > -334.0f) {
				posZnino -= 1.5f;
			}
			else
				estadosnino = 5;
			break;

		case 5:
			rotanino = 180.0f;
			if (posXnino > 30.0f) {
				posXnino -= 1.5f;
			}
			else
				estadosnino = 6;
			break;

		case 6:
			rotanino = 270.0f;
			if (posZnino < -40.0f) {
				posZnino += 1.5f;
			}
			else
				estadosnino = 7;
			break;

		case 7:
			rotanino = 200.0f;
			if (posXnino > -130.0f and posZnino < 195) {
				posXnino -= 1.5f;
				posZnino += 0.6f;
			}
			else
				estadosnino = 8;
			break;

		case 8:
			rotanino = 180.0f;
			if (posXnino > -342.0f) {
				posXnino -= 1.5f;
			}
			else
				estadosnino = 9;
			break;

		case 9:
			rotanino = 270.0f;
			if (posZnino < 62.0f) {
				posZnino += 1.5f;
			}
			else
				estadosnino = 1;
			break;
		
		default:
			break;
		}
	}
	movenino = glm::vec3(posXnino, 0.0f, posZnino);
}


int main() {
	glfwInit();

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
	if (window == NULL) {
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
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
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

	vector<std::string> faces1 //Amanecer
		{
			"resources/skybox/AnochecerRight.jpg",
			"resources/skybox/AnochecerLeft.jpg",
			"resources/skybox/AnochecerTop.jpg",
			"resources/skybox/AnochecerBottom.jpg",
			"resources/skybox/AnochecerFront.jpg",
			"resources/skybox/AnochecerBack.jpg"
		};

	vector<std::string> faces2 //Dia
		{
			"resources/skybox/DiaRight.jpg",
			"resources/skybox/DiaLeft.jpg",
			"resources/skybox/DiaTop.jpg",
			"resources/skybox/DiaBottom.jpg",
			"resources/skybox/DiaFront.jpg",
			"resources/skybox/DiaBack.jpg"
		};

	vector<std::string> faces4 //Noche
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
	Model comedor_M("resources/objects/comedor/all_comedor.obj");
	Model fuente_M("resources/objects/fuente/fuente.obj");
	Model puesto_M("resources/objects/puesto/puestofyv.obj");
	Model basecolumpio_M("resources/objects/juegos/base_columpio.obj");
	Model asientocolumpio_M("resources/objects/juegos/asiento_columpio.obj");
	//Model allpalmeras1_M("resources/objects/palmeras/all_palmeras1.obj");
	//Model allpalmeras2_M("resources/objects/palmeras/all_palmeras2.obj");
	//Model allpalmeras3_M("resources/objects/palmeras/all_palmeras3.obj");
	Model puerta1_M("resources/objects/puertas/puerta1.obj");
	Model puerta2_M("resources/objects/puertas/puerta2.obj");
	Model carro("resources/objects/Golf/sportcar.017.obj");
	Model nube_M("resources/objects/nube/Nube.obj");
	Model anuncio_M("resources/objects/anuncio/anuncio.obj");
	Model pina_M("resources/objects/anuncio/pina.obj");
	Model patinetanino_M("resources/objects/nino/ninopatineta.obj");
	//Model arbol1_M("resources/objects/plantas/OC13_Howea_forsteriana_Kentia_Palm/arbol1.obj");

	ModelAnim personaje("resources/objects/Personaje/Walking.dae");
	personaje.initShaders(animShader.ID);

	KeyFrame[0].movNube_x = -150;
	KeyFrame[0].movNube_y = 0;
	KeyFrame[0].movNube_z = 0;

	KeyFrame[1].movNube_x = -75;
	KeyFrame[1].movNube_y = 0;
	KeyFrame[1].movNube_z = 75;

	KeyFrame[2].movNube_x = 0;
	KeyFrame[2].movNube_y = 0;
	KeyFrame[2].movNube_z = 150;

	KeyFrame[3].movNube_x = 75;
	KeyFrame[3].movNube_y = 0;
	KeyFrame[3].movNube_z = 75;

	KeyFrame[4].movNube_x = 150;
	KeyFrame[4].movNube_y = 0;
	KeyFrame[4].movNube_z = 0;

	KeyFrame[5].movNube_x = 75;
	KeyFrame[5].movNube_y = 0;
	KeyFrame[5].movNube_z = -75;

	KeyFrame[6].movNube_x = 0;
	KeyFrame[6].movNube_y = 0;
	KeyFrame[6].movNube_z = -150;

	KeyFrame[7].movNube_x = -75;
	KeyFrame[7].movNube_y = 0;
	KeyFrame[7].movNube_z = -75;

	KeyFrame[8].movNube_x = -150;
	KeyFrame[8].movNube_y = 0;
	KeyFrame[8].movNube_z = 0;


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window)) {
		skyboxShader.use();
		animate();
		cambioCamara();
		animatedoor();
		animacionCarro();
		animatecolumpio();
		animatepin();
		animatenino();



		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();


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
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.0f + tiempoLuz, 0.0f + tiempoLuz, 0.0f + tiempoLuz));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

		//Fuentes de luz posicionales (Replicar los focos, lamparas, etc)
		staticShader.setVec3("pointLight[0].position", glm::vec3(-90.692f, 60.0f, 407.02f));
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(Noche));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(Noche));
		staticShader.setFloat("pointLight[0].constant", 0.002f);
		staticShader.setFloat("pointLight[0].quadratic", 0.0012f); // intensidad de la luz

		staticShader.setVec3("pointLight[1].position", glm::vec3(466.18f, 60.0f, 409.2f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(Noche));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(Noche));
		staticShader.setFloat("pointLight[1].constant", 0.002f);
		staticShader.setFloat("pointLight[1].quadratic", 0.0012f); // intensidad de la luz

		staticShader.setVec3("pointLight[2].position", glm::vec3(-301.64f, 60.0f, 407.008f));
		staticShader.setVec3("pointLight[2].ambient", glm::vec3(Noche));
		staticShader.setVec3("pointLight[2].diffuse", glm::vec3(Noche));
		staticShader.setFloat("pointLight[2].constant", 0.002f);
		staticShader.setFloat("pointLight[2].quadratic", 0.0012f); // intensidad de la luz

		staticShader.setVec3("pointLight[3].position", glm::vec3(-113.626f, 60.0f, 185.486f));
		staticShader.setVec3("pointLight[3].ambient", glm::vec3(Noche));
		staticShader.setVec3("pointLight[3].diffuse", glm::vec3(Noche));
		staticShader.setFloat("pointLight[3].constant", 0.002f);
		staticShader.setFloat("pointLight[3].quadratic", 0.0012f); // intensidad de la luz

		staticShader.setVec3("pointLight[4].position", glm::vec3(-87.99f, 60.0f, 57.306f));
		staticShader.setVec3("pointLight[4].ambient", glm::vec3(Noche));
		staticShader.setVec3("pointLight[4].diffuse", glm::vec3(Noche));
		staticShader.setFloat("pointLight[4].constant", 0.002f);
		staticShader.setFloat("pointLight[4].quadratic", 0.0012f); // intensidad de la luz

		staticShader.setVec3("pointLight[5].position", glm::vec3(-204.98f, 60.0f, 7.5526f));
		staticShader.setVec3("pointLight[5].ambient", glm::vec3(Noche));
		staticShader.setVec3("pointLight[5].diffuse", glm::vec3(Noche));
		staticShader.setFloat("pointLight[5].constant", 0.002f);
		staticShader.setFloat("pointLight[5].quadratic", 0.0012f); // intensidad de la luz

		staticShader.setVec3("pointLight[6].position", glm::vec3(-345.28f, 60.0f, 73.428f));
		staticShader.setVec3("pointLight[6].ambient", glm::vec3(Noche));
		staticShader.setVec3("pointLight[6].diffuse", glm::vec3(Noche));
		staticShader.setFloat("pointLight[6].constant", 0.002f);
		staticShader.setFloat("pointLight[6].quadratic", 0.0012f); // intensidad de la luz

		staticShader.setVec3("pointLight[7].position", glm::vec3(-267.68f, 60.0f, -153.78f));
		staticShader.setVec3("pointLight[7].ambient", glm::vec3(Noche2));
		staticShader.setVec3("pointLight[7].diffuse", glm::vec3(Noche2));
		staticShader.setFloat("pointLight[7].constant", 0.002f);
		staticShader.setFloat("pointLight[7].quadratic", 0.0012f); // intensidad de la luz

		staticShader.setVec3("pointLight[8].position", glm::vec3(-447.18f, 60.0f, -375.28f));
		staticShader.setVec3("pointLight[8].ambient", glm::vec3(Noche2));
		staticShader.setVec3("pointLight[8].diffuse", glm::vec3(Noche2));
		staticShader.setFloat("pointLight[8].constant", 0.002f);
		staticShader.setFloat("pointLight[8].quadratic", 0.0012f); // intensidad de la luz

		staticShader.setVec3("pointLight[9].position", glm::vec3(85.508f, 60.0f, -278.88f));
		staticShader.setVec3("pointLight[9].ambient", glm::vec3(Noche));
		staticShader.setVec3("pointLight[9].diffuse", glm::vec3(Noche));
		staticShader.setFloat("pointLight[9].constant", 0.002f);
		staticShader.setFloat("pointLight[9].quadratic", 0.0012f); // intensidad de la luz

		staticShader.setVec3("pointLight[10].position", glm::vec3(277.32f, 60.0f, -277.4f));
		staticShader.setVec3("pointLight[10].ambient", glm::vec3(Noche));
		staticShader.setVec3("pointLight[10].diffuse", glm::vec3(Noche));
		staticShader.setFloat("pointLight[10].constant", 0.002f);
		staticShader.setFloat("pointLight[10].quadratic", 0.0012f); // intensidad de la luz

		staticShader.setVec3("pointLight[11].position", glm::vec3(277.06f, 60.0f, -86.096f));
		staticShader.setVec3("pointLight[11].ambient", glm::vec3(Noche));
		staticShader.setVec3("pointLight[11].diffuse", glm::vec3(Noche));
		staticShader.setFloat("pointLight[11].constant", 0.002f);
		staticShader.setFloat("pointLight[11].quadratic", 0.0012f); // intensidad de la luz

		staticShader.setVec3("pointLight[12].position", glm::vec3(86.034f, 60.0f, -88.108f));
		staticShader.setVec3("pointLight[12].ambient", glm::vec3(Noche));
		staticShader.setVec3("pointLight[12].diffuse", glm::vec3(Noche));
		staticShader.setFloat("pointLight[12].constant", 0.002f);
		staticShader.setFloat("pointLight[12].quadratic", 0.0012f); // intensidad de la luz

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

		// -------------------------------------------------
		// Personaje principal - animado
		// -------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(PosX + PersonaX1, 10.0f, PosY + PersonaX2));
		model = glm::rotate(model, glm::radians(rotacion), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.08f));
		animShader.setMat4("model", model);
		personaje.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		// -------------------------------------------------------------------------------------------------------------------------
		// Carro
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(movAuto_x, movAuto_y, movAuto_z));
		model = glm::scale(model, glm::vec3(12.0f));
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		carro.Draw(staticShader);

		//------------------------------------------------------------------------------------------

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		escenario_M.Draw(staticShader);

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
		model = glm::scale(model, glm::vec3(0.3f, 0.4f, 0.3f));
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

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		comedor_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		fuente_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		puesto_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-205.0f, 30.0f, 125.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		basecolumpio_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-205.0f, 30.0f, 125.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotacionswing), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		asientocolumpio_M.Draw(staticShader);
		/*
		//Palmeras
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		allpalmeras1_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		allpalmeras2_M.Draw(staticShader);
		
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		allpalmeras3_M.Draw(staticShader);
		*/
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-101.0f, 0.0f, 270.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(rotaciondoor1), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		puerta1_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 270.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(rotaciondoor2), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		puerta2_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		anuncio_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(movepin));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		pina_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(movenino));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotanino), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		patinetanino_M.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f + movNube_x, 180.0f, 0.0f + movNube_z));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		nube_M.Draw(staticShader);

		

		/*Palmera prueba
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		arbol1_M.Draw(staticShader);*/


		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

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
		if (deltaTime < LOOP_TIME) {
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
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (terceraPersona == true) {
			camera.ProcessKeyboard(FORWARD, (float)deltaTime);
			if (guardado == false) {
				PosX = camera.Position.x;
				PosY = camera.Position.z;
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (terceraPersona == true) {
			camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
			if (guardado == false) {
				PosX = camera.Position.x;
				PosY = camera.Position.z;
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
		if (terceraPersona == true) {
			if (guardado == false) {
				PosX = camera.Position.x;
				PosY = camera.Position.z;
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
		if (terceraPersona == true) {
			if (guardado == false) {
				PosX = camera.Position.x;
				PosY = camera.Position.z;
			}
		}
	}


	//Camara aerea y tercera persona
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		terceraPersona = false;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		terceraPersona = true;

	//Animacion abrir y cerrar puerta
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		banderadoor = true;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		banderadoor = false;

	//Animacion columpio
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		banderaswing = 1;
	
	//Prender y apagar luces
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		Noche2 = 1.0f; //Prende el foco de las piscina
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		Noche2 = 0.0f; //Apaga el foco de la piscina
	
	//Niño en patineta
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		estadosnino = 1; //Activa
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		estadosnino = 0; //Apaga
		


	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		if (play == false && (FrameIndex > 1)) {
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else {
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		Inicio = true;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
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

		if (guardado == false) {
			camera.ProcessMouseMovement(xoffset, 0);

			if (xoffset > 0) {
				PersonaX1 = 15 * (camera.Front.x);
				PersonaX2 = 15 * (camera.Front.z);
			}
			if (xoffset < 0) {
				PersonaX1 = 15 * (camera.Front.x);
				PersonaX2 = 15 * (camera.Front.z);
			}
			if (((PosY - 10) - (PosY + PersonaX2)) > 0 and ((PosX - 10) - (PosX + PersonaX1)) < 0 and ((10 + PosX) - (PosX + PersonaX1)) > 0) {
				rotacion = 180.0f;
			}
			if (((10 + PosY) - (PosY + PersonaX2)) < 0 and ((PosX - 10) - (PosX + PersonaX1)) < 0 and ((10 + PosX) - (PosX + PersonaX1)) > 0) {
				rotacion = 0.0f;
			}
			if (((PosX - 10) - (PosX + PersonaX1)) > 0 and ((PosY - 10) - (PosY + PersonaX2)) < 0 and ((10 + PosY) - (PosY + PersonaX2)) > 0) {
				rotacion = 270.0f;
			}
			if (((10 + PosX) - (PosX + PersonaX1)) < 0 and ((PosY - 10) - (PosY + PersonaX2)) < 0 and ((10 + PosY) - (PosY + PersonaX2)) > 0) {
				rotacion = 90.0f;
			}
		}
	}
	if (terceraPersona == false)
		camera.ProcessMouseMovement(xoffset, -90);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}