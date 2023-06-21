#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <bits/stdc++.h> 

using namespace std;

void normalize(float *a) {
    float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]);
    if(l!=0){
        a[0] = a[0]/l;
        a[1] = a[1]/l;
        a[2] = a[2]/l;
    }
}

void cross(float *a, float *b, float *res) {

    res[0] = a[1]*b[2] - a[2]*b[1];
    res[1] = a[2]*b[0] - a[0]*b[2];
    res[2] = a[0]*b[1] - a[1]*b[0];
}

string Gsphere(float radius, int slices, int stacks){

    stringstream buffer;
    
    float x1, x2, x3, x4, y1, y2, z1, z2, z3, z4;
    float n1[3], n2[3], n3[3], n4[3];

    float a = 2 * M_PI / slices; // angle for each slice

    float b = M_PI / stacks; // angle for each stack
    
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < stacks; j++) {

            x1 = radius * cos(M_PI_2 - b * j) * sin(a * i);
            n1[0] = cos(M_PI_2 - b * j) * sin(a*i);

            x2 = radius * cos(M_PI_2 - b * (j+1)) * sin(a * i);
            n2[0] = cos(M_PI_2 - b * (j+1)) * sin(a*i);

            x3 = radius * cos(M_PI_2 - b * (j+1)) * sin(a * (i + 1));
            n3[0] = cos(M_PI_2 - b * (j+1)) * sin(a*(i+1));

            x4 = radius * cos(M_PI_2 - b * j) * sin(a * (i + 1));
            n4[0] = cos(M_PI_2 - b * j) * sin(a*(i+1));


            y1 = radius * sin(M_PI_2 - b * j);
            n1[1] = sin(M_PI_2 - b*j);
            n4[1] = sin(M_PI_2 - b*j);

            y2 = radius * sin(M_PI_2 - b * (j+1));
            n2[1] = sin(M_PI_2 - b * (j+1));
            n3[1] = sin(M_PI_2 - b * (j+1));


            z1 = radius * cos(M_PI_2 - b * j) * cos(a * i);
            n1[2] = cos(M_PI_2 - b * j) * cos(a*i);

            z2 = radius * cos(M_PI_2 - b * (j+1)) * cos(a * i);
            n2[2] = cos(M_PI_2 - b * (j+1)) * cos(a*i);

            z3 = radius * cos(M_PI_2 - b * (j+1)) * cos(a * (i + 1));
            n3[2] = cos(M_PI_2 - b * (j+1)) * cos(a*(i+1));

            z4 = radius * cos(M_PI_2 - b * j) * cos(a * (i + 1));
            n4[2] = cos(M_PI_2 - b * j) * cos(a*(i+1));

            normalize(n1);
            normalize(n2);
            normalize(n3);
            normalize(n4);

            if (j != stacks-1) {
                buffer << x1 << ' ' << y1 << ' ' << z1 << ' ';
                buffer << n1[0] << ' ' << n1[1] << ' ' << n1[2] << ' ';
                buffer << (float) i/slices << ' ' << (float) j/stacks << '\n';

                buffer << x2 << ' ' << y2 << ' ' << z2 << ' ';
                buffer << n2[0] << ' ' << n2[1] << ' ' << n2[2] << ' ';
                buffer << (float) i/slices << ' ' << (float) (j+1)/stacks << '\n';

                buffer << x3 << ' ' << y2 << ' ' << z3 << ' ';
                buffer << n3[0] << ' ' << n3[1] << ' ' << n3[2] << ' ';
                buffer << (float) (i+1)/slices << ' ' << (float) (j+1)/stacks << '\n';
            }
        
            if (j != 0) {
                buffer << x1 << ' ' << y1 << ' ' << z1 << '\n';
                buffer << n1[0] << ' ' << n1[1] << ' ' << n1[2] << ' ';
                buffer << (float) i/slices << ' ' << (float) j/stacks << '\n';
                
                buffer << x3 << ' ' << y2 << ' ' << z3 << ' ';
                buffer << n3[0] << ' ' << n3[1] << ' ' << n3[2] << ' ';
                buffer << (float) (i+1)/slices << ' ' << (float) (j+1)/stacks << '\n';
                
                buffer << x4 << ' ' << y1 << ' ' << z4 << ' ';
                buffer << n4[0] << ' ' << n4[1] << ' ' << n4[2] << ' ';
                buffer << (float) (i+1)/slices << ' ' << (float) j/stacks << '\n';
            }
        }
    }
    return buffer.str();
}

