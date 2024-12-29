#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <render/shader.h>

#define STB_IMAGE_IMPLEMENTATION
//#include <stb/stb_image.h>

// GLTF model loader
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tinygltf-2.9.3/tiny_gltf.h>

#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctime>  // For time() function
#include <iomanip>

static GLFWwindow *window;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

 //old camera definition
// OpenGL camera view parameters
static glm::vec3 eye_center(100.0f, 0.0f, 100.0f);
static glm::vec3 lookat(-50, 0, 5000000);

static glm::vec3 up(0, 1, 0);

// View control 
static float viewAzimuth = 0.1f;
static float viewPolar = 0.f;
static float viewDistance = 500.0f;


// Lighting control
const glm::vec3 wave500(0.0f, 255.0f, 146.0f);
const glm::vec3 wave600(255.0f, 190.0f, 0.0f);
const glm::vec3 wave700(205.0f, 0.0f, 0.0f);
static glm::vec3 lightIntensity = 5.0f * (8.0f * wave500 + 15.6f * wave600 + 18.4f * wave700);
static glm::vec3 lightPosition(-275.0f, 500.0f, -275.0f);

//shadows
static glm::vec3 lightUp(0, 0, 1);
static int shadowMapWidth = 1024;
static int shadowMapHeight = 1024;
//GLuint depthTexture, fbo;

// TODO: set these parameters
static float depthFoV = 45.f;
static float depthNear = 100.f;
static float depthFar = 2000.f;

// Animation
static bool playAnimation = true;
static float playbackSpeed = 2.0f;

static GLuint LoadTextureTileBox(const char *texture_file_path) {
    int w, h, channels;
    uint8_t* img = stbi_load(texture_file_path, &w, &h, &channels, 3);
    GLuint texture;
    glGenTextures(1, &texture);  
    glBindTexture(GL_TEXTURE_2D, texture);  

    // To tile textures on a box, we set wrapping to repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (img) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture " << texture_file_path << std::endl;
    }
    stbi_image_free(img);

    return texture;
}
//function to simulate endless effet
void resetEye(float gridsize) {
	if (eye_center.x >= gridsize * 100) {
		eye_center.x -= gridsize * 100;
	}
	if (eye_center.z >= gridsize * 100) {
		eye_center.z -= gridsize * 100;
	}
	if (eye_center.x <= 0) {
		eye_center.x += gridsize * 100;
	}
	if (eye_center.z <= 0) {
		eye_center.z += gridsize * 100;
	}
}

/*
//below is code for simulating an enless effect
struct Ground {
	GLuint vpMatrixID;
	GLuint eyecenterID;
	GLuint gridSizeID;
	GLuint programID;

	void init() {
		// Load shaders
		programID = LoadShadersFromFile("../lab2/ground.vert", "../lab2/ground.frag");

		if (programID == 0) {
			std::cerr << "Failed to load shaders." << std::endl;
		}

		// Get uniform locations
		vpMatrixID = glGetUniformLocation(programID, "VP");
		eyecenterID = glGetUniformLocation(programID, "camPos");
		gridSizeID = glGetUniformLocation(programID, "gGridCCellSize");
	}

	void render(glm::mat4 cameraMatrix, glm::vec3 eyeCenter, float grid_size) {
		glUseProgram(programID);

		//glm::mat4 vp = cameraMatrix;
		//glm::vec3 camPos = eyeCenter;
		// Pass uniforms to shaders
		glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &cameraMatrix[0][0]);
		glUniform3fv(eyecenterID, 1, &eyeCenter[0]);
		glUniform1f(gridSizeID, grid_size);

		// Render the plane
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void cleanup() {
		glDeleteProgram(programID);
	}
};
*/


struct Building {
	glm::vec3 position;		// Position of the box 
	glm::vec3 scale;		// Size of the box in each axis
	
	GLfloat vertex_buffer_data[72] = {	// Vertex definition for a canonical box
		// Front face
		-1.0f, -1.0f, 1.0f, 
		1.0f, -1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f, 
		-1.0f, 1.0f, 1.0f, 
		
		// Back face 
		1.0f, -1.0f, -1.0f, 
		-1.0f, -1.0f, -1.0f, 
		-1.0f, 1.0f, -1.0f, 
		1.0f, 1.0f, -1.0f,
		
		// Left face
		-1.0f, -1.0f, -1.0f, 
		-1.0f, -1.0f, 1.0f, 
		-1.0f, 1.0f, 1.0f, 
		-1.0f, 1.0f, -1.0f, 

		// Right face 
		1.0f, -1.0f, 1.0f, 
		1.0f, -1.0f, -1.0f, 
		1.0f, 1.0f, -1.0f, 
		1.0f, 1.0f, 1.0f,

		// Top face
		-1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, -1.0f, 
		-1.0f, 1.0f, -1.0f, 

		// Bottom face
		-1.0f, -1.0f, -1.0f, 
		1.0f, -1.0f, -1.0f, 
		1.0f, -1.0f, 1.0f, 
		-1.0f, -1.0f, 1.0f, 
	};

	GLfloat color_buffer_data[72] = {
		// Front, red
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		// Back, yellow
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		// Left, green
		0.0f, 1.0f, 0.0f, 
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		// Right, cyan
		0.0f, 1.0f, 1.0f, 
		0.0f, 1.0f, 1.0f, 
		0.0f, 1.0f, 1.0f, 
		0.0f, 1.0f, 1.0f, 

		// Top, blue
		0.0f, 0.0f, 1.0f, 
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		// Bottom, magenta
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 
		1.0f, 0.0f, 1.0f, 
		1.0f, 0.0f, 1.0f,  
	};

