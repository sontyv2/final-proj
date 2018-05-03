#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader_m.h"
#include "camera.h"
#include "data.h"

#include <iostream>

#include <unordered_map>
#include <string>
#include <algorithm>
#include <utility>
#include <vector>
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif
using namespace std;




void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

unsigned int loadCubemap(vector<std::string> faces);
void callPython(long maze_w, long maze_h);
glm::vec3* mazeboxCoords(glm::vec3 box_dimensions, string filename);
glm::vec3* powerupCoords(glm::vec3 box_dimensions, string filename);
int boxCount(string filename);
std::vector<int> getCubeTypes(string filename);


// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(-4.0f, 0.0f, 7.5f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// cube Positions
glm::vec3* cubePositions;
glm::vec3 cubeSize = glm::vec3(3, 3, 3);
unordered_map<float, vector<glm::vec3 *> *> map;
int numBoxes;

// powerup position
glm::vec3* powerupPositions;
glm::vec3 powerupSize = glm::vec3(1, 1, 1);
int numPowerups;

// cube highlights
bool mirror_highlights = false;
bool glass_highlights = false;
bool mirror_solid = false;
bool glass_solid = false;
bool canpress1 = true;
bool canpress2 = true;
bool canpress3 = true;
bool canpress4 = true;
bool canpress5 = true;