string Gcone(float radius, float height, int slices, int stacks){
        
    stringstream buffer;

    float ratio = height / radius, stack_size = height/stacks;

    float x1, x2, x3, x4, y1, y2, z1, z2, z3, z4, h1, h2, r1, r2, ny;

    float a = 2 * M_PI / slices; // angle for each slice
    float n1[3], n2[3], n3[3], n4[3];


    // bottom of the cone
    for (int i = 0; i < slices; i++) {

        x1 = radius * sin(a * i);
        x2 = radius * sin(a * (i+1));
        z1 = radius * cos(a * i);
        z2 = radius * cos(a * (i+1));

        buffer << x1 << ' ' << 0 << ' ' << z1 << ' ';
        buffer << 0 << ' ' << -1 << ' ' << 0 << ' ';
        buffer << 0.5 + sin(a* i) << ' ' << 0.5 + cos(a * i) << '\n';
        
        buffer << 0 << ' ' << 0 << ' ' << 0 << ' ';
        buffer << 0 << ' ' << -1 << ' ' << 0 << ' ';
        buffer << (float) 0.5 << ' ' << (float) 0.5  << '\n';
        
        buffer << x2 << ' ' << 0 << ' ' << z2 << ' ';
        buffer << 0 << ' ' << -1 << ' ' << 0 << ' ';
        buffer << 0.5 + sin(a * (i+1)) << ' ' << 0.5 + cos(a * (i+1)) << '\n';

    }

    ny = sin(atan(radius/height));

    // sides of the cone
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {

            h1 = height - (i * stack_size);
            h2 = height - ((i+1) * stack_size);
            r1 = h1 / ratio;
            r2 = h2 / ratio;

            x1 = r1 * sin(a * j);
            x2 = r1 * sin(a * (j+1));
            x3 = r2 * sin(a * (j+1));
            x4 = r2 * sin(a * j);
            y1 = (i * stack_size);
            y2 = (i+1) * stack_size;
            z1 = r1 * cos(a * j);
            z2 = r1 * cos(a * (j+1));
            z3 = r2 * cos(a * (j+1));
            z4 = r2 * cos(a * j);

            n1[1] = ny;
            n2[1] = ny;
            n3[1] = ny;
            n4[1] = ny;

            z1 = r1 * cos(a * j);
            n1[2] = cos(a * j);
            z2 = r1 * cos(a * (j+1));
            n2[2] = cos(a * (j+1));
            z3 = r2 * cos(a * (j+1));
            n3[2] = cos(a * (j+1));
            z4 = r2 * cos(a * j);
            n4[2] = cos(a * j);


            normalize(n1);
            normalize(n2);
            normalize(n3);
            normalize(n4);

            buffer << x1 << ' ' << y1 << ' ' << z1 << ' ';
            buffer << n1[0] << ' ' << n1[1] << ' ' << n1[2] << ' ';
            buffer << (float) j/slices << ' ' << (float) i/stacks << '\n';
            
            buffer << x2 << ' ' << y1 << ' ' << z2 << ' ';
            buffer << n2[0] << ' ' << n2[1] << ' ' << n2[2] << ' ';
            buffer << (float) (j+1)/slices << ' ' << (float) i/stacks << '\n';
            
            buffer << x4 << ' ' << y2 << ' ' << z4 << ' ';
            buffer << n4[0] << ' ' << n4[1] << ' ' << n4[2] << ' ';
            buffer << (float) j/slices << ' ' << (float) (i+1)/stacks << '\n';


            if (i != stacks - 1) {
                buffer << x4 << ' ' << y2 << ' ' << z4 << ' ';
                buffer << n4[0] << ' ' << n4[1] << ' ' << n4[2] << ' ';
                buffer << (float) j/slices << ' ' << (float) (i+1)/stacks << '\n';                
                
                buffer << x2 << ' ' << y1 << ' ' << z2 << ' ';
                buffer << n2[0] << ' ' << n2[1] << ' ' << n2[2] << ' ';
                buffer << (float) (j+1)/slices << ' ' << (float) i/stacks << '\n';
                
                buffer << x3 << ' ' << y2 << ' ' << z3 << ' ';
                buffer << n3[0] << ' ' << n3[1] << ' ' << n3[2] << ' ';
                buffer << (float) (j+1)/slices << ' ' << (float) (i+1)/stacks << '\n';
            }           

        }
    }

    return buffer.str();
}