	GLuint index_buffer_data[36] = {		// 12 triangle faces of a box
		0, 1, 2, 	
		0, 2, 3, 
		
		4, 5, 6, 
		4, 6, 7, 

		8, 9, 10, 
		8, 10, 11, 

		12, 13, 14, 
		12, 14, 15, 

		16, 17, 18, 
		16, 18, 19, 

		20, 21, 22, 
		20, 22, 23, 
	};

    // TODO: Define UV buffer data
    // ---------------------------
    GLfloat uv_buffer_data[48] = {
// Front
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
// Back
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
// Left
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
// Right
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
// Top - we do not want texture the top
            0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
// Bottom - we do not want texture the bottom
            0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
    };
    
	// OpenGL buffers
	GLuint vertexArrayID; 
	GLuint vertexBufferID; 
	GLuint indexBufferID; 
	GLuint colorBufferID;
	GLuint uvBufferID;
	GLuint textureID;

	// Shader variable IDs
	GLuint mvpMatrixID;
	GLuint lightPositionID;//added for lighting & shadows
	GLuint lightIntensityID;//
	GLuint textureSamplerID;
	GLuint programID;

	void initialize(glm::vec3 position, glm::vec3 scale) {
		// Define scale of the building geometry
		this->position = position;
		this->scale = scale;

		// Create a vertex array object
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		// Create a vertex buffer object to store the vertex data		
		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

		// Create a vertex buffer object to store the color data
        // TODO:
        for (int i = 0; i < 24; ++i) uv_buffer_data[2*i+1] *= 5;
		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

		// TODO: Create a vertex buffer object to store the UV data
		// --------------------------------------------------------
        // ---------------------------------------------------
        // Create a vertex buffer object to store the UV data
        glGenBuffers(1, &uvBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data,
                     GL_STATIC_DRAW);
		// Create an index buffer object to store the index data that defines triangle faces
		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

		// Create and compile our GLSL program from the shaders
		programID = LoadShadersFromFile("../lab2/project.vert", "../lab2/project.frag");
		if (programID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}

		// Get a handle for our "MVP" uniform
		mvpMatrixID = glGetUniformLocation(programID, "MVP");
		lightPositionID = glGetUniformLocation(programID, "lightPosition");//added lighting
		lightIntensityID = glGetUniformLocation(programID, "lightIntensity");//

        // TODO: Load a texture 
        // --------------------
        // --------------------
        textureID = LoadTextureTileBox("../lab2/cyber_building.jpg");

        // TODO: Get a handle to texture sampler 
        // -------------------------------------
        // -------------------------------------
        textureSamplerID = glGetUniformLocation(programID,"textureSampler");
	}

	void render(glm::mat4 cameraMatrix) {
		glUseProgram(programID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		// TODO: Model transform 
		// -----------------------

        glm::mat4 modelMatrix = glm::mat4();
        modelMatrix = glm::translate(modelMatrix, glm::vec3(position.x, position.y+ (scale.y), position.z));
        modelMatrix = glm::scale(modelMatrix, scale);

		// Set model-view-projection matrix
		glm::mat4 mvp = cameraMatrix * modelMatrix;
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

		// Set light data
		glUniform3fv(lightPositionID, 1, &lightPosition[0]);//
		glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);//

		// TODO: Enable UV buffer and texture sampler
		// ------------------------------------------
        // -----------------------------------------
		// Set textureSampler to use texture unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(textureSamplerID, 0);

		// Draw the box
		glDrawElements(
			GL_TRIANGLES,      // mode
			36,    			   // number of indices
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
	}

	void cleanup() {
		glDeleteBuffers(1, &vertexBufferID);
		glDeleteBuffers(1, &colorBufferID);
		glDeleteBuffers(1, &indexBufferID);
		glDeleteVertexArrays(1, &vertexArrayID);
		//glDeleteBuffers(1, &uvBufferID);
		//glDeleteTextures(1, &textureID);
		glDeleteProgram(programID);
	}
}; 
//define the skybox (from lab 2)

struct skyBox {
    glm::vec3 position;		// Position of the box
    glm::vec3 scale;		// Size of the box in each axis

    GLfloat vertex_buffer_data[72] = {	// Vertex definition for a canonical box
            // Front face
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,

            // Back face
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,

            // Left face
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            // Right face
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,

            // Top face
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            // Bottom face
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
    };

    GLfloat color_buffer_data[72] = {
            // Front, red
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,

            // Back, yellow
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,

            // Left, green
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,

            // Right, cyan
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,

            // Top, blue
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,

            // Bottom, magenta
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
    };

	GLuint index_buffer_data[36] = {		// 12 triangle faces of a box
		// Front face
		0, 2, 1,
		0, 3, 2,

		// Back face
		4, 6, 5,
		4, 7, 6,

		// Left face
		8, 10, 9,
		8, 11, 10,

		// Right face
		12, 14, 13,
		12, 15, 14,

		// Top face
		16, 18, 17,
		16, 19, 18,

		// Bottom face
		20, 22, 21,
		20, 23, 22,
	};


