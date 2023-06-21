#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
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
float centerCoords[3] = {0,0,0};
stringstream models;
stringstream actions;
int alfaM = 45, betaM = 45, r;
int startX, startY, tracking = 0, target = 0;
int iterarbuff = 0,timebase = 0, frame = 0,w,h;
int nP = 0, placement = 0, nmodels = 0;
GLuint nvertices;
bool showfps = false;

void changeSize(int ww, int hh) {

	w = ww;
	h = hh;

	if(h == 0)
		h = 1;

	float ratio = w * 1.0 / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glViewport(0, 0, w, h);
	gluPerspective(camProjfov, ratio, camProjnear, camProjfar);
	glMatrixMode(GL_MODELVIEW);
}

void prepareArray(string model, GLuint *buffer){

	float *modelArray;
	int size;
	string coord;
	nvertices = 0;

	ifstream modelfile("../../3d/" + model);

	if(!modelfile){
    	std::cout <<"!Error opening " << model <<"!\n";
    	return ;
    }

	// Calcula o tamanho do modelfile para alocar espaço suficiente no modelArray
	modelfile.seekg(0, ios::end);
	
	size = modelfile.tellg();

	modelArray = (float *)malloc(sizeof(float) * size);


	// São alocados os valores de modelfile no modelArray

	modelfile.clear();
	modelfile.seekg(0);

	while (modelfile >> coord){
		modelArray[nvertices] = stof(coord);
		nvertices++;
	}

	modelfile.close();

	// Buffer guarda os pontos que serão desenhados

	glBindBuffer(GL_ARRAY_BUFFER,buffer[iterarbuff]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(float) * nvertices, modelArray, GL_STATIC_DRAW);

	free(modelArray);
}


void DrawModels(GLuint *buffer){

	// Desenha os vértices guardados no buffer

	glBindBuffer(GL_ARRAY_BUFFER,buffer[iterarbuff]);
	glVertexPointer(3,GL_FLOAT,0,0);	
	glDrawArrays(GL_TRIANGLES, 0, nvertices/3);

	nvertices = 0;
	iterarbuff++;
	placement++;
}

void buildRotMatrix(float *x, float *y, float *z, float *m) {

	m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
	m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
	m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}


void cross(float *a, float *b, float *res) {

	res[0] = a[1]*b[2] - a[2]*b[1];
	res[1] = a[2]*b[0] - a[0]*b[2];
	res[2] = a[0]*b[1] - a[1]*b[0];
}


void normalize(float *a) {

	float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0]/l;
	a[1] = a[1]/l;
	a[2] = a[2]/l;
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

void multMatrixVector(float *m, float *v, float *res) {

	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}

}

void getCatmullRomPoint(float t, vector<float> p0, vector<float> p1, vector<float> p2, vector<float> p3, float *pos, float *deriv) {

	float m[4][4] = {	{-0.5f,  1.5f, -1.5f,  0.5f},
						{ 1.0f, -2.5f,  2.0f, -0.5f},
						{-0.5f,  0.0f,  0.5f,  0.0f},
						{ 0.0f,  1.0f,  0.0f,  0.0f}};

	float T[4] = {pow(t,3.0f), pow(t,2.0f), t, 1};
	float T1[4] = {3* pow(t,2.0f), 2*t, 1, 0};

	float A[4];

	for(int i = 0; i < 3; i++){
		float A[4];
		float temp[] = {p0[i], p1[i], p2[i], p3[i]};
		multMatrixVector((float *) m, temp, A);

		pos[i] = pow(t, 3) * A[0] + pow(t, 2) * A[1] + t * A[2] + A[3];

		deriv[i] = 3 * pow(t, 2) * A[0] + 2 * t * A[1] + A[2];
	}

}


void getGlobalCatmullRomPoint(float gt, float *pos, float *deriv, vector<vector<float>> CRpoints) {
	
	float t = gt * nP;
	int index = floor(t);
	t = t - index;

	int indices[4]; 
	indices[0] = (index + nP-1)%nP;	
	indices[1] = (indices[0]+1)%nP;
	indices[2] = (indices[1]+1)%nP; 
	indices[3] = (indices[2]+1)%nP;
	
	getCatmullRomPoint(t, CRpoints[indices[0]], CRpoints[indices[1]], CRpoints[indices[2]], CRpoints[indices[3]], pos, deriv);
}