string Gplane(float length, int divisions){
    stringstream buffer;
    
    float unit = length / divisions;
    float range = length / 2;
    float x1, x2, z1, z2;
    float tex = pow(divisions,-1);

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            
            x1 = i * unit - range;
            z1 = j * unit - range;
            x2 = (i+1) * unit - range;
            z2 = (j+1) * unit - range;

            buffer << x1 << ' ' << 0 << ' ' << z1 << ' ';
            buffer << 0 << ' ' << 1 << ' ' << 0 << ' ';
            buffer << i*tex << ' ' << j*tex << '\n';
            buffer << x2 << ' ' << 0 << ' ' << z2 << ' ';
            buffer << 0 << ' ' << 1 << ' ' << 0 << ' ';
            buffer << (i+1)*tex << ' ' << (j+1)*tex << '\n';
            buffer << x2 << ' ' << 0 << ' ' << z1 << ' ';
            buffer << 0 << ' ' << 1 << ' ' << 0 << ' ';
            buffer << (i+1)*tex << ' ' << j*tex << '\n';

            buffer << x1 << ' ' << 0 << ' ' << z1 << ' ';
            buffer << 0 << ' ' << 1 << ' ' << 0 << ' ';
            buffer << i*tex << ' ' << j*tex << '\n';
            buffer << x1 << ' ' << 0 << ' ' << z2 << ' ';
            buffer << 0 << ' ' << 1 << ' ' << 0 << ' ';
            buffer << i*tex << ' ' << (j+1)*tex << '\n';
            buffer << x2 << ' ' << 0 << ' ' << z2 << ' ';
            buffer << 0 << ' ' << 1 << ' ' << 0 << ' ';
            buffer << (i+1)*tex << ' ' << (j+1)*tex << '\n';
        }
    }

    return buffer.str();
}

