#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <iostream>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include <iostream>
#include <sstream>
#include <string.h>

using namespace rapidxml;
using namespace std;

float windowWidth,windowHeight,camPosx,camPosy,camPosz,camLookx,camLooky,camLookz;
float camUpx,camUpy,camUpz,camProjfov = 60.0f,camProjnear = 1.0f,camProjfar = 1000.0f;
stringstream models;


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);

	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(camProjfov, ratio, camProjnear, camProjfar);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


void DrawModels(){

	string model;
	string pointx;
	string pointy;
	string pointz;


	// iterates through all the models gotten from the XML file 
	// and draws triangles with the points written in the .3d file
	   
	while (models >> model){

		ifstream modelfile("../../output/" + model);

		if(!modelfile){
        std::cout <<"!Error opening " << model <<"!\n";
        return ;
    	}


		glBegin(GL_TRIANGLES);
			glColor3f(1.0f, 1.0f, 1.0f);
			while (modelfile >> pointx){
				modelfile >> pointy;
				modelfile >> pointz;
				glVertex3f(stof(pointx),stof(pointy),stof(pointz));
			}
		glEnd();

		modelfile.close();

	}
}


void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(camPosx,camPosy,camPosz, 
		      camLookx,camLooky,camLookz,
			  camUpx,camUpy,camUpz);

	glPolygonMode(GL_FRONT,GL_LINE);

	// axis lines
	glBegin(GL_LINES);
		// X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f( 100.0f, 0.0f, 0.0f);
		// Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
		// Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

	DrawModels();

	// End of frame
	glutSwapBuffers();
}

void XMLreading(char **fxml){

	file<> file(*fxml);
	xml_document<> doc;
	doc.parse<0>(file.data());

	windowWidth = stof(doc.first_node("world")->first_node("window")->first_attribute("width")->value());
	windowHeight = stof(doc.first_node("world")->first_node("window")->first_attribute("height")->value());
	camPosx = stof(doc.first_node("world")->first_node("camera")->first_node("position")->first_attribute("x")->value());
	camPosy = stof(doc.first_node("world")->first_node("camera")->first_node("position")->first_attribute("y")->value());
	camPosz = stof(doc.first_node("world")->first_node("camera")->first_node("position")->first_attribute("z")->value());
	camLookx = stof(doc.first_node("world")->first_node("camera")->first_node("lookAt")->first_attribute("x")->value());
	camLooky = stof(doc.first_node("world")->first_node("camera")->first_node("lookAt")->first_attribute("y")->value());
	camLookz = stof(doc.first_node("world")->first_node("camera")->first_node("lookAt")->first_attribute("z")->value());
	camUpx = stof(doc.first_node("world")->first_node("camera")->first_node("up")->first_attribute("x")->value());
	camUpy = stof(doc.first_node("world")->first_node("camera")->first_node("up")->first_attribute("y")->value());
	camUpz = stof(doc.first_node("world")->first_node("camera")->first_node("up")->first_attribute("z")->value());
	camProjfov = stof(doc.first_node("world")->first_node("camera")->first_node("projection")->first_attribute("fov")->value());
	camProjnear = stof(doc.first_node("world")->first_node("camera")->first_node("projection")->first_attribute("near")->value());
	camProjfar = stof(doc.first_node("world")->first_node("camera")->first_node("projection")->first_attribute("far")->value());

	for (xml_node<> * modelaux = doc.first_node("world")->first_node("group")->first_node("models")->first_node("model");
	modelaux;
	modelaux = modelaux->next_sibling()){
		models << modelaux->first_attribute("file")->value() << '\n';
	}

}



int main(int argc, char **argv) {

// reads the XML file from the input
	XMLreading(&argv[1]);

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(windowWidth,windowHeight);
	glutCreateWindow("Model");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
