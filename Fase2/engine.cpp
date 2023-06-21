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
stringstream actions;
int alfaM = 45, betaM = 45, r;
int startX, startY, tracking = 0, target = 0;

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


void DrawModels(string model){

	string pointx;
	string pointy;
	string pointz;

	// iterates through all the models gotten from the XML file 
	// and draws triangles with the points written in the .3d file
	   
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

int len(string str)  
{  
    int length = 0;  
    for (int i = 0; str[i] != '\0'; i++)  
    {  
        length++;  
          
    }  
    return length;     
}  


void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(camPosx,camPosy,camPosz, 
		      camLookx,camLooky,camLookz,
			  camUpx,camUpy,camUpz);

	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	/* // axis lines
	glBegin(GL_LINES);
		// X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-1000.0f, 0.0f, 0.0f);
		glVertex3f( 1000.0f, 0.0f, 0.0f);
		// Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -1000.0f, 0.0f);
		glVertex3f(0.0f, 1000.0f, 0.0f);
		// Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -1000.0f);
		glVertex3f(0.0f, 0.0f, 1000.0f);
	glEnd(); */


	glPushMatrix();

	switch(target){
		case(1):
			glTranslatef(-300,0,0);
			break;
		case(2):
			glTranslatef(-400,0,0);
			break;
		case(3):
			glTranslatef(-500,0,0);
			break;
		case(4):
			glTranslatef(-600,0,0);
			break;
		case(5):
			glTranslatef(-1700,0,0);
			break;
		case(6):
			glTranslatef(-3050,0,0);
			break;
		case(7):
			glTranslatef(-5950,0,0);
			break;
		case(8):
			glTranslatef(-9150,0,0);
			break;
	}


	string action;

	actions.clear();
	actions.seekg(0);

	while (actions >> action){

		string strings[6];

    	int curr = 0, i = 0;  
    	int start = 0, end = 0; 

    	while (i <= len(action))  
    	{  
        	if (action[i] == '|' || i == len(action))  
        	{  
            	end = i;  
            	string subStr = "";
            	subStr.append(action, start, end - start);  
            	strings[curr] = subStr;  
            	curr += 1;  
            	start = end + 1;  
        	}  
        	i++;  
        }

		if (strings[0] == "group"){
			if (strings[1] =="true"){
				glPushMatrix();
			}
			else {
				glPopMatrix();
			}
		}
		else if (strings[0] == "translate"){
			glTranslatef(stof(strings[1]),stof(strings[2]),stof(strings[3]));
		}
		else if (strings[0] =="rotate"){
			glRotatef(stof(strings[1]),stof(strings[2]),stof(strings[3]),stof(strings[4]));
		}
		else if (strings[0] =="scale"){
			glScalef(stof(strings[1]),stof(strings[2]),stof(strings[3]));
		}
		else {
			DrawModels(strings[1]);
		}
	}
	glPopMatrix();


	// End of frame
	glutSwapBuffers();
}

void groupReader(xml_node<> * group){

	actions << "group|true ";

	xml_node<> * transformaux = group->first_node("transform")->first_node();
	while (transformaux){

		string nome = transformaux->name();

		if (nome =="translate"){
			float x = stof(transformaux->first_attribute("x")->value());
			float y = stof(transformaux->first_attribute("y")->value());
			float z = stof(transformaux->first_attribute("z")->value());

			actions << "translate|" << x << "|" << y << "|" << z << " ";
		}
		else if (nome =="rotate"){
			float angle = stof(transformaux->first_attribute("angle")->value());
			float x = stof(transformaux->first_attribute("x")->value());
			float y = stof(transformaux->first_attribute("y")->value());
			float z = stof(transformaux->first_attribute("z")->value());

			actions << "rotate|" << angle << "|" << x << "|" << y << "|" << z << " ";
		}
		else if (nome =="scale"){
			float x = stof(transformaux->first_attribute("x")->value());
			float y = stof(transformaux->first_attribute("y")->value());
			float z = stof(transformaux->first_attribute("z")->value());

			actions << "scale|" << x << "|" << y << "|" << z << " ";
		}
			
		transformaux = transformaux->next_sibling();
	}

	if(xml_node<> * model1 = group->first_node("models")){
		for (xml_node<> * modelaux = model1->first_node("model");
		modelaux;
		modelaux = modelaux->next_sibling()){

			actions << "model|" << modelaux->first_attribute("file")->value() << " ";
		}
	}

	for(xml_node<> * groupaux = group->first_node("group"); groupaux; groupaux = groupaux->next_sibling()){
		groupReader(groupaux);
	}

	actions << "group|false ";	

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

	for (xml_node<> * groupaux = doc.first_node("world")->first_node("group");
		groupaux;
		groupaux = groupaux->next_sibling()){
		groupReader(groupaux);
	
	}

}