	//produces textures on outside of box, not inside
	/*
    GLuint index_buffer_data[36] = {		// 12 triangle faces of a box
            0, 1, 2,
            0, 2, 3,

            4, 5, 6,
            4, 6, 7,

            8, 9, 10,
            8, 10, 11,

            12, 13, 14,
            12, 14, 15,

            16, 17, 18,
            16, 18, 19,

            20, 21, 22,
            20, 22, 23,
    };
	*/



    GLfloat uv_buffer_data[48]={

            // Front
            1.0f, 0.665f,
            0.75f, 0.665f,
            0.75f, 0.334f,
            1.0f, 0.334f,

            // Back
            0.5f, 0.665f,
            0.25f, 0.665f,
            0.25f, 0.334f,
            0.5f, 0.334f,

            // Left
            0.25f, 0.665f,
            0.0f, 0.665f,
            0.0f, 0.334f,
            0.25f, 0.334f,

            // Right
            0.75f, 0.665f,
            0.5f, 0.665f,
            0.5f, 0.334f,
            0.75f, 0.334f,

            // Top - we do not want texture the top
            0.251f, 0.0f,
            0.499f, 0.0f,
            0.499f, 0.334f,
            0.251f, 0.334f,

            // Bottom - we do not want texture the bottom
            0.251f, 0.665f,
            0.499f, 0.665f,
            0.499f, 1.0f,
            0.251f, 1.0f,
    };

    // OpenGL buffers
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint colorBufferID;
    GLuint uvBufferID;
    GLuint textureID;

    // Shader variable IDs
    GLuint mvpMatrixID;
	GLuint lightPositionID;//added for lighting
	GLuint lightIntensityID;//
    GLuint textureSamplerID;
    GLuint programID;

    void initialize(glm::vec3 position, glm::vec3 scale) {
        // Define scale of the building geometry
        this->position = position;
        this->scale = scale;

        // Create a vertex array object
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        // Create a vertex buffer object to store the vertex data
        glGenBuffers(1, &vertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

        // Create a vertex buffer object to store the color data
        // TODO:
        glGenBuffers(1, &colorBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

        // TODO: Create a vertex buffer object to store the UV data
        // --------------------------------------------------------
        // ---------------------------------------------------
        // Create a vertex buffer object to store the UV data
        glGenBuffers(1, &uvBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data,
                     GL_STATIC_DRAW);
        // Create an index buffer object to store the index data that defines triangle faces
        glGenBuffers(1, &indexBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

        // Create and compile our GLSL program from the shaders
        programID = LoadShadersFromFile("../lab2/project.vert", "../lab2/project.frag");
        if (programID == 0)
        {
            std::cerr << "Failed to load shaders." << std::endl;
        }

        // Get a handle for our "MVP" uniform
        mvpMatrixID = glGetUniformLocation(programID, "MVP");
    	lightPositionID = glGetUniformLocation(programID, "lightPosition");//added for lighting
    	lightIntensityID = glGetUniformLocation(programID, "lightIntensity");//

        // TODO: Load a texture
        // --------------------
        // --------------------
        textureID = LoadTextureTileBox("../lab2/my_cave.png");

        // TODO: Get a handle to texture sampler
        // -------------------------------------
        // -------------------------------------
        textureSamplerID = glGetUniformLocation(programID,"textureSampler");
    }

    void render(glm::mat4 cameraMatrix, glm::vec3 camPos) {
        glUseProgram(programID);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

        // TODO: Model transform
        // -----------------------

        glm::mat4 modelMatrix = glm::mat4();

        modelMatrix = glm::translate(modelMatrix, glm::vec3(position.x, position.y+ (scale.y), position.z));
        modelMatrix = glm::scale(modelMatrix, scale);
        // -----------------------

        glm::mat4 mvp = cameraMatrix * modelMatrix;
        glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);


    	// Set light data
    	glUniform3fv(lightPositionID, 1, &lightPosition[0]);//
    	glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);//
    	//infinite plane implementation
		//pass camera position to shader
    	//GLuint camPosID = glGetUniformLocation(programID, "camPos");
    	//glUniform3fv(camPosID, 1, &eye_center[0]); // Pass camera position to the shader


        // TODO: Enable UV buffer and texture sampler
        // ------------------------------------------
        // ------------------------------------------
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
// Set textureSampler to use texture unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(textureSamplerID, 0);

        // Draw the box
        glDrawElements(
                GL_TRIANGLES,      // mode
                36,    			   // number of indices
                GL_UNSIGNED_INT,   // type
                (void*)0           // element array buffer offset
        );

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        //glDisableVertexAttribArray(2);
    }

    void cleanup() {
        glDeleteBuffers(1, &vertexBufferID);
        glDeleteBuffers(1, &colorBufferID);
        glDeleteBuffers(1, &indexBufferID);
        glDeleteVertexArrays(1, &vertexArrayID);
        //glDeleteBuffers(1, &uvBufferID);
        //glDeleteTextures(1, &textureID);
        glDeleteProgram(programID);
    }
};



//define the road strct

struct Road {
    glm::vec3 position;        // Position of the road
    glm::vec3 scale;           // Size of the road in each axis
    GLuint textureID;          // Texture ID for OpenGL

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint colorBufferID;
    GLuint uvBufferID;
    GLuint programID;
    GLuint mvpMatrixID;
    GLuint textureSamplerID;

    GLfloat vertex_buffer_data[72] = {
        // Front face
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,

        // Back face
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,

        // Left face
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        // Right face
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,

        // Top face
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        // Bottom face
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
    };