string Gbox(float length, int divisions){

    stringstream buffer;

    float unit = length / divisions;
    float range = length / 2;
    float x1,x2,y1, y2, z1, z2, tx1, tx2, tz1, tz2;
    float tex = pow(divisions,-1);

    // bottom and top of the box

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            
            x1 = i * unit - range;
            z1 = j * unit - range;
            x2 = (i+1) * unit - range;
            z2 = (j+1) * unit - range;
            tx1 = i*tex;
            tx2 = (i+1)*tex;
            tz1 = j*tex;
            tz2 = (j+1)*tex;

            //cima
            buffer << x1 << ' ' << range << ' ' << z1 << ' ';
            buffer << 0 << ' ' << 1 << ' ' << 0 << ' ';
            buffer << tx1 << ' ' << tz1 << '\n';

            buffer << x2 << ' ' << range << ' ' << z2 << ' ';
            buffer << 0 << ' ' << 1 << ' ' << 0 << ' ';
            buffer << tx2 << ' ' << tz2 << '\n';

            buffer << x2 << ' ' << range << ' ' << z1 << ' ';
            buffer << 0 << ' ' << 1 << ' ' << 0 << ' ';
            buffer << tx2 << ' ' << tz1 << '\n';



            buffer << x1 << ' ' << range << ' ' << z1 << ' ';
            buffer << 0 << ' ' << 1 << ' ' << 0 << ' ';
            buffer << tx1 << ' ' << tz1 << '\n';

            buffer << x1 << ' ' << range << ' ' << z2 << ' ';
            buffer << 0 << ' ' << 1 << ' ' << 0 << ' ';
            buffer << tx1 << ' ' << tz2 << '\n';

            buffer << x2 << ' ' << range << ' ' << z2 << ' ';
            buffer << 0 << ' ' << 1 << ' ' << 0 << ' ';
            buffer << tx2 << ' ' << tz2 << '\n';
            //fim cima

            //baixo
            buffer << x2 << ' ' << -range << ' ' << z2 << ' ';
            buffer << 0 << ' ' << -1 << ' ' << 0 << ' ';
            buffer << tx2 << ' ' << tz2 << '\n';

            buffer << x1 << ' ' << -range << ' ' << z1 << ' ';
            buffer << 0 << ' ' << -1 << ' ' << 0 << ' ';
            buffer << tx1 << ' ' << tz1 << '\n';

            buffer << x2 << ' ' << -range << ' ' << z1 << ' ';
            buffer << 0 << ' ' << -1 << ' ' << 0 << ' ';
            buffer << tx2 << ' ' << tz1 << '\n';



            buffer << x1 << ' ' << -range << ' ' << z2 << ' ';
            buffer << 0 << ' ' << -1 << ' ' << 0 << ' ';
            buffer << tx1 << ' ' << tz2 << '\n';

            buffer << x1 << ' ' << -range << ' ' << z1 << ' ';
            buffer << 0 << ' ' << -1 << ' ' << 0 << ' ';
            buffer << tx1 << ' ' << tz1 << '\n';

            buffer << x2 << ' ' << -range << ' ' << z2 << ' ';
            buffer << 0 << ' ' << -1 << ' ' << 0 << ' ';
            buffer << tx2 << ' ' << tz2 << '\n';
            //fim baixo
        }
    }

    // sides of the box
    
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            
            y1 = i * unit - range;
            z1 = j * unit - range;
            y2 = (i+1) * unit - range;
            z2 = (j+1) * unit - range;
            tx1 = i*tex;
            tx2 = (i+1)*tex;
            tz1 = j*tex;
            tz2 = (j+1)*tex;

            //direita
            buffer << range << ' ' << y1 << ' ' << z1 << ' ';
            buffer << 1 << ' ' << 0 << ' ' << 0 << ' ';
            buffer << tx1 << ' ' << tz1 << '\n';

            buffer << range << ' ' << y2 << ' ' << z2 << ' ';
            buffer << 1 << ' ' << 0 << ' ' << 0 << ' ';
            buffer << tx2 << ' ' << tz2 << '\n';

            buffer << range << ' ' << y1 << ' ' << z2 << ' ';
            buffer << 1 << ' ' << 0 << ' ' << 0 << ' ';
            buffer << tx2 << ' ' << tz1 << '\n';


            buffer << range << ' ' << y1 << ' ' << z1 << ' ';
            buffer << 1 << ' ' << 0 << ' ' << 0 << ' ';
            buffer << tx1 << ' ' << tz1 << '\n';

            buffer << range << ' ' << y2 << ' ' << z1 << ' ';
            buffer << 1 << ' ' << 0 << ' ' << 0 << ' ';
            buffer << tx1 << ' ' << tz2 << '\n';

            buffer << range << ' ' << y2 << ' ' << z2 << ' ';
            buffer << 1 << ' ' << 0 << ' ' << 0 << ' ';
            buffer << tx2 << ' ' << tz2 << '\n';
            // fim direita

            //esquerda
            buffer << -range << ' ' << y2 << ' ' << z2 << ' ';
            buffer << -1 << ' ' << 0 << ' ' << 0 << ' ';
            buffer << tx2 << ' ' << tz2 << '\n';

            buffer << -range << ' ' << y1 << ' ' << z1 << ' ';
            buffer << -1 << ' ' << 0 << ' ' << 0 << ' ';
            buffer << tx1 << ' ' << tz1 << '\n';

            buffer << -range << ' ' << y1 << ' ' << z2 << ' ';
            buffer << -1 << ' ' << 0 << ' ' << 0 << ' ';
            buffer << tx2 << ' ' << tz1 << '\n';


            buffer << -range << ' ' << y2 << ' ' << z1 << ' ';
            buffer << -1 << ' ' << 0 << ' ' << 0 << ' ';
            buffer << tx1 << ' ' << tz2 << '\n';

            buffer << -range << ' ' << y1 << ' ' << z1 << ' ';
            buffer << -1 << ' ' << 0 << ' ' << 0 << ' ';
            buffer << tx1 << ' ' << tz1 << '\n';

            buffer << -range << ' ' << y2 << ' ' << z2 << ' ';
            buffer << -1 << ' ' << 0 << ' ' << 0 << ' ';
            buffer << tx2 << ' ' << tz2 << '\n';

            //fim esquerda
        }
    }

    
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            
            y1 = i * unit - range;
            x1 = j * unit - range;
            y2 = (i+1) * unit - range;
            x2 = (j+1) * unit - range;
            tx1 = i*tex;
            tx2 = (i+1)*tex;
            tz1 = j*tex;
            tz2 = (j+1)*tex;

            //frente
            buffer << x2 << ' ' << y2 << ' ' << range << ' ';
            buffer << 0 << ' ' << 0 << ' ' << 1 << ' ';
            buffer << tx2 << ' ' << tz2 << '\n';

            buffer << x1 << ' ' << y1 << ' ' << range << ' ';
            buffer << 0 << ' ' << 0 << ' ' << 1 << ' ';
            buffer << tx1 << ' ' << tz1 << '\n';

            buffer << x2 << ' ' << y1 << ' ' << range << ' ';
            buffer << 0 << ' ' << 0 << ' ' << 1 << ' ';
            buffer << tx2 << ' ' << tz1 << '\n';


            buffer << x1 << ' ' << y2 << ' ' << range << ' ';
            buffer << 0 << ' ' << 0 << ' ' << 1 << ' ';
            buffer << tx1 << ' ' << tz2 << '\n';

            buffer << x1 << ' ' << y1 << ' ' << range << ' ';
            buffer << 0 << ' ' << 0 << ' ' << 1 << ' ';
            buffer << tx1 << ' ' << tz1 << '\n';

            buffer << x2 << ' ' << y2 << ' ' << range << ' ';
            buffer << 0 << ' ' << 0 << ' ' << 1 << ' ';
            buffer << tx2 << ' ' << tz2 << '\n';

            //fim frente

            //trás
            buffer << x1 << ' ' << y1 << ' ' << -range << ' ';
            buffer << 0 << ' ' << 0 << ' ' << -1 << ' ';
            buffer << tx1 << ' ' << tz1 << '\n';

            buffer << x2 << ' ' << y2 << ' ' << -range << ' ';
            buffer << 0 << ' ' << 0 << ' ' << -1 << ' ';
            buffer << tx2 << ' ' << tz2 << '\n';

            buffer << x2 << ' ' << y1 << ' ' << -range << ' ';
            buffer << 0 << ' ' << 0 << ' ' << -1 << ' ';
            buffer << tx2 << ' ' << tz1 << '\n';



            buffer << x1 << ' ' << y1 << ' ' << -range << ' ';
            buffer << 0 << ' ' << 0 << ' ' << -1 << ' ';
            buffer << tx1 << ' ' << tz1 << '\n';

            buffer << x1 << ' ' << y2 << ' ' << -range << ' ';
            buffer << 0 << ' ' << 0 << ' ' << -1 << ' ';
            buffer << tx1 << ' ' << tz2 << '\n';

            buffer << x2 << ' ' << y2 << ' ' << -range << ' ';
            buffer << 0 << ' ' << 0 << ' ' << -1 << ' ';
            buffer << tx2 << ' ' << tz2 << '\n';
            //fim trás
        }
    }

    return buffer.str();
}

