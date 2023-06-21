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
#include <sstream>
#include <string.h>
#include <IL/il.h>

using namespace rapidxml;
using namespace std;
float windowWidth,windowHeight,camPosx,camPosy,camPosz,camLookx,camLooky,camLookz;
float camUpx,camUpy,camUpz,camProjfov = 60.0f,camProjnear = 1.0f,camProjfar = 1000.0f;
float centerCoords[3] = {0,0,0};
stringstream models;
stringstream actions;
stringstream lights;
int alfaM = 45, betaM = 45, r;
int startX, startY, tracking = 0, target = 0;
int iterarbuff = 0,timebase = 0, frame = 0,w,h;
int nP = 0, placement = 0, nmodels = 0;
GLuint nvmodel,nvnormal,nvtex;
bool showfps = false;
vector < string > subactions;
float *luzes = (float *)malloc((4*8) * sizeof(float));
float **luzesSL;
 


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

void prepareArray(string model, GLuint *bufferModels, GLuint *bufferNormal, GLuint *bufferTex){

	float *modelArray;
	float *normalArray;
	float *texArray;
	int size;
	string item;
	string coord;
	size = nvmodel = nvnormal = nvtex = 0;

	ifstream modelfile("../../3d/" + model);

	if(!modelfile){
    	std::cout <<"!Error opening " << model <<"!\n";
    	return ;
    }

	// Calcula o tamanho do modelfile para alocar espaço suficiente no modelArray

	while(modelfile >> item){
		size++;
	}

	int nlines = size/8;


	modelArray = (float *)malloc(sizeof(float) * nlines * 3);
	normalArray = (float *)malloc(sizeof(float) * nlines * 3);
	texArray = (float *)malloc(sizeof(float) * nlines * 2);


	// São alocados os valores de modelfile no modelArray

	modelfile.clear();
	modelfile.seekg(0);

	

	while (modelfile >> coord){
		for(int j = 0; j <= 2; j++){
			modelArray[nvmodel] = stof(coord);
			modelfile >> coord;
			nvmodel++;  
		}    
		for(int j = 0; j <= 2; j++){
			normalArray[nvnormal] = stof(coord);
			modelfile >> coord;						
			nvnormal++;                                  
		}												 
		for(int j = 0; j <= 1; j++){
			
			texArray[nvtex] = stof(coord);
			
			if (j == 0){
				modelfile >> coord;
			}
			nvtex++;
		}

	}

	modelfile.close();

	// Buffer guarda os pontos que serão desenhados

	glBindBuffer(GL_ARRAY_BUFFER, bufferModels[iterarbuff]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nvmodel, modelArray, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, bufferNormal[iterarbuff]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nvnormal, normalArray, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, bufferTex[iterarbuff]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nvtex, texArray, GL_STATIC_DRAW);
	
	free(modelArray);
	free(normalArray);
	free(texArray);
}


