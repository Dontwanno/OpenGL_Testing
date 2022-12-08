// Local Headers
#include "glitter.hpp"

// System Headers
#include <GLFW/glfw3.h>
#include <glad/glad.h>
//#include <GL/freeglut.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include "btBulletDynamicsCommon.h"
#include <cstdio>
#include <learnopengl/camera.h>

// learnopengl includes
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/sphere.h>
//include freeglut
#include <GL/freeglut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1440;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

    // Initialize GLUT
    int argc = 0;
    char* argv[] = { NULL };
    glutInit(&argc, argv);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("colors.vs", "colors.fs");
    Shader lightCubeShader("light_cube.vs", "light_cube.fs");
    Shader floorShader("floor.vs", "floor.fs");

    // vertices for a black and white checkered floor with normals and colors centered at a corner
    // -----------------------------
    float floorVertices[] = {
            // positions          // normals           // colors
            1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,
            -1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,
            -1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,

            1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,
            1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,
            -1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f
    };

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    unsigned int floorVBO, floorVAO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    Sphere sphere = Sphere(1.0f, 50);


    float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Model ourModel("../../Glitter/Resources/Opzetstuk.STL");

    btVector3 inertia = ourModel.rigidBody->getLocalInertia();
    ourModel.rigidBody->getOrientation();
    ourModel.rigidBody->getCenterOfMassPosition();

//     Print the x, y, and z components of the tensor
    std::cout << "Inertia tensor: (" << inertia.getX() << ", " << inertia.getY() << ", " << inertia.getZ() << ")" << std::endl;

    int size = 25;

    // bullet physics setup


// Create the broadphase object
    btBroadphaseInterface *broadphase = new btDbvtBroadphase();

// Create the collision configuration
    btDefaultCollisionConfiguration *collisionConfiguration =
            new btDefaultCollisionConfiguration();

// Create the collision dispatcher
    btCollisionDispatcher *dispatcher =
            new btCollisionDispatcher(collisionConfiguration);

// Create the constraint solver
    btSequentialImpulseConstraintSolver *solver =
            new btSequentialImpulseConstraintSolver();

// Create the dynamics world
    btDiscreteDynamicsWorld *dynamicsWorld = new btDiscreteDynamicsWorld(
            dispatcher, broadphase, solver, collisionConfiguration);

// Set the gravity of the world
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

// Create a sphere shape
    btCollisionShape *sphereShape = new btSphereShape(1);
// Create the initial position and orientation of the sphere
    btTransform sphereTransform;
    sphereTransform.setIdentity();
    sphereTransform.setOrigin(btVector3(0, 10, 0));
// Create the motion state for the sphere
    btDefaultMotionState *sphereMotionState = new btDefaultMotionState(sphereTransform);
// Create the rigid body construction info for the sphere
    btScalar mass = 1;
    btVector3 sphereInertia(0, 0, 0);
    sphereShape->calculateLocalInertia(mass, sphereInertia);
    btRigidBody::btRigidBodyConstructionInfo sphereRigidBodyCI(mass, sphereMotionState, sphereShape, sphereInertia);
// Create the rigid body for the sphere
    btRigidBody* sphereRigidBody = new btRigidBody(sphereRigidBodyCI);

// Create the plane shape
    btStaticPlaneShape* planeShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
// Create the initial position and orientation of the plane
    btTransform planeTransform;
    planeTransform.setIdentity();
    planeTransform.setOrigin(btVector3(0, -1, 0));
// Create the motion state for the plane
    btDefaultMotionState* planeMotionState = new btDefaultMotionState(planeTransform);
// Create the rigid body construction info for the plane
    btRigidBody::btRigidBodyConstructionInfo planeRigidBodyCI(0, planeMotionState, planeShape, btVector3(0, 0, 0));
// Create the plane rigid body
    btRigidBody* planeRigidBody = new btRigidBody(planeRigidBodyCI);

    // Create the box shape
    btCollisionShape *boxShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
// Create the initial position and orientation of the box
    btTransform boxTransform;
    boxTransform.setIdentity();
    boxTransform.setOrigin(btVector3(-.5, 5, 0));
// Create the motion state for the box
    btDefaultMotionState *boxMotionState = new btDefaultMotionState(boxTransform);
// Create the rigid body construction info for the box
    btScalar mass2 = 1;
    btVector3 boxInertia(0, 0, 0);
    boxShape->calculateLocalInertia(mass2, boxInertia);
    btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass2, boxMotionState, boxShape, boxInertia);