    GLfloat color_buffer_data[72] = {
        // Front, red
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        // Back, yellow
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,

        // Left, green
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        // Right, cyan
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,

        // Top, blue
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        // Bottom, magenta
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
    };


	//tezture appeared stretched and morphed
	//fix: create a new, rectangular tezxture to match dimesions of roads.
	GLfloat uv_buffer_data[48] = {
		// Assume the longer dimension is aligned horizontally for these faces
		// Front (narrow face, vertical road's cross-section)
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		// Back (same as Front)
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		// Left (long face, vertical road, texture should be scaled according to road length)
		0.0f, 10.0f, // The texture repeats 10 times along the road's length
		0.0f, 0.0f,
		1.0f, 10.0f,
		1.0f, 0.0f,

		// Right (same as Left)
		0.0f, 10.0f, // Adjust repetition according to actual road length
		0.0f, 0.0f,
		1.0f, 10.0f,
		1.0f, 0.0f,

		// Top (long face, horizontal road, texture matches dimensions)
		0.0f, 1.0f,
		10.0f, 1.0f, // Assuming the horizontal road is also long and the texture should repeat
		10.0f, 0.0f,
		0.0f, 0.0f,

		// Bottom (typically unseen, simple mapping)
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f
	};


    GLuint index_buffer_data[36] = {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7,
        8, 9, 10,
        8, 10, 11,
        12, 13, 14,
        12, 14, 15,
        16, 17, 18,
        16, 18, 19,
        20, 21, 22,
        20, 22, 23
    };

	GLuint lightPositionID;//
	GLuint lightIntensityID;//


	void initialize(glm::vec3 position, glm::vec3 scale, const char* textureFilePath) {
		this->position = position;
		this->scale = scale;

		// Load the specified texture
		textureID = LoadTextureTileBox(textureFilePath);

		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

		glGenBuffers(1, &uvBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);

		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

		programID = LoadShadersFromFile("../lab2/project.vert", "../lab2/project.frag");
		mvpMatrixID = glGetUniformLocation(programID, "MVP");
		lightPositionID = glGetUniformLocation(programID, "lightPosition");//added for lighting
		lightIntensityID = glGetUniformLocation(programID, "lightIntensity");//
		textureSamplerID = glGetUniformLocation(programID, "textureSampler");
	}


    void render(glm::mat4 cameraMatrix) {
        glUseProgram(programID);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::scale(modelMatrix, scale);

        glm::mat4 mvp = cameraMatrix * modelMatrix;
        glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
		// Set light data
		glUniform3fv(lightPositionID, 1, &lightPosition[0]);//
		glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);//

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(textureSamplerID, 0);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }

    void cleanup() {
        glDeleteBuffers(1, &vertexBufferID);
        glDeleteBuffers(1, &colorBufferID);
        glDeleteBuffers(1, &indexBufferID);
        glDeleteVertexArrays(1, &vertexArrayID);
        glDeleteTextures(1, &textureID);
        glDeleteProgram(programID);
    }
};


// Helper class to render skeleton based on given animation data
struct Skeleton {
	/*
	const std::string vertexShader = R"(
	#version 330 core

	layout(location = 0) in vec3 vertexPosition;

	uniform mat4 MVP;

	void main() {
		gl_Position =  MVP * vec4(vertexPosition, 1);
	}
	)";

	const std::string fragmentShader = R"(
	#version 330 core

	out vec3 finalColor;

	void main()
	{
		finalColor = vec3(0.2, 0.8, 0);
	}
	)";
	*/

	GLuint programID;
    GLuint mvpMatrixID;
	GLuint sphereVAO, sphereVBO, sphereEBO;
	int sphereIndexCount = 0;

	void initialize() {
		createSphereMesh(1.0f, 8, 8);
		programID = LoadShadersFromFile("../lab2/project.vert", "../lab2/project.frag");
        //programID = LoadShadersFromString(vertexShader, fragmentShader);
		if (programID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}

		mvpMatrixID = glGetUniformLocation(programID, "MVP");
	}

	void createSphereMesh(float radius, int sectorCount, int stackCount) {
		std::vector<GLfloat> vertices;
		std::vector<GLuint> indices;

		float x, y, z, xy;                            // vertex position
		float sectorStep = 2 * M_PI / sectorCount;
		float stackStep = M_PI / stackCount;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= stackCount; ++i) {
			stackAngle = M_PI / 2 - i * stackStep;    // from pi/2 to -pi/2
			xy = radius * cosf(stackAngle);           // r * cos(u)
			z = radius * sinf(stackAngle);            // r * sin(u)

			for (int j = 0; j <= sectorCount; ++j) {
				sectorAngle = j * sectorStep;         // from 0 to 2pi
				x = xy * cosf(sectorAngle);           // x = r * cos(u) * cos(v)
				y = xy * sinf(sectorAngle);           // y = r * cos(u) * sin(v)

				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);
			}
		}

		for (int i = 0; i < stackCount; ++i) {
			int k1 = i * (sectorCount + 1);
			int k2 = k1 + sectorCount + 1;

			for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
				if (i != 0) {
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				if (i != (stackCount - 1)) {
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}

		sphereIndexCount = indices.size();

		glGenVertexArrays(1, &sphereVAO);
		glGenBuffers(1, &sphereVBO);
		glGenBuffers(1, &sphereEBO);

		glBindVertexArray(sphereVAO);

		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	void renderSphere(const glm::vec3& position, float radius, const glm::mat4& viewProjMatrix) {
		glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(radius));

        glm::mat4 mvp = viewProjMatrix * modelMatrix;

		glUseProgram(programID);
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void renderLine(const glm::vec3& start, const glm::vec3& end, const glm::mat4& viewProjMatrix) {
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &viewProjMatrix[0][0]);

		GLfloat vertices[] = { start.x, start.y, start.z, end.x, end.y, end.z };
		GLuint VBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glUseProgram(programID);
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 2);

		glBindVertexArray(0);
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}