void translateCR(bool align, float time, string points){

	nP = 0;
	static float y_bf[3] = {0,1,0};
	time = time*1000;

	for(char coords : points){
		if (coords == ',') nP++;
	}

	nP = nP/3;

	vector<vector<float>> CRpoints;
	string Pvalues[nP*3];
    int curr = 0, i = 0, start = 0, end = 0;

	// Extração e organização dos pontos com os quais
	// se vai construir a curva de catmullrom  

    while (i <= len(points)-1)  
    {  
        if (points[i] == ',' || i == len(points)-1)  
        {  
            end = i;  
            string subStr = "";
            subStr.append(points, start, end - start);  
            Pvalues[curr] = subStr;
            curr += 1;  
            start = end + 1;  
        }  
        i++;  
    }
	int n = 0;
	for(int i = 0; i < nP; i++){
		vector<float> vaux;
		for(int j = 0; j < 3; j++){
			vaux.push_back(stof(Pvalues[n]));
			n++;
		}
		CRpoints.push_back(vaux);
	}

	// Desenha a curva de catmullrom

	float posaux[3];
    float derivaux[3];

    glBegin(GL_LINE_LOOP);
    for(int i=0;i<100;i++){
        getGlobalCatmullRomPoint(pow(100,-1) * i,posaux,derivaux, CRpoints);
        glVertex3f(posaux[0],posaux[1],posaux[2]);
    }
    glEnd();

	float pos[3], deriv[3];

	float t = glutGet(GLUT_ELAPSED_TIME)/time;

	getGlobalCatmullRomPoint(t, pos, deriv,CRpoints);
	
	glTranslatef(pos[0], pos[1], pos[2]);

	if (camLookx == 0 && camLooky == 0 && camLookz == 0){
		if (placement % nmodels == target){
			centerCoords[0] = -pos[0];
			centerCoords[1] = pos[1];
			centerCoords[2] = -pos[2];
		}
		if (target == 0){
			centerCoords[0] = 0;
			centerCoords[1] = 0;
			centerCoords[2] = 0;
		}
	}

	// Verifica se o align está com o valor de true
	// e caso esteja alinha à curva de catmullrom
	if (align){

		float xm[3] = {deriv[0],deriv[1],deriv[2]};
		float ym[3], zm[3], m[16];

		normalize(xm);
		cross(xm,y_bf,zm);

		normalize(zm);
		cross(zm,xm,ym);

		normalize(ym);
		memcpy(y_bf, ym, 3*sizeof(float));

		buildRotMatrix(xm, ym, zm, m);

		glMultMatrixf(m);
	}


}

void rotateT(float time, float x, float y, float z){
	time = time*1000;
	float angle = glutGet(GLUT_ELAPSED_TIME)*360/time;
    glRotatef(angle, x, y, z);
}


void renderText() {

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(-10, w, h, -10);
	glMatrixMode(GL_MODELVIEW);

	glDisable(GL_DEPTH_TEST);

	glPushMatrix();
	glLoadIdentity();
	glRasterPos2d(10, 20);


	char text[64];
	if (target == 0){
		strcpy(text,"Sun");
	}
	else if (target == 1){
		strcpy(text,"Mercury");
	}
	else if (target == 2){
		strcpy(text,"Venus");
	}
	else if (target == 3){
		strcpy(text,"Earth");
	}
	else if (target == 5){
		strcpy(text,"Mars");
	}
	else if (target == 8){
		strcpy(text,"Comet");
	}
	else if (target == 9){
		strcpy(text,"Jupyter");
	}
	else if (target == 14){
		strcpy(text,"Saturn");
	}
	else if (target == 18){
		strcpy(text,"Uranus");
	}
	else if (target == 23){
		strcpy(text,"Neptune");
	}

	for (char *c = text; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
}

void renderScene(void) {
	float fps;
	int time;
	char s[64];

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	glTranslatef(centerCoords[0],centerCoords[1],centerCoords[2]);

	string action;
	nmodels = 0;

	actions.clear();
	actions.seekg(0);
	
	while (actions >> action){
		if (action.substr(0,5) == "model"){
			nmodels++;
		}
	}

	actions.clear();
	actions.seekg(0);
	
	GLuint buffer[nmodels];
	
	glGenBuffers(nmodels, buffer);

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
			if (strings[1] == "true"){
				
				translateCR(true,stof(strings[2]),strings[3]);
			}
			else if (strings[1] == "false"){
				translateCR(false,stof(strings[2]),strings[3]);
			}
			else{
				glTranslatef(stof(strings[1]),stof(strings[2]),stof(strings[3]));
			}
			
		}
		else if (strings[0] =="rotate"){
			if (strings[1] == "T"){
				rotateT(stof(strings[2]),stof(strings[3]),stof(strings[4]),stof(strings[5]));
			}
			else{
				glRotatef(stof(strings[1]),stof(strings[2]),stof(strings[3]),stof(strings[4]));
			}
		}
		else if (strings[0] =="scale"){
			glScalef(stof(strings[1]),stof(strings[2]),stof(strings[3]));
		}
		else {
			prepareArray(strings[1],buffer);
			DrawModels(buffer);
		}					
	}
	glPopMatrix();
	
	if (showfps){
		frame++;
		time=glutGet(GLUT_ELAPSED_TIME); 
		if (time - timebase > 1000) { 
			fps = frame*1000.0/(time-timebase); 
			timebase = time; 
			frame = 0;
			string name;
			
			sprintf(s, "FPS: %f6.2", fps);
			glutSetWindowTitle(s);
		}
	}
	else{
		glutSetWindowTitle("Demo");
	}

	renderText();

	iterarbuff = 0;

	glutSwapBuffers();
}

