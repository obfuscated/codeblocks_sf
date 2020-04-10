#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

static void error_callback(int error, const char* description)
{

    std::cerr << description << std::endl;
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

struct point
{
    point()
    {
        x = 0.0;
        y = 0.0;
    }

    point(float px, float py)
    {
        x = px;
        y = py;
    }

    float x;
    float y;
};

struct triangle
{
    triangle(point t, point bl, point br )
    {
        top = t;
        bot_left = bl;
        bot_right = br;
    }

    point top;
    point bot_left;
    point bot_right;
};

// Generate the Sierpinski triangle with a given level
void generateTriangle(std::vector<triangle>& tr, int level, point top_point, point left_point, point right_point)
{
    if (level == 0)
    {

        tr.push_back(triangle(top_point, left_point, right_point));
    }
    else
    {
        // Find the edge midpoints.
        point left_mid = point(
            (top_point.x + left_point.x) / 2.0,
            (top_point.y + left_point.y) / 2.0);
        point right_mid = point(
            (top_point.x + right_point.x) / 2.0,
            (top_point.y + right_point.y) / 2.0);
        point bottom_mid = point(
            (left_point.x + right_point.x) / 2.0,
            (left_point.y + right_point.y) / 2.0);


        // Recursively generate smaller triangles.
        generateTriangle(tr, level - 1, top_point, left_mid, right_mid);
        generateTriangle(tr, level - 1, left_mid, left_point, bottom_mid);
        generateTriangle(tr, level - 1, right_mid, bottom_mid, right_point);
    }
}

int main(void)
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return -1;


    window = glfwCreateWindow(640, 480, "Sierpinski example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);


    std::vector<triangle> image;
    generateTriangle(image, 5, point(0, 1), point(-0.8, -0.8), point(0.8, -0.8));
    std::cout << "Draw " << image.size() << " triangles..." << std::endl;
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();


        glOrtho(-ratio, ratio, -1.0, 1.0, 1.0, -1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glRotatef(glfwGetTime() * 50.0, 0.0 , 0.0 , 1.0);
        glBegin(GL_TRIANGLES);
        // go trought all triangles and draw them
        for(const triangle& item : image)
        {
            glColor3f(1.0, 0.0, 0.0);
            glVertex3f(item.top.x, item.top.y, 0.0);
            glColor3f(0.0, 1.0, 0.0);
            glVertex3f(item.bot_left.x, item.bot_left.y, 0.0);
            glColor3f(0.0, 0.0, 1.0);
            glVertex3f(item.bot_right.x, item.bot_right.y, 0.0);
        }
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
     glfwTerminate();
     return 0;
 }