bool movable = true;
glm::vec3 oldCameraPos;
float oldCameraPitch;
float oldCameraYaw;
float mazeWidth = 10;
float mazeHeight = 7;


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "A-maze-ing Game", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("7.4.camera.vs", "7.4.camera.fs");
	Shader reflection("mirror.vs", "mirror.fs");
	Shader reflection_high("mirror.vs", "mirror_high.fs");
	Shader refraction("mirror.vs", "glass.fs");
	Shader refraction_high("mirror.vs", "glass_high.fs");
	Shader skyboxShader("skybox.vs", "skybox.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	// world params
	//float floorHeight = 0.0f;
	//float floorWidth = 2.0f;
	//float elevation = floorHeight - floorWidth / 2.0f + mazeHeight / 2.0f;

	// world space positions of our cubes
	callPython(mazeWidth, mazeHeight);
	numBoxes = boxCount("maze.txt");
	cubePositions = mazeboxCoords(cubeSize, "maze.txt");
	std::vector<int> cubeTypes = getCubeTypes("maze.txt");

	// hashmap begin
	//for (const auto &entry : map) {
	//	delete(entry.second);
	//}
	//map.clear();
	//for (int i = 0; i< numBoxes; ++i) {
	//	float hash1 = floor(cubePositions[i][0] / cubeSize[0]/3.0f)*31 + floor(cubePositions[i][2] / cubeSize[2] / 3.0f);
	//	if (map[hash1] == nullptr) {
	//		//cout << "Map doesn't exist";
	//		// new __ returns pointer to __ object
	//		map[hash1] = new vector<glm::vec3*>();
	//	}
	//	map[hash1]->emplace_back(&(cubePositions[i]));
	//}


	glm::vec3* cubeScalings = new glm::vec3[numBoxes];
	std::fill_n(cubeScalings, numBoxes, cubeSize);

	numPowerups = 5;
	powerupPositions = powerupCoords(cubeSize, "maze.txt");
	glm::vec3* powerupScalings = new glm::vec3[numPowerups];
	std::fill_n(powerupScalings, numPowerups, powerupSize);
	//powerupPosition = glm::vec3(-1.0f, -1.0f, 7.5f);
	//glm::vec3 powerupScalings = glm::vec3(powerupSize);

	// regular cube VAO
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// reflective cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// skybox VAO
	unsigned int skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// powerup VAO
	unsigned int powerVBO, powerVAO;
	glGenVertexArrays(1, &powerVAO);
	glGenBuffers(1, &powerVBO);
	glBindVertexArray(powerVAO);
	glBindBuffer(GL_ARRAY_BUFFER, powerVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// texture 2
	// ---------
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	// skybox texture
	vector<std::string> faces =
	{
		"pond/posx.jpg",
		"pond/negx.jpg",
		"pond/posy.jpg",
		"pond/negy.jpg",
		"pond/posz.jpg",
		"pond/negz.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	reflection.use();
	reflection.setInt("skybox", 0);

	refraction.use();
	refraction.setInt("skybox", 0);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// activate shader
		ourShader.use();

		// pass projection matrix to shaders (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		// render powerups
		for (unsigned int i = 0; i < numPowerups; ++i) {
			glm::mat4 pModel;
			pModel = glm::translate(pModel, powerupPositions[i]);
			pModel = glm::scale(pModel, powerupScalings[i]);
			ourShader.use();
			ourShader.setMat4("model", pModel);
			glBindVertexArray(powerVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture2);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}


		//render boxes

		for (unsigned int i = 0; i < numBoxes; ++i) {
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			float angle = 0; // 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::scale(model, cubeScalings[i]);

			// normal boxes
			if (cubeTypes[i] == 1) {
				ourShader.use();
				ourShader.setMat4("model", model);

				glBindVertexArray(VAO);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture1);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glBindVertexArray(0);
			}
			// mirror boxes
			else if (cubeTypes[i] == 2) {
				if (mirror_solid) {
					ourShader.use();
					ourShader.setMat4("model", model);

					glBindVertexArray(VAO);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texture1);
					glDrawArrays(GL_TRIANGLES, 0, 36);
					glBindVertexArray(0);
				}
				else {
					if (mirror_highlights) {
						reflection_high.use();
						reflection_high.setMat4("model", model);
						reflection_high.setMat4("view", view);
						reflection_high.setMat4("projection", projection);
						reflection_high.setVec3("cameraPos", camera.Position);
					}
					else {
						reflection.use();
						reflection.setMat4("model", model);
						reflection.setMat4("view", view);
						reflection.setMat4("projection", projection);
						reflection.setVec3("cameraPos", camera.Position);
					}
					// cubes
					glBindVertexArray(cubeVAO);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
					glDrawArrays(GL_TRIANGLES, 0, 36);
					glBindVertexArray(0);
				}
			}
			// glass boxes
			else if (cubeTypes[i] == 3) {
				if (glass_solid) {
					ourShader.use();
					ourShader.setMat4("model", model);

					glBindVertexArray(VAO);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texture1);
					glDrawArrays(GL_TRIANGLES, 0, 36);
					glBindVertexArray(0);
				}
				else {
					if (glass_highlights) {
						refraction_high.use();
						refraction_high.setMat4("model", model);
						refraction_high.setMat4("view", view);
						refraction_high.setMat4("projection", projection);
						refraction_high.setVec3("cameraPos", camera.Position);
					}
					else {
						refraction.use();
						refraction.setMat4("model", model);
						refraction.setMat4("view", view);
						refraction.setMat4("projection", projection);
						refraction.setVec3("cameraPos", camera.Position);
					}
					// cubes
					glBindVertexArray(cubeVAO);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
					glDrawArrays(GL_TRIANGLES, 0, 36);
					glBindVertexArray(0);
				}
			}
		}

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

							  // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
							  // -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);
	glDeleteVertexArrays(1, &powerVAO);
	glDeleteBuffers(1, &powerVBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		glfwSetWindowTitle(window, "A-maze-ing Game");
		camera.ProcessKeyboard(movable, FORWARD, deltaTime, cubePositions, cubeSize, numBoxes, map);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		glfwSetWindowTitle(window, "Take it back now y'all.");
		camera.ProcessKeyboard(movable, BACKWARD, deltaTime, cubePositions, cubeSize, numBoxes, map);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		glfwSetWindowTitle(window, "Don't get left behind!");
		camera.ProcessKeyboard(movable, LEFT, deltaTime, cubePositions, cubeSize, numBoxes, map);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		glfwSetWindowTitle(window, "Right on.");
		camera.ProcessKeyboard(movable, RIGHT, deltaTime, cubePositions, cubeSize, numBoxes, map);
	}

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		if (canpress1) {
			mirror_highlights = !mirror_highlights;
			canpress1 = false;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE) {
		canpress1 = true;
	}

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		if (canpress2) {
			glass_highlights = !glass_highlights;
			canpress2 = false;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE) {
		canpress2 = true;
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		if (canpress3) {
			mirror_solid = !mirror_solid;
			canpress3 = false;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_RELEASE) {
		canpress3 = true;
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		if (canpress4) {
			glass_solid = !glass_solid;
			canpress4 = false;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE) {
		canpress4 = true;
	}

	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		if (canpress5) {
			if (movable) {
				movable = false;
				oldCameraPos = camera.getPosition();
				oldCameraPitch = camera.getPitch();
				oldCameraYaw = camera.getYaw();
				camera.setPosition(glm::vec3(-20, 80, 3.0f*mazeWidth*cubeSize[2] / 2.0f));
				camera.setPitch(-70.f);
				camera.setYaw(0.0f);
				canpress5 = false;
			}
			else {
				movable = true;
				camera.setPosition(oldCameraPos);
				camera.setPitch(oldCameraPitch);
				camera.setYaw(oldCameraYaw);
				canpress5 = false;
			}
			camera.ProcessMouseMovement(0.01f, 0.01f);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE) {
		canpress5 = true;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		std::vector<bool> del {false, false, false, false};
		int initialnumPowerups = numPowerups;
		for (int i = 0; i < initialnumPowerups; ++i) {
			glm::vec3 powerpos = powerupPositions[i];
			glm::vec3 campos = camera.getPosition();
			if (glm::abs(campos[0] - powerpos[0]) < 1.5f) {
				if (glm::abs(campos[2] - powerpos[2]) < 1.5f) {
					numPowerups--;
					del[i] = true;
				}
			}
		}
		glm::vec3* newPositions = new glm::vec3[numPowerups];
		int count = 0;
		for (int i = 0; i < initialnumPowerups; ++i) {
			if (!del[i]) {
				newPositions[count] = powerupPositions[i];
				count++;
			}
		}
		powerupPositions = newPositions;
	}

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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void callPython(long maze_w = 16, long maze_h = 8) {
	Py_Initialize();
	PyObject *pName, *pModule, *pDict, *pFunc, *pArgs, *pWidth, *pHeight;
	pName = PyString_FromString("mazegen");

	pModule = PyImport_Import(pName);
	pDict = PyModule_GetDict(pModule);
	pFunc = PyDict_GetItemString(pDict, "to_call");
	pArgs = PyTuple_New(2);
	pWidth = PyInt_FromLong(maze_w);
	pHeight = PyInt_FromLong(maze_h);
	PyTuple_SetItem(pArgs, 0, pWidth);
	PyTuple_SetItem(pArgs, 1, pHeight);
	PyObject* pResult = PyObject_CallObject(pFunc, pArgs);

	Py_Finalize();

}

glm::vec3* mazeboxCoords(glm::vec3 box_dimensions, string filename) {

	string line;
	ifstream mazeFile(filename);

	int maze_w = 0;
	int maze_h = 0;

	std::vector< std::vector<char> > arr;

	//int numBoxes = boxCount(filename);

	if (mazeFile.is_open()) {
		while (getline(mazeFile, line)) {
			if (line == "") {
				break;
			}
			cout << line << '\n';
			arr.push_back(std::vector<char>());
			for (unsigned int i = 0; i < line.length(); ++i) {
				char c = line[i];
				arr[maze_h].push_back(c);
			}
			maze_h += 1;
		}
		mazeFile.close();
	}
	else {
		cout << "Unable to open file";
	}

	maze_w = arr[0].size();

	glm::vec3* boxes = new glm::vec3[numBoxes];
	int count = 0;

	for (int r = 0; r < maze_h; ++r) {
		for (int c = 0; c < maze_w; ++c) {

			if (arr[r][c] != '0' && arr[r][c] != '4') {
				float x, y, z;
				x = r * box_dimensions.x + (box_dimensions.x / 2.0);
				z = c * box_dimensions.y + (box_dimensions.y / 2.0);
				y = 0;
				boxes[count] = glm::vec3(x, y, z);
				++count;
			}
		}
	}

	return boxes;

}

glm::vec3* powerupCoords(glm::vec3 box_dimensions, string filename) {

	string line;
	ifstream mazeFile(filename);

	int maze_w = 0;
	int maze_h = 0;

	std::vector< std::vector<char> > arr;

	if (mazeFile.is_open()) {
		while (getline(mazeFile, line)) {
			if (line == "") {
				break;
			}
			cout << line << '\n';
			arr.push_back(std::vector<char>());
			for (unsigned int i = 0; i < line.length(); ++i) {
				char c = line[i];
				arr[maze_h].push_back(c);
			}
			maze_h += 1;
		}
		mazeFile.close();
	}
	else {
		cout << "Unable to open file";
	}

	maze_w = arr[0].size();

	glm::vec3* powerups = new glm::vec3[5];
	powerups[0] = glm::vec3(-1.0f, -1.0f, 7.5f);
	int count = 1;

	for (int r = 0; r < maze_h; ++r) {
		for (int c = 0; c < maze_w; ++c) {
			if (arr[r][c] == '4') {
				float x, y, z;
				x = r * box_dimensions.x + (box_dimensions.x / 2.0);
				z = c * box_dimensions.y + (box_dimensions.y / 2.0);
				y = -1.0f;
				powerups[count] = glm::vec3(x, y, z);
				++count;
			}
		}
	}

	return powerups;

}

int boxCount(string filename) {

	string line;
	ifstream mazeFile(filename);

	int maze_w = 0;
	int maze_h = 0;

	std::vector< std::vector<char> > arr;

	if (mazeFile.is_open()) {
		while (getline(mazeFile, line)) {
			if (line == "") {
				break;
			}
			arr.push_back(std::vector<char>());
			for (unsigned int i = 0; i < line.length(); ++i) {
				char c = line[i];
				arr[maze_h].push_back(c);
			}
			maze_h += 1;
		}
		mazeFile.close();
	}
	else {
		cout << "Unable to open file";
	}

	maze_w = arr[0].size();

	int numboxes = 0;
	for (int r = 0; r < maze_h; ++r) {
		for (int c = 0; c < maze_w; ++c) {
			if (arr[r][c] != '0' && arr[r][c] != '4') {
				++numboxes;
			}
		}
	}
	return numboxes;
}

std::vector<int> getCubeTypes(string filename) {

	string line;
	ifstream mazeFile(filename);

	int maze_w = 0;
	int maze_h = 0;

	std::vector< std::vector<char> > arr;

	if (mazeFile.is_open()) {
		while (getline(mazeFile, line)) {
			if (line == "") {
				break;
			}
			arr.push_back(std::vector<char>());
			for (unsigned int i = 0; i < line.length(); ++i) {
				char c = line[i];
				arr[maze_h].push_back(c);
			}
			maze_h += 1;
		}
		mazeFile.close();
	}
	else {
		cout << "Unable to open file";
	}

	maze_w = arr[0].size();

	std::vector<int> types;

	for (int r = 0; r < maze_h; ++r) {
		for (int c = 0; c < maze_w; ++c) {
			if (arr[r][c] != '0' && arr[r][c] != '4') {
				int num = arr[r][c] - '0';
				types.push_back(num);
			}

		}
	}
	return types;

}