void groupReader(xml_node<> * group){

	actions << "group|true ";

	xml_node<> * transformaux = group->first_node("transform")->first_node();
	while (transformaux){

		string nome = transformaux->name();
		if (nome =="translate"){
			xml_attribute<> *time =  transformaux->first_attribute("time");
			xml_attribute<> *align = transformaux->first_attribute("align");

			if (time && align){
				actions << "translate|" << align->value() << "|" << time->value() << "|";
				
				xml_node<> * pointsaux = transformaux->first_node("point");

				while(pointsaux){
					char *x = pointsaux->first_attribute("x")->value();
					char *y = pointsaux->first_attribute("y")->value();
					char *z = pointsaux->first_attribute("z")->value();
					actions << x << "," << y << "," << z << ",";

					pointsaux = pointsaux->next_sibling();
				}
				
				actions << " ";

			}
			else{
				char *x = transformaux->first_attribute("x")->value();
				char *y = transformaux->first_attribute("y")->value();
				char *z = transformaux->first_attribute("z")->value();

				actions << "translate|" << x << "|" << y << "|" << z << " ";
			}

		}
		else if (nome =="rotate"){
			xml_attribute<> *time = transformaux->first_attribute("time");
			xml_attribute<> *angle = transformaux->first_attribute("angle");

			char *x = transformaux->first_attribute("x")->value();
			char *y = transformaux->first_attribute("y")->value();
			char *z = transformaux->first_attribute("z")->value();

			if (time){
				actions << "rotate|T|" << time->value() << "|" << x << "|" << y << "|" << z << " ";
			}
			else{
				actions << "rotate|" << angle->value() << "|" << x << "|" << y << "|" << z << " ";
			}

		}
		else if (nome =="scale"){
			char *x = transformaux->first_attribute("x")->value();
			char *y = transformaux->first_attribute("y")->value();
			char *z = transformaux->first_attribute("z")->value();

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
		case 'f':
			if(showfps){
				showfps = false;
			}
			else{
				showfps = true;
			}
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
		case 'q':
			target--;
			while (target == 4 or target == 6 
				or target == 7 or target == 10 or target == 11 
				or target == 12 or target == 13 or target == 15 
				or target == 16 or target == 17 or target == 19
				or target == 20 or target == 21 or target == 22 
				or target == 24){
				target--;
			}
			if (target < 0){
				target = nmodels - 2;
			}
			break;
		case 'e':
			target++;
			while (target == 4 or target == 6 
				or target == 7 or target == 10 or target == 11 
				or target == 12 or target == 13 or target == 15 
				or target == 16 or target == 17 or target == 19
				or target == 20 or target == 21 or target == 22 
				or target == 24){
				target++;
			}
			if (target >= nmodels){
				target = 0;
			}
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
	cout << "F <---------------------------------------- SHOW FPS\n";
	cout << "W,A,S,D <---------------------------------- MOVE THE CAMERA FREELY\n";
	cout << "C <---------------------------------------- RESET THE CAMERA\n\n";
	cout << "!!!MOUSE COMMANDS ONLY WORK WHILE THE CAMERA IS LOCKED AT SOME PLANET/SUN!!!\n\n";
	cout << "MOUSE LEFT CLICK <------------------------- ROTATE THE CAMERA AROUND ORIGIN\n";
	cout << "MOUSE RIGHT CLICK <------------------------ ZOOM TOWARDS ORIGIN\n\n";
	cout << "Q AND E KEYS SWITCH THE CAMERA FOCUS\n\n";
	cout << "E <-------- NEXT PLANET\n";
	cout << "Q <-------- PREVIOUS PLANET\n";
}


int main(int argc, char **argv) {
	if (argc != 2){
		cout << "! Invalid call !\n";
		cout << "Run call must be in the following format:\n";
		cout << "./[executable] [XMLfile]\n";
		return 0;
	}
	XMLreading(&argv[1]);

	r = sqrt(pow(camPosx,2) + pow(camPosy,2) + pow(camPosz,2));
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(windowWidth,windowHeight);
	glutCreateWindow("Demo");
		 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnableClientState(GL_VERTEX_ARRAY);
	
	info();

#ifndef __APPLE__	
	glewInit();
#endif	
	
	glutMainLoop();

	return 1;
}