void multMatrixVector(float m[4][4], float *v, float *res) {

    for (int j = 0; j < 4; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j][k];
        }
    }

}

void multMatrixMatrix(float a[4][4], float b[4][4], float res[4][4]){
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++){
            res[i][j] = 0;
            for(int k = 0; k < 4; k++)
                res[i][j] += a[i][k] * b[k][j];
        }
}

float Bpoint(float U, float V, float m[4][4]){

    float aux[4];
    float v[4];
    float r;

    v[0] = powf(V,3);
    v[1] = powf(V,2);
    v[2] = V;
    v[3] = 1;

    multMatrixVector(m,v,aux);

    r = powf(U,3)*aux[0] + powf(U,2)*aux[1] + U*aux[2] + aux[3];

    return r;
}

float Du(float U, float V, float m[4][4]){

    float aux[4];
    float v[4];
    float r;

    v[0] = powf(V,3);
    v[1] = powf(V,2);
    v[2] = V;
    v[3] = 1;

    multMatrixVector(m,v,aux);

    r = 3*powf(U,2)*aux[0] + 2*U*aux[1] + aux[2];



    return r;

}

float Dv(float U, float V, float m[4][4]){

    float aux[4];
    float v[4];
    float r;

    v[0] = 3*powf(V,2);
    v[1] = V*2;
    v[2] = 1;
    v[3] = 0;

    multMatrixVector(m,v,aux);

    r = powf(U,3)*aux[0] + powf(U,2)*aux[1] + U*aux[2] + aux[3];



    return r;

}