void processKeys(unsigned char key, int xx, int yy) {
	switch(key) {
		case 'w':
			camPosz += 100;
			camLookz += 100;
			break;
		case 's':
			camPosz -= 100;
			camLookz -= 100;
			break;
		case 'd':
			camPosx += 100;
			camLookx += 100;
			break;
		case 'a':
			camPosx -= 100;
			camLookx -= 100;
			break;
		case 'c':
			camPosx = 300;
			camPosy = 300;
			camPosz = 300;
			camLookx = 0;
			camLookz = 0;
			r = sqrt(pow(camPosx,2) + pow(camPosy,2) + pow(camPosz,2));
			betaM = 45;
			alfaM = 45;
			break;
		case '0':
			target = 0;
			break;
		case '1':
			target = 1;
			break;
		case '2':
			target = 2;
			break;
		case '3':
			target = 3;
			break;
		case '4':
			target = 4;
			break;
		case '5':
			target = 5;
			break;
		case '6':
			target = 6;
			break;
		case '7':
			target = 7;
			break;
		case '8':
			target = 8;
			break;
	}
}


void processMouseButtons(int button, int state, int xx, int yy) {
	
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alfaM += (xx - startX);
			betaM += (yy - startY);
		}
		else if (tracking == 2) {
			
			r -= yy - startY;
			if (r < 3)
				r = 3.0;
		}
		tracking = 0;
	}
}

void processMouseMotion(int xx, int yy) {

	int deltaX, deltaY;
	int alfaAux, betaAux;
	int rAux;

	if (!tracking)
		return;

	deltaX = xx - startX;
	deltaY = yy - startY;

	if (tracking == 1) {

		alfaAux = alfaM + deltaX;
		betaAux = betaM + deltaY;

		if (betaAux > 85.0)
			betaAux = 85.0;
		else if (betaAux < -85.0)
			betaAux = -85.0;

		rAux = r;
	}
	else if (tracking == 2) {

		alfaAux = alfaM;
		betaAux = betaM;
		rAux = r - deltaY;
		if (rAux < 3)
			rAux = 3;
	}
	if (camLookx == 0 && camLooky == 0 && camLookz == 0){
		camPosx = rAux * sin(alfaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
		camPosz = rAux * cos(alfaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
		camPosy = rAux * 							    sin(betaAux * 3.14 / 180.0);
	}
}

void info(){
	cout << "\n<<<<<<<<<<<<<<<<<<<<<COMMAND INSTRUCTIONS>>>>>>>>>>>>>>>>>>>>>\n\n";
	cout << "W,A,S,D <---------------------------------- MOVE THE CAMERA FREELY\n";
	cout << "C <---------------------------------------- RESET THE CAMERA\n\n";
	cout << "!!!MOUSE COMMANDS ONLY WORK WHILE THE CAMERA IS LOCKED AT SOME PLANET/SUN!!!\n\n";
	cout << "MOUSE LEFT CLICK <------------------------- ROTATE THE CAMERA AROUND ORIGIN\n";
	cout << "MOUSE RIGHT CLICK <------------------------ ZOOM TOWARDS ORIGIN\n\n";
	cout << "NUMBER KEYS MAKE THE CAMERA FOCUS ON THE PLANETS\n\n";
	cout << "0 - SUN | 1 - MERCURY | 2 - VENUS | 3 - EARTH | 4 - MARS\n";
	cout << "5 - JUPYTER | 6 - SATURN | 7 - URANUS | 8 - NEPTUNE\n";

}


int main(int argc, char **argv) {

// reads the XML file from the input
	XMLreading(&argv[1]);

	r = sqrt(pow(camPosx,2) + pow(camPosy,2) + pow(camPosz,2));

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(windowWidth,windowHeight);
	glutCreateWindow("Model");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	info();
// enter GLUT's main cycle
	glutMainLoop();

	
	
	return 1;
}