// Create the rigid body for the box
    btRigidBody* boxRigidBody = new btRigidBody(boxRigidBodyCI);


// Add the plane to the dynamics world
    dynamicsWorld->addRigidBody(planeRigidBody);
// Add the sphere to the dynamics world
    dynamicsWorld->addRigidBody(sphereRigidBody);
// Add the box to the dynamics world
    dynamicsWorld->addRigidBody(boxRigidBody);
    // Add the model to the dynamics world

    // Create a transform for the rigid body
    btTransform transform;
// Set the position of the rigid body
    transform.setOrigin(btVector3(0, 15, 0));
// Set the orientation of the rigid body
    transform.setRotation(btQuaternion(0, 0, 0, 1));
// Apply the transform to the rigid body
    ourModel.rigidBody->setWorldTransform(transform);
    dynamicsWorld->addRigidBody(ourModel.rigidBody);





    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // step the simulation
        dynamicsWorld->stepSimulation(1.f / 60.f, 10);

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightPos = glm::vec3(cos(glfwGetTime()) * 2.0f, 0.0f, sin(glfwGetTime()) * 2.0f);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // get the box world transform
        btTransform boxWorldTrans = boxRigidBody->getWorldTransform();
        // get the box position
        btVector3 boxPos = boxWorldTrans.getOrigin();
        // get the box rotation
        btQuaternion boxRot = boxWorldTrans.getRotation();
        // convert the box rotation to a glm quaternion
        glm::quat boxQuat(boxRot.getW(), boxRot.getX(), boxRot.getY(), boxRot.getZ());
        // convert the box rotation to a glm mat4
        glm::mat4 boxRotMat = glm::mat4_cast(boxQuat);
        // create the box model matrix
        glm::mat4 boxModel = glm::mat4(1.0f);
        boxModel = glm::translate(boxModel, glm::vec3(boxPos.getX(), boxPos.getY(), boxPos.getZ()));
        boxModel = boxModel * boxRotMat;
        lightingShader.setMat4("model", boxModel);


        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // get the model world transform
        btTransform modelWorldTrans = ourModel.rigidBody->getWorldTransform();
        // get the model position
        btVector3 modelPos = modelWorldTrans.getOrigin();
        // get the model rotation
        btQuaternion modelRot = modelWorldTrans.getRotation();
        // convert the model rotation to a glm quaternion
        glm::quat modelQuat(modelRot.getW(), modelRot.getX(), modelRot.getY(), modelRot.getZ());
        // convert the model rotation to a glm mat4
        glm::mat4 modelRotMat = glm::mat4_cast(modelQuat);
        // create the model model matrix
        glm::mat4 modelModel = glm::mat4(1.0f);
        // set the model scale  (this is a hack to make the model smaller)
        modelModel = glm::scale(modelModel, glm::vec3(0.1f, 0.1f, 0.1f));
        modelModel = glm::translate(modelModel, glm::vec3(modelPos.getX(), modelPos.getY(), modelPos.getZ()));
        modelModel = modelModel * modelRotMat;
        lightingShader.setMat4("model", modelModel);
        lightingShader.setVec3("objectColor", 0.5f, 0.5f, 0.0f);
        lightingShader.setMat4("model", modelModel);
        ourModel.Draw(lightingShader);


        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // also draw the floor
        glBindVertexArray(floorVAO);
        floorShader.use();
        floorShader.setMat4("projection", projection);
        floorShader.setMat4("view", view);
        floorShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        floorShader.setVec3("lightPos", lightPos);
        floorShader.setVec3("viewPos", camera.Position);

        for (int i = -size; i < size; i++) {
            for (int j = -size; j < size; j++) {
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(i, -1.0f, j));
                model = glm::scale(model, glm::vec3(0.5f));
                floorShader.setMat4("model", model);
                if ((i+j) % 2 == 0) {
                    floorShader.setVec3("objectColor", 0.9f, 0.9f, 0.9f);
                } else {
                    floorShader.setVec3("objectColor", 0.2f, 0.2f, 0.2f);
                }
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        // get the rigid body's world transform
        btTransform transform = sphereRigidBody->getWorldTransform();
        btVector3 origin = transform.getOrigin();


        lightingShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(origin.getX(), origin.getY(), origin.getZ()));
        model = glm::scale(model, glm::vec3(1.0f));
        lightingShader.setMat4("model", model);

        sphere.Draw();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

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
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