string Gsurface(float px[4][4], float py[4][4], float pz[4][4], int tesselation){

    stringstream buffer;
    float x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4;
    float p1u[3], p1v[3], n1[3], p2u[3], p2v[3], n2[3], p3u[3], p3v[3], n3[3], p4u[3], p4v[3], n4[3];
    float t = pow(tesselation,-1);

    for(float i=0; i<1; i+=t){
        for(float j=0; j<1; j+=t){


            x1 = Bpoint(i,j,px);
            p1u[0] = Du(i,j,px);
            p1v[0] = Dv(i,j,px);

            x2 = Bpoint(i+t,j,px);
            p2u[0] = Du(i+t,j,px);
            p2v[0] = Dv(i+t,j,px);

            x3 = Bpoint(i+t,j+t,px);
            p3u[0] = Du(i+t,j+t,px);
            p3v[0] = Dv(i+t,j+t,px);

            x4 = Bpoint(i,j+t,px);
            p4u[0] = Du(i,j+t,px);
            p4v[0] = Dv(i,j+t,px);
            

            y1 = Bpoint(i,j,py);
            p1u[1] = Du(i,j,py);
            p1v[1] = Dv(i,j,py);

            y2 = Bpoint(i+t,j,py);
            p2u[1] = Du(i+t,j,py);
            p2v[1] = Dv(i+t,j,py);

            y3 = Bpoint(i+t,j+t,py);
            p3u[1] = Du(i+t,j+t,py);
            p3v[1] = Dv(i+t,j+t,py);

            y4 = Bpoint(i,j+t,py);
            p4u[1] = Du(i,j+t,py);
            p4v[1] = Dv(i,j+t,py);


            z1 = Bpoint(i,j,pz);
            p1u[2] = Du(i,j,pz);
            p1v[2] = Dv(i,j,pz);

            z2 = Bpoint(i+t,j,pz);
            p2u[2] = Du(i+t,j,pz);
            p2v[2] = Dv(i+t,j,pz);

            z3 = Bpoint(i+t,j+t,pz);
            p3u[2] = Du(i+t,j+t,pz);
            p3v[2] = Dv(i+t,j+t,pz);

            z4 = Bpoint(i,j+t,pz);
            p4u[2] = Du(i,j+t,pz);
            p4v[2] = Dv(i,j+t,pz);

            cross(p1u,p1v,n1);
            cross(p2u,p2v,n2);
            cross(p3u,p3v,n3);
            cross(p4u,p4v,n4);

            normalize(n1);
            normalize(n2);
            normalize(n3);
            normalize(n4);

            /*buffer << x1 << ' ' << y1 << ' ' << z1 << '\n';
            
            buffer << x2 << ' ' << y2 << ' ' << z2 << '\n';

            buffer << x4 << ' ' << y4 << ' ' << z4 << '\n';
            
            
            buffer << x2 << ' ' << y2 << ' ' << z2 << '\n';

            buffer << x3 << ' ' << y3 << ' ' << z3 << '\n';

            buffer << x4 << ' ' << y4 << ' ' << z4 << '\n';*/



            buffer << x1 << ' ' << y1 << ' ' << z1 << '\n';
            buffer << n1[0] << ' ' << n1[1] << ' ' << n1[2] << ' ';
            buffer << j << ' ' << i << '\n';


            buffer << x2 << ' ' << y2 << ' ' << z2 << '\n';
            buffer << n2[0] << ' ' << n2[1] << ' ' << n2[2] << ' ';
            buffer << j << ' ' << i+t << '\n';


            buffer << x3 << ' ' << y3 << ' ' << z3 << '\n';
            buffer << n3[0] << ' ' << n3[1] << ' ' << n3[2] << ' ';
            buffer << j+t << ' ' << i+t << '\n';

            
            
            buffer << x3 << ' ' << y3 << ' ' << z3 << ' ';
            buffer << n3[0] << ' ' << n3[1] << ' ' << n3[2] << ' ';
            buffer << j+t << ' ' << i+t << '\n';

            
            buffer << x4 << ' ' << y4 << ' ' << z4 << ' ';
            buffer << n4[0] << ' ' << n4[1] << ' ' << n4[2] << ' ';
            buffer << j+t << ' ' << i << '\n';

            
            buffer << x1 << ' ' << y1 << ' ' << z1 << ' ';
            buffer << n1[0] << ' ' << n1[1] << ' ' << n1[2] << ' ';
            buffer << j << ' ' << i << '\n';
                    
        }
        
    }
    
    return buffer.str();
}