	void renderSkeleton(const tinygltf::Model& model,
        const tinygltf::Skin &skin,
        const std::vector<glm::mat4> &globalTransforms,
        const glm::mat4& viewProjMatrix)
    {
        if (skin.joints.size() <= 0) return;

		for (size_t j = 0; j < skin.joints.size(); ++j) {
            int nodeIndex = skin.joints[j];
            const tinygltf::Node &node = model.nodes[nodeIndex];
			if (node.translation.size() >= 0) {
                glm::vec3 jointPosition =
                    glm::vec3(globalTransforms[nodeIndex][3]);
				renderSphere(jointPosition, 2.0f, viewProjMatrix);

				for (const int childIndex : node.children) {
					const auto& childNode = model.nodes[childIndex];
					glm::vec3 childPosition =
                        glm::vec3(globalTransforms[childIndex][3]);
					renderLine(jointPosition, childPosition, viewProjMatrix);
				}
			}
		}
	}

};

// Our 3D character model
struct MyBot {
	tinygltf::Model model;

	// Animation data
	struct SamplerObject {
		std::vector<float> input;
		std::vector<glm::vec4> output;
		int interpolation;
	};
	struct ChannelObject {
		int sampler;
		std::string targetPath;
		int targetNode;
	};
	struct AnimationObject {
		std::vector<SamplerObject> samplers;
	};
	std::vector<AnimationObject> animationObjects;

	// The skeleton class for rendering the skeleton given the transforms
	// obtained from the animation object
    Skeleton skeleton;

	// The current global transforms for each joint
	// Update this will result in skeleton in different poses
    std::vector<glm::mat4> globalTransforms;