void DrawModels(GLuint *bufferModels, GLuint *bufferNormal, GLuint *bufferTex, GLuint texModel){

	// Desenha os vértices guardados no buffer
	glBindBuffer(GL_ARRAY_BUFFER,bufferModels[iterarbuff]);
	
	glVertexPointer(3,GL_FLOAT,0,0);
	
	glBindBuffer(GL_ARRAY_BUFFER,bufferNormal[iterarbuff]);
	glNormalPointer(GL_FLOAT,0,0);
	

	if(texModel!=-1){

		glBindTexture(GL_TEXTURE_2D, texModel);
		
		glBindBuffer(GL_ARRAY_BUFFER,bufferTex[iterarbuff]);
		glTexCoordPointer(2,GL_FLOAT,0,0);
	}
	glDrawArrays(GL_TRIANGLES, 0, nvmodel/3);

	glBindTexture(GL_TEXTURE_2D, 0);

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

void customSplit(string str, char separator) {
    int startIndex = 0, endIndex = 0;
    for (int i = 0; i <= str.size(); i++) {
        
        if (str[i] == separator || i == str.size()) {
            endIndex = i;
            string temp;
            temp.append(str, startIndex, endIndex - startIndex);
            subactions.push_back(temp);
            startIndex = endIndex + 1;
        }
    }
}

int getLight(int nLight) {
    int luz1;
    switch (nLight) {
        case 0: luz1 = GL_LIGHT0; break;
        case 1: luz1 = GL_LIGHT1; break;
        case 2: luz1 = GL_LIGHT2; break;
        case 3: luz1 = GL_LIGHT3; break;
        case 4: luz1 = GL_LIGHT4; break;
        case 5: luz1 = GL_LIGHT5; break;
        case 6: luz1 = GL_LIGHT6; break;
        case 7: luz1 = GL_LIGHT7; break;
        default: exit(1);
    }
    return luz1;
}



void lightsSetup(){

	lights.clear();
	lights.seekg(0);

	string light;

	int n=0;
	float dark[4] = {0.2, 0.2, 0.2, 1.0};
	float white[4] = {1.0, 1.0, 1.0, 1.0};
	float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	while(lights>>light){

		subactions.clear();
		customSplit(light,'|');

		glLightfv(getLight(n), GL_AMBIENT, dark);
		glLightfv(getLight(n), GL_DIFFUSE, white);
		glLightfv(getLight(n), GL_SPECULAR, white);

		if(subactions[0]=="point"){
			float pnt[4]={stof(subactions[1]),stof(subactions[2]),stof(subactions[3]),1.0f};
			glLightfv(getLight(n),GL_POSITION,pnt);

		}
		else if(subactions[0]=="directional"){
			float direct[4]={stof(subactions[1]),stof(subactions[2]),stof(subactions[3]),0.0f};
			glLightfv(getLight(n),GL_POSITION,direct); 
		}
		else if(subactions[0]=="spotlight"){
			float pnt[4]={stof(subactions[1]),stof(subactions[2]),stof(subactions[3]),1.0f};
			float direct[4]={stof(subactions[4]),stof(subactions[5]),stof(subactions[6]),0.0f};

			glLightfv(getLight(n),GL_POSITION,pnt);
			glLightfv(getLight(n), GL_SPOT_DIRECTION, direct);
            glLightf(getLight(n), GL_SPOT_CUTOFF, stof(subactions[7]));
		}
		n++;
	}
		
}


GLuint loadTexture(std::string s) {

	unsigned int t,tw,th;
	unsigned char *texData;
	GLuint texID;

	ilInit();
	
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilGenImages(1,&t);
	ilBindImage(t);
	ilLoadImage((ILstring)s.c_str());
	tw = ilGetInteger(IL_IMAGE_WIDTH);
	th = ilGetInteger(IL_IMAGE_HEIGHT);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	texData = ilGetData();
	
	glGenTextures(1,&texID);
	
	glBindTexture(GL_TEXTURE_2D,texID);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_T,GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	
	glGenerateMipmap(GL_TEXTURE_2D);
	
	return texID;
}

/*void ApplyTexture(string tex){
	texModel = loadTexture("texturas/" + tex);	
}*/

void renderScene(void) {
	float fps;
	int time;
	char s[64];

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camPosx,camPosy,camPosz, 
		      camLookx,camLooky,camLookz,
			  camUpx,camUpy,camUpz);

	float aux[4] = {1,0.7,0.5,0};

	lightsSetup();

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

	actions.clear();
	actions.seekg(0);
	
	GLuint bufferModels[nmodels],bufferNormal[nmodels],bufferTex[nmodels];
	
	glGenBuffers(nmodels, bufferModels);
	glGenBuffers(nmodels, bufferNormal);
	glGenBuffers(nmodels, bufferTex);

	while (actions >> action){
		GLuint texModel = -1;
		subactions.clear();

    	customSplit(action,'|');

		if (subactions[0] == "group"){
			if (subactions[1] =="true"){
				glPushMatrix();
			}
			else {
				
				glPopMatrix();
			}
		}
		else if (subactions[0] == "translate"){
			if (subactions[1] == "true"){
				
				translateCR(true,stof(subactions[2]),subactions[3]);
			}
			else if (subactions[1] == "false"){
				translateCR(false,stof(subactions[2]),subactions[3]);
			}
			else{
				glTranslatef(stof(subactions[1]),stof(subactions[2]),stof(subactions[3]));
			}
			
		}
		else if (subactions[0] =="rotate"){
			if (subactions[1] == "T"){
				rotateT(stof(subactions[2]),stof(subactions[3]),stof(subactions[4]),stof(subactions[5]));
			}
			else{
				glRotatef(stof(subactions[1]),stof(subactions[2]),stof(subactions[3]),stof(subactions[4]));
			}
		}
		else if (subactions[0] =="scale"){
			glScalef(stof(subactions[1]),stof(subactions[2]),stof(subactions[3]));
		}
		else {
			int j = 2;
			
			prepareArray(subactions[1],bufferModels, bufferNormal,bufferTex);
			
			while(j < subactions.size()){

				bool default1 = true;
				
				if (subactions[j] == "diffuse"){
					
					default1 = false;

					float color[4] = {stof(subactions[j+1])/255,stof(subactions[j+2])/255,stof(subactions[j+3])/255,1.0f};
					
					glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
					j+=4;
					 
				}
				else if (subactions[j] == "ambient"){

					default1 = false;
					float color[4] = {stof(subactions[j+1])/255,stof(subactions[j+2])/255,stof(subactions[j+3])/255,1.0f};
					
					glMaterialfv(GL_FRONT, GL_AMBIENT, color);
					j+=4;

				}
				else if (subactions[j] == "specular"){
					default1 = false;
					float color[4] = {stof(subactions[j+1])/255,stof(subactions[j+2])/255,stof(subactions[j+3])/255,1.0f};
					
					glMaterialfv(GL_FRONT, GL_SPECULAR, color);
					j+=4;
				
				}
				else if (subactions[j] == "emissive"){
					
					default1 = false;
					float color[4] = {stof(subactions[j+1])/255,stof(subactions[j+2])/255,stof(subactions[j+3])/255,1.0f};
					
					glMaterialfv(GL_FRONT, GL_EMISSION, color);
					j+=4;
				}
				else if (subactions[j] == "shininess"){
					
					default1 = false;
					glMaterialf(GL_FRONT, GL_SHININESS, stof(subactions[j+1]));
					j+=2;
					
				}
				
				if (default1){
					
					float dark[] = { 0.2, 0.2, 0.2, 1.0 };
                    float white[] = { 0.8, 0.8, 0.8, 1.0 };
                    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dark);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                    glMaterialf(GL_FRONT, GL_SHININESS, 128);
					
				}

				if (subactions[j] == "texture"){
					texModel = loadTexture("texturas/" + subactions[j+1]);
					j+=2;
				}
			}
			
			DrawModels(bufferModels, bufferNormal,bufferTex,texModel);
			

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

	xml_node<> * nivel2 = group->first_node();
	while (nivel2){


		//string nome = transformaux->name();
		string nome = nivel2->name();
		if(nome=="transform"){

			xml_node<> * transformaux = group->first_node("transform")->first_node();

			while(transformaux){

				string nomeT = transformaux->name();

				if (nomeT =="translate"){
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
				else if (nomeT =="rotate"){
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
				else if (nomeT =="scale"){
					char *x = transformaux->first_attribute("x")->value();
					char *y = transformaux->first_attribute("y")->value();
					char *z = transformaux->first_attribute("z")->value();

					actions << "scale|" << x << "|" << y << "|" << z << " ";
				}
					
				transformaux = transformaux->next_sibling();
			}
		}

		if(nome=="models"){

			xml_node<> * model1 = group->first_node("models");

			for (xml_node<> * modelaux = model1->first_node("model");
			modelaux;
			modelaux = modelaux->next_sibling()){

				actions << "model|" << modelaux->first_attribute("file")->value();

				xml_node<> * details = modelaux->first_node();
				string nomeM = details->name();

				if(nomeM=="texture"){
					actions << "|texture|" << details->first_attribute("file")->value();
					details=details->next_sibling();
					if (details){
						nomeM = details->name();
					}
					
				}

				if(nomeM=="color"){
					
					for (xml_node<> *cor = details->first_node();
					cor;
					cor = cor->next_sibling()){
						char * nomeC = cor->name();

						if(!(cor->next_sibling())){
							actions << "|" << nomeC << "|" << details->first_node(nomeC)->first_attribute("value")->value();
							break;
						}

						char * red = details->first_node(nomeC)->first_attribute("R")->value();
						char * green = details->first_node(nomeC)->first_attribute("G")->value();
						char * blue = details->first_node(nomeC)->first_attribute("B")->value();

						actions << "|" << nomeC << "|" << red << "|" << green << "|" << blue; 
					}
					
				}
			}
			actions << " ";
		}


		for(xml_node<> * groupaux = group->first_node("group"); groupaux; groupaux = groupaux->next_sibling()){
			groupReader(groupaux);
		}
		nivel2 = nivel2->next_sibling();
		
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
	
	for (xml_node<> * lightaux = doc.first_node("world")->first_node("lights")->first_node("light");
		lightaux;
		lightaux = lightaux->next_sibling()){

		string nomeT = lightaux->first_attribute("type")->value();

		if (nomeT == "point"){
			lights << "point" << "|" << lightaux->first_attribute("posx")->value() << "|" << lightaux->first_attribute("posy")->value() << "|"
			 << lightaux->first_attribute("posz")->value() << " ";
		}

	
		if (nomeT == "directional"){
			lights << "directional" << "|" << lightaux->first_attribute("dirx")->value() << "|" << lightaux->first_attribute("diry")->value() << "|"
			 << lightaux->first_attribute("dirz")->value() << " ";
		}

		if (nomeT == "spotlight"){
			lights << "spotlight" << "|" << lightaux->first_attribute("posx")->value() << "|" << lightaux->first_attribute("posy")->value() << "|"
			<< lightaux->first_attribute("posz")->value() << "|" << lightaux->first_attribute("dirx")->value() << "|" << lightaux->first_attribute("diry")->value() << "|"
			<< lightaux->first_attribute("dirz")->value() << "|" << lightaux->first_attribute("cutoff")->value() << " ";
		}
	}

	

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




void initGL() {

// alguns settings para OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_RESCALE_NORMAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
	glEnable(GL_LIGHT5);
	glEnable(GL_LIGHT6);
	glEnable(GL_LIGHT7);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glClearColor(0, 0, 0, 0);

	float dark[4] = {0.3,0.3,0.3,1.0};
	float white[4] = {1,1,1,1.0};
	float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	float amb[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0,GL_AMBIENT,dark);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,white);
	glLightfv(GL_LIGHT0,GL_SPECULAR,white);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);


	glEnable(GL_TEXTURE_2D);


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
	
	info();

#ifndef __APPLE__	
	glewInit();
#endif

	initGL();

	
	glutMainLoop();

	return 1;
}