string Gpatch(string controlP, int tesselation){

    ifstream controlfile("3d/" + controlP);

	if(!controlfile){
    	std::cout <<"!Error opening " << controlP <<"!\n";
    	return "";
    }

    

    stringstream buffer;
    float bezierMat[4][4] = {{-1.0f,  3.0f, -3.0f, 1.0f},
                             { 3.0f, -6.0f,  3.0f, 0.0f},
                             {-3.0f,  3.0f,  0.0f, 0.0f},
                             { 1.0f,  0.0f,  0.0f, 0.0f}};
    vector<vector<float>> points;
    vector<vector<int>> indexes;
    string aux;
    float px[4][4];
    float py[4][4];
    float pz[4][4];

    controlfile >> aux;

    int npatches = stoi(aux);

    controlfile >> aux;
    for(int i = 0; i < npatches; i++){
        vector<int> vec;
        
        while(aux[aux.length() - 1] == ','){
            vec.push_back(stoi(aux.substr(0,aux.length() - 1)));
            controlfile >> aux;
            
        }


        vec.push_back(stoi(aux));
        indexes.push_back(vec);
        controlfile >> aux;
    }

    int npoints = stoi(aux);
    int i = 0;

    while(controlfile >> aux){
        vector<float> vec;
        vec.push_back(stof(aux));
        controlfile >> aux;
        vec.push_back(stof(aux));
        controlfile >> aux;
        vec.push_back(stof(aux));
        points.push_back(vec);
        i++;
    }

    float mataux[4][4];

    
    for(vector<int> indice: indexes){

        for(int i=0; i<4; i++){
            for(int j = 0; j<4; j++){
                vector<float> thispoint = points[indice[i*4+j]];
                
                px[j][i] = thispoint[0];
                py[j][i] = thispoint[1];
                pz[j][i] = thispoint[2];
                
            }
            
        }
        
        multMatrixMatrix(bezierMat,px,mataux);
        multMatrixMatrix(mataux,bezierMat,px);

        multMatrixMatrix(bezierMat,py,mataux);
        multMatrixMatrix(mataux,bezierMat,py);

        multMatrixMatrix(bezierMat,pz,mataux);
        multMatrixMatrix(mataux,bezierMat,pz);

        
        buffer << Gsurface(px,py,pz,tesselation);
        
    }
        
    
    return buffer.str();
}