	// Parse the transformation data at a node and turn into a 4x4 matrix
	glm::mat4 getNodeTransform(const tinygltf::Node& node) {
		glm::mat4 transform(1.0f);

		if (node.matrix.size() == 16) {
			transform = glm::make_mat4(node.matrix.data());
		} else {
			if (node.translation.size() == 3) {
				transform = glm::translate(transform, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
			}
			if (node.rotation.size() == 4) {
				glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
				transform *= glm::mat4_cast(q);
			}
			if (node.scale.size() == 3) {
				transform = glm::scale(transform, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
			}
		}
		return transform;
	}

	void computeLocalNodeTransform(const tinygltf::Model& model,
		int nodeIndex,
		std::vector<glm::mat4> &localTransforms)
	{
		// TODO: your code here
		//recursively traverse skeleton
		const tinygltf::Node& node = model.nodes[nodeIndex];
		glm::mat4 transform = getNodeTransform(node);

		//store local transform in localTransforms
		localTransforms[nodeIndex] = transform;

		// Process child nodes
		for (int childIndex : node.children) {
			computeLocalNodeTransform(model, childIndex, localTransforms);
		}
	}

	void computeGlobalNodeTransform(const tinygltf::Model& model,
		const std::vector<glm::mat4> &localTransforms,
		int nodeIndex, const glm::mat4& parentTransform,
		std::vector<glm::mat4> &globalTransforms)
	{
		// TODO: your code here
		// global = parent * local
		glm::mat4 transform = parentTransform * localTransforms[nodeIndex];
		//save global transform
		globalTransforms[nodeIndex] = transform;

		// Recursively process child nodes
		const tinygltf::Node& node = model.nodes[nodeIndex];
		for (int childIndex : node.children) {
			computeGlobalNodeTransform(model, localTransforms, childIndex, transform, globalTransforms);
		}
	}

	int findKeyframeIndex(const std::vector<float>& times, float animationTime)
	{
		int left = 0;
		int right = times.size() - 1;

		while (left <= right) {
			int mid = (left + right) / 2;

			if (mid + 1 < times.size() && times[mid] <= animationTime && animationTime < times[mid + 1]) {
				return mid;
			}
			else if (times[mid] > animationTime) {
				right = mid - 1;
			}
			else { // animationTime >= times[mid + 1]
				left = mid + 1;
			}
		}

		// Target not found
		return times.size() - 2;
	}

	std::vector<AnimationObject> prepareAnimation(const tinygltf::Model &model)
	{
		std::vector<AnimationObject> animationObjects;
		for (const auto &anim : model.animations) {
			AnimationObject animationObject;

			for (const auto &sampler : anim.samplers) {
				SamplerObject samplerObject;

				const tinygltf::Accessor &inputAccessor = model.accessors[sampler.input];
				const tinygltf::BufferView &inputBufferView = model.bufferViews[inputAccessor.bufferView];
				const tinygltf::Buffer &inputBuffer = model.buffers[inputBufferView.buffer];

				assert(inputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				assert(inputAccessor.type == TINYGLTF_TYPE_SCALAR);

				// Input (time) values
				samplerObject.input.resize(inputAccessor.count);

				const unsigned char *inputPtr = &inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset];
				const float *inputBuf = reinterpret_cast<const float*>(inputPtr);

				// Read input (time) values
				int stride = inputAccessor.ByteStride(inputBufferView);
				for (size_t i = 0; i < inputAccessor.count; ++i) {
					samplerObject.input[i] = *reinterpret_cast<const float*>(inputPtr + i * stride);
				}

				const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
				const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
				const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

				assert(outputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
				const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

				int outputStride = outputAccessor.ByteStride(outputBufferView);

				// Output values
				samplerObject.output.resize(outputAccessor.count);

				for (size_t i = 0; i < outputAccessor.count; ++i) {

					if (outputAccessor.type == TINYGLTF_TYPE_VEC3) {
						memcpy(&samplerObject.output[i], outputPtr + i * 3 * sizeof(float), 3 * sizeof(float));
					} else if (outputAccessor.type == TINYGLTF_TYPE_VEC4) {
						memcpy(&samplerObject.output[i], outputPtr + i * 4 * sizeof(float), 4 * sizeof(float));
					} else {
						std::cout << "Unsupport accessor type ..." << std::endl;
					}

				}

				animationObject.samplers.push_back(samplerObject);
			}

			animationObjects.push_back(animationObject);
		}
		return animationObjects;
	}

	void updateAnimation(
		const tinygltf::Model &model,
		const tinygltf::Animation &anim,
		const AnimationObject &animationObject,
		float time,
		std::vector<glm::mat4> &nodeTransforms)
	{
		// There are many channels so we have to accumulate the transforms
		for (const auto &channel : anim.channels) {

			int targetNodeIndex = channel.target_node;
			const auto &sampler = anim.samplers[channel.sampler];

			// Access output (value) data for the channel
			const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
			const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
			const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

			// Calculate current animation time (wrap if necessary)
			const std::vector<float> &times = animationObject.samplers[channel.sampler].input;
			float animationTime = fmod(time, times.back());

			// ----------------------------------------------------------
			// TODO: Find a keyframe for getting animation data
			// ----------------------------------------------------------
			//int keyframeIndex = 0;
			int keyframeIndex = findKeyframeIndex(animationObject.samplers[channel.sampler].input, time);
			int nextKeyframeIndex = keyframeIndex + 1;

			// stop overflow
			if (nextKeyframeIndex >= animationObject.samplers[channel.sampler].input.size()) {
				nextKeyframeIndex = keyframeIndex;
			}

			const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
			const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

			// -----------------------------------------------------------
			// TODO: Add interpolation for smooth interpolation
			// -----------------------------------------------------------
			//interpolation:
			float t0 = animationObject.samplers[channel.sampler].input[keyframeIndex];
			float t1 = animationObject.samplers[channel.sampler].input[nextKeyframeIndex];
			float factor = (time - t0) / (t1 - t0); // Interpolation factor


			//attempt 2:
			if (channel.target_path == "translation") {
				glm::vec3 translation0, translation1;
				memcpy(&translation0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));
				memcpy(&translation1, outputPtr + nextKeyframeIndex * 3 * sizeof(float), 3 * sizeof(float));

				glm::vec3 translation = glm::mix(translation0, translation1, factor);
				nodeTransforms[targetNodeIndex] = glm::translate(glm::mat4(1.0f), translation);
			} else if (channel.target_path == "rotation") {
				glm::quat rotation0, rotation1;
				memcpy(&rotation0, outputPtr + keyframeIndex * 4 * sizeof(float), 4 * sizeof(float));
				memcpy(&rotation1, outputPtr + nextKeyframeIndex * 4 * sizeof(float), 4 * sizeof(float));

				glm::quat rotation = glm::slerp(rotation0, rotation1, factor);
				nodeTransforms[targetNodeIndex] *= glm::mat4_cast(rotation); // Multiply for cumulative rotation
			} else if (channel.target_path == "scale") {
				glm::vec3 scale0, scale1;
				memcpy(&scale0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));
				memcpy(&scale1, outputPtr + nextKeyframeIndex * 3 * sizeof(float), 3 * sizeof(float));

				glm::vec3 scale = glm::mix(scale0, scale1, factor);
				nodeTransforms[targetNodeIndex] = glm::scale(nodeTransforms[targetNodeIndex], scale); // Combine scale
			}

		}
	}


	void update(float time) {
		// TODO:
		//return;	 Do nothing for T-pose, comment out for animation

		if (model.animations.size() > 0) {
			const tinygltf::Animation &animation = model.animations[0];
			const AnimationObject &animationObject = animationObjects[0];

            const tinygltf::Skin &skin = model.skins[0];

			std::vector<glm::mat4> nodeTransforms(skin.joints.size());
			for (size_t i = 0; i < nodeTransforms.size(); ++i) {
				nodeTransforms[i] = glm::mat4(1.0);
			}

			updateAnimation(model, animation, animationObject, time, nodeTransforms);

			//debugging...see if transforms are correct
			/*
			for (size_t i = 0; i < nodeTransforms.size(); ++i) {
				std::cout << "Node " << i << ": " << glm::to_string(nodeTransforms[i]) << std::endl;
			}
			*/

			// ----------------------------------------------
            // TODO: Recompute global transforms at each node
			// using the updated node transforms above
            // ----------------------------------------------
			glm::mat4 parentTransform(1.0f); // Start with the identity matrix for the root
			computeGlobalNodeTransform(model, nodeTransforms, skin.joints[0], parentTransform, globalTransforms);

		}
	}

	bool loadModel(tinygltf::Model &model, const char *filename) {
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
		if (!warn.empty()) {
			std::cout << "WARN: " << warn << std::endl;
		}

		if (!err.empty()) {
			std::cout << "ERR: " << err << std::endl;
		}

		if (!res)
			std::cout << "Failed to load glTF: " << filename << std::endl;
		else
			std::cout << "Loaded glTF: " << filename << std::endl;

		return res;
	}

	void initialize() {
		// Modify your path if needed
		if (!loadModel(model, "../lab2/model/bot/bot.gltf")) {
			return;
		}

		// Prepare animation data
		animationObjects = prepareAnimation(model);

		// Just take the first skin/skeleton model
        const tinygltf::Skin &skin = model.skins[0];

        // Compute local transforms at each node
        int rootNodeIndex = skin.joints[0];
        std::vector<glm::mat4> localNodeTransforms(skin.joints.size());
        computeLocalNodeTransform(model, rootNodeIndex, localNodeTransforms);

        // Compute global transforms at each node
        glm::mat4 parentTransform(1.0f);
        std::vector<glm::mat4> globalNodeTransforms(skin.joints.size());
        computeGlobalNodeTransform(model, localNodeTransforms, rootNodeIndex, parentTransform, globalNodeTransforms);

        globalTransforms = globalNodeTransforms;
        skeleton.initialize();
	}

	void render(glm::mat4 cameraMatrix) {
		//insert here:



        const tinygltf::Skin &skin = model.skins[0];
        skeleton.renderSkeleton(model, skin, globalTransforms, cameraMatrix);
		// Reset OpenGL state after skeleton rendering
		glBindVertexArray(0);                // Unbind VAO
		glUseProgram(0);                     // Reset active shader program
		glEnable(GL_DEPTH_TEST);             // Ensure depth testing is enabled
		glCullFace(GL_BACK);                 // Ensure back-face culli
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

	}




	void cleanup() {

	}
};





int main(void) {
    // Initialise GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    //GLFWwindow* window = glfwCreateWindow(2048, 1536, "Lab 2", NULL, NULL);
	GLFWwindow* window = glfwCreateWindow(512, 384, "Project", NULL, NULL);

	if (window == NULL) {
        std::cerr << "Failed to open a GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(window, key_callback);

    // Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        std::cerr << "Failed to initialize OpenGL context." << std::endl;
        return -1;
    }

    // Background
    glClearColor(0.2f, 0.2f, 0.25f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    std::vector<Building> buildings; // For storing  buildings
	std::vector<Road> roads;  // Use separate vector for roads
	skyBox skybox;	//introduce skybox (cave setting)
	//Ground g;
	// Our 3D character
	MyBot bot;
	bot.initialize();
    int gridSize = 6; // Number of buildings along x/y axes.
    float baseSpace = 100.0f; // Base space between buildings
	float skyboxMargin = 100.0f; //space between outer building edge and skybox

	//lookat.x = eye_center.x + sin(viewAzimuth);
	//lookat.z = eye_center.z + cos(viewAzimuth);
	//lookat.y = eye_center.y;

	// Time and frame rate tracking
	static double lastTime = glfwGetTime();
	float time = 0.0f;			// Animation time
	float fTime = 0.0f;			// Time for measuring fps
	unsigned long frames = 0;
	lookat.x = 100.0f;
	lookat.z = 0.0f;
	lookat.y = 10.0f;

    std::srand(static_cast<unsigned>(std::time(nullptr))); // Seed for better randomness.

    // Create roads
    for (int i = 0; i < gridSize; i++) {
        Road roadH;
        glm::vec3 positionH(i * baseSpace, 0, gridSize / 2 * baseSpace);
        glm::vec3 scaleH(baseSpace * gridSize, 1.0f, 10.0f);
        roadH.initialize(positionH, scaleH, "../lab2/orange.jpg"); // Specify road texture
    	roads.push_back(roadH);

        Road roadV;
        glm::vec3 positionV(gridSize / 2 * baseSpace, 0, i * baseSpace);
        glm::vec3 scaleV(10.0f, 1.0f, baseSpace * gridSize);
        roadV.initialize(positionV, scaleV, "../lab2/orange.jpg"); // Specify road texture
    	roads.push_back(roadV);
    }

    // Create buildings
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (i == gridSize / 2 || j == gridSize / 2) continue; // Skip the road lines

            Building b;
            float spaceVariationX = static_cast<float>(rand() % 20 - 10); // Random variation
            float spaceVariationY = static_cast<float>(rand() % 20 - 10);
            float dynamicSpaceX = baseSpace + spaceVariationX;
            float dynamicSpaceY = baseSpace + spaceVariationY;
            glm::vec3 position(i * dynamicSpaceX, 0, j * dynamicSpaceY);
            float width = 14.0f + static_cast<float>(rand() % 10);
            float height = 40.0f + static_cast<float>(rand() % 120);
            float depth = 14.0f + static_cast<float>(rand() % 10);
            glm::vec3 size(width, height, depth);
            b.initialize(position, size); // Specify building texture
            buildings.push_back(b);
        }
    }