bool isFloat(string s) {

    bool point = false;

    if (!isdigit(s[0])) return false;

    for (int i = 1; i < s.string::length(); i++){
        if ((s[i] == '.' or s[i] == ',') and !point){
            point = true;
        }
        else if (!isdigit(s[i])){
            return false;
        }
    }
    return true;
}

bool isInt(string s) {
  for (int i = 0; i < s.string::length(); i++){
    if (!isdigit(s[i])) return false;
  }
  return true;
}

int main(int argc, char const *argv[]){

    string filename = argv[argc-1];

    ofstream outfile(string("3d/") + argv[argc-1]);

    if(!outfile){
        cout <<"!Error opening " << argv[argc-1] <<"!\n";
        return 0;
    }

    if (strcmp(argv[1],"sphere") == 0 and argc == 6 and isFloat(argv[2]) and isInt(argv[3]) and isInt(argv[4])){
        outfile << Gsphere(stof(argv[2]),stoi(argv[3]),stoi(argv[4]));
    }
    else if (strcmp(argv[1],"box") == 0 and argc == 5 and isFloat(argv[2]) and isInt(argv[3])){
        outfile << Gbox(stof(argv[2]),stoi(argv[3]));
    }
    else if (strcmp(argv[1],"cone") == 0 and argc == 7 and isFloat(argv[2]) and isFloat(argv[3]) and isInt(argv[4]) and isInt(argv[5])){
        outfile << Gcone(stof(argv[2]),stof(argv[3]),stoi(argv[4]),stoi(argv[5]));
    }
    else if (strcmp(argv[1],"plane") == 0 and argc == 5 and isFloat(argv[2]) and isInt(argv[3])){
        outfile << Gplane(stof(argv[2]),stoi(argv[3]));
    }
    else if (strcmp(argv[1],"patch") == 0 and argc == 5 and isInt(argv[3])){
        outfile << Gpatch(argv[2],stoi(argv[3]));
    }
    else{
        cout << "!Error found in input prompt!\n";
        cout << "Generator supports the following syntax:\n\n";
        cout << "sphere [radius](float) [slices](int) [stacks](int) [output file]\n";
        cout << "box [length](float) [divisions](int) [output file]\n";
        cout << "cone [radius](float) [height](float) [slices](int) [stacks](int) [output file]\n";
        cout << "plane [length](float) [divisions](int) [output file]\n";
        cout << "patch [input file] [tesselation level](int) [output file]\n";
        return 0;
    }

    outfile.close();

}