	// Calculate skybox size and position
	float citySize = gridSize * baseSpace;             // Size of the city
	float skyboxSize = citySize + skyboxMargin * 2.0f; // Skybox size with margin
	glm::vec3 skyboxPosition(citySize / 2.0f, 0, citySize / 2.0f);
	glm::vec3 skyboxScale(skyboxSize, skyboxSize, skyboxSize);
	skybox.initialize(skyboxPosition, skyboxScale);


	eye_center.y = viewDistance * cos(viewPolar);
	eye_center.x = viewDistance * cos(viewAzimuth);
	eye_center.z = viewDistance * sin(viewAzimuth);

	//glm::mat4 viewMatrix, projectionMatrix;
	glm::mat4 viewMatrix, projectionMatrix, light_view_matrix, light_projection_matrix, LVP;
	glm::float32 FoV = 45;
	glm::float32 zNear = 0.1f;
	glm::float32 zFar = 30000.0f;//1000
	projectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, zNear, zFar);
	light_projection_matrix = glm::perspective(glm::radians(depthFoV), (float)shadowMapWidth / shadowMapHeight, depthNear, depthFar);

    // Rendering loop
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//implement lighting
    	light_view_matrix = glm::lookAt(lightPosition, lightPosition * glm::vec3(0.0f,-1.0f,0.0f), lightUp);//
    	light_projection_matrix = glm::perspective(glm::radians(depthFoV), (float)shadowMapWidth / shadowMapHeight, depthNear, depthFar);//
    	LVP = light_projection_matrix * light_view_matrix;//
    	glUniformMatrix4fv(glGetUniformLocation(skybox.programID, "LVP"), 1, GL_FALSE, &LVP[0][0]);//

    	// Update states for animation
    	double currentTime = glfwGetTime();
    	float deltaTime = float(currentTime - lastTime);
    	lastTime = currentTime;

    	if (playAnimation) {
    		time += deltaTime * playbackSpeed;
    		bot.update(time);
    	}

        glm::mat4 viewMatrix = glm::lookAt(eye_center, lookat, up);
        glm::mat4 vp = projectionMatrix * viewMatrix;
    	//resetEye(gridSize);  // Adjust camera position
    	//g.render(vp, eye_center, gridSize);
    	// Render the skybox

    	glUseProgram(skybox.programID);
    	//glBindVertexArray(0);
    	skybox.render(vp, eye_center);
    	for (auto &building : buildings) {
    		building.render(vp);
    	}
    	for (auto &road : roads) {
    		road.render(vp);
    	}
    	//animation
    	//bot.render(vp);
    	//restore previous state after bot (inserted trying to debug why everything wont show togetehr)


    	// FPS tracking
    	// Count number of frames over a few seconds and take average
    	frames++;
    	fTime += deltaTime;
    	if (fTime > 2.0f) {
    		float fps = frames / fTime;
    		frames = 0;
    		fTime = 0;

    		std::stringstream stream;
    		stream << std::fixed << std::setprecision(2) << "Final Scene | Frames per second (FPS): " << fps;
    		glfwSetWindowTitle(window, stream.str().c_str());
    	}

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(window));

    // Cleanup
    for (auto &building : buildings) {
        building.cleanup();
    }
	//g.cleanup();
	bot.cleanup();
	skybox.cleanup();
    glfwTerminate();
    return 0;
}



// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		viewAzimuth = 0.f;
		viewPolar = 0.f;
		eye_center.y = viewDistance * cos(viewPolar);
		eye_center.x = viewDistance * cos(viewAzimuth);
		eye_center.z = viewDistance * sin(viewAzimuth);
		std::cout << "Reset." << std::endl;
	}

	//stop animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		playAnimation = !playAnimation;
	}

	if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		//viewPolar -= 0.1f;
		//eye_center.y = viewDistance * cos(viewPolar);
		lookat.y += 25.0f;
	}

	if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		//viewPolar += 0.1f;
		//eye_center.y = viewDistance * cos(viewPolar);
		lookat.y -= 25.0f;
	}

	if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		lookat.x += 25.0f;
		//eye_center.x = viewDistance * cos(viewAzimuth);
		//eye_center.z = viewDistance * sin(viewAzimuth);
		//lookat.x = eye_center.x + sin(viewAzimuth);
		//lookat.z = eye_center.z + cos(viewAzimuth);
	}

	if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		lookat.x -= 25.0f;
		//eye_center.x = viewDistance * cos(viewAzimuth);
		//eye_center.z = viewDistance * sin(viewAzimuth);
		//lookat.x = eye_center.x + sin(viewAzimuth);
		//lookat.z = eye_center.z + cos(viewAzimuth);
	}

	//move forwards: w
	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		eye_center.x += 10.2f * sin(viewAzimuth);
		eye_center.z += 10.2f * cos(viewAzimuth);
		//lookat.x += eye_center.x;
		//lookat.z += eye_center.z;
	}

	//move backwards: s
	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		eye_center.x -= 10.2f * sin(viewAzimuth);
		eye_center.z -= 10.2f * cos(viewAzimuth);
		//lookat.x = eye_center.x + sin(viewAzimuth);
		//lookat.z = eye_center.z + cos(viewAzimuth);
	}

	//move left: a
	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		eye_center.x -= 10.2f * cos(viewAzimuth + 3.14f);
		eye_center.z += 10.2f * sin(viewAzimuth + 3.14f);
		//lookat.x = eye_center.x + sin(viewAzimuth);
		//lookat.z = eye_center.z + cos(viewAzimuth);
	}

	//move back: d
	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		eye_center.x += 10.2f * cos(viewAzimuth - 3.14f);
		eye_center.z -= 10.2f * sin(viewAzimuth - 3.14f);
		//lookat.x = eye_center.x + sin(viewAzimuth);
		//lookat.z = eye_center.z + cos(viewAzimuth);
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

