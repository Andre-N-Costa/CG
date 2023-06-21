#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <math.h>

using namespace std;

string Gsphere(float radius, int slices, int stacks){

    stringstream buffer;
    
    float x1, x2, x3, x4, y1, y2, z1, z2, z3, z4;

    float a = 2 * M_PI / slices; // angle for each slice

    float b = M_PI / stacks; // angle for each stack
    
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < stacks; j++) {

            x1 = radius * cos(M_PI_2 - b * j) * sin(a * i);
            x2 = radius * cos(M_PI_2 - b * (j+1)) * sin(a * i);
            x3 = radius * cos(M_PI_2 - b * (j+1)) * sin(a * (i + 1));
            x4 = radius * cos(M_PI_2 - b * j) * sin(a * (i + 1));

            y1 = radius * sin(M_PI_2 - b * j);
            y2 = radius * sin(M_PI_2 - b * (j+1));

            z1 = radius * cos(M_PI_2 - b * j) * cos(a * i);
            z2 = radius * cos(M_PI_2 - b * (j+1)) * cos(a * i);
            z3 = radius * cos(M_PI_2 - b * (j+1)) * cos(a * (i + 1));
            z4 = radius * cos(M_PI_2 - b * j) * cos(a * (i + 1));


            if (j != stacks-1) {
                buffer << x1 << ' ' << y1 << ' ' << z1 << '\n';
                buffer << x2 << ' ' << y2 << ' ' << z2 << '\n';
                buffer << x3 << ' ' << y2 << ' ' << z3 << '\n';
            }
        
            if (j != 0) {
                buffer << x1 << ' ' << y1 << ' ' << z1 << '\n';
                buffer << x3 << ' ' << y2 << ' ' << z3 << '\n';
                buffer << x4 << ' ' << y1 << ' ' << z4 << '\n';
            }


        }
    }
    return buffer.str();
}

string Gcone(float radius, float height, int slices, int stacks){
        
    stringstream buffer;

    float ratio = height / radius, stack_size = height/stacks;

    float x1, x2, x3, x4, y1, y2, z1, z2, z3, z4, h1, h2, r1, r2;

    float a = 2 * M_PI / slices; // angle for each slice


    // bottom of the cone
    for (int i = 0; i < slices; i++) {

        x1 = radius * sin(a * i);
        x2 = radius * sin(a * (i+1));
        z1 = radius * cos(a * i);
        z2 = radius * cos(a * (i+1));

        buffer << x1 << ' ' << 0 << ' ' << z1 << '\n';
        buffer << 0 << ' ' << 0 << ' ' << 0 << '\n';
        buffer << x2 << ' ' << 0 << ' ' << z2 << '\n';
    }

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

            buffer << x1 << ' ' << y1 << ' ' << z1 << '\n';
            buffer << x2 << ' ' << y1 << ' ' << z2 << '\n';
            buffer << x4 << ' ' << y2 << ' ' << z4 << '\n';

            if (j != slices - 1) {
                buffer << x4 << ' ' << y2 << ' ' << z4 << '\n';
                buffer << x2 << ' ' << y1 << ' ' << z2 << '\n';
                buffer << x3 << ' ' << y2 << ' ' << z3 << '\n';
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

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            
            x1 = i * unit - range;
            z1 = j * unit - range;
            x2 = (i+1) * unit - range;
            z2 = (j+1) * unit - range;

            buffer << x1 << ' ' << 0 << ' ' << z1 << '\n';
            buffer << x2 << ' ' << 0 << ' ' << z2 << '\n';
            buffer << x2 << ' ' << 0 << ' ' << z1 << '\n';

            buffer << x1 << ' ' << 0 << ' ' << z1 << '\n';
            buffer << x1 << ' ' << 0 << ' ' << z2 << '\n';
            buffer << x2 << ' ' << 0 << ' ' << z2 << '\n';
        }
    }

    return buffer.str();
}

string Gbox(float length, int divisions){

    stringstream buffer;

    float unit = length / divisions;
    float range = length / 2;
    float x1,x2,y1, y2, z1, z2;

    // bottom and top of the box

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            
            x1 = i * unit - range;
            z1 = j * unit - range;
            x2 = (i+1) * unit - range;
            z2 = (j+1) * unit - range;

            buffer << x1 << ' ' << range << ' ' << z1 << '\n';
            buffer << x2 << ' ' << range << ' ' << z2 << '\n';
            buffer << x2 << ' ' << range << ' ' << z1 << '\n';

            buffer << x1 << ' ' << range << ' ' << z1 << '\n';
            buffer << x1 << ' ' << range << ' ' << z2 << '\n';
            buffer << x2 << ' ' << range << ' ' << z2 << '\n';

            buffer << x2 << ' ' << -range << ' ' << z2 << '\n';
            buffer << x1 << ' ' << -range << ' ' << z1 << '\n';
            buffer << x2 << ' ' << -range << ' ' << z1 << '\n';

            buffer << x1 << ' ' << -range << ' ' << z2 << '\n';
            buffer << x1 << ' ' << -range << ' ' << z1 << '\n';
            buffer << x2 << ' ' << -range << ' ' << z2 << '\n';
        }
    }

    // sides of the box
    
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            
            y1 = i * unit - range;
            z1 = j * unit - range;
            y2 = (i+1) * unit - range;
            z2 = (j+1) * unit - range;

            buffer << range << ' ' << y1 << ' ' << z1 << '\n';
            buffer << range << ' ' << y2 << ' ' << z2 << '\n';
            buffer << range << ' ' << y1 << ' ' << z2 << '\n';

            buffer << range << ' ' << y1 << ' ' << z1 << '\n';
            buffer << range << ' ' << y2 << ' ' << z1 << '\n';
            buffer << range << ' ' << y2 << ' ' << z2 << '\n';

            buffer << -range << ' ' << y2 << ' ' << z2 << '\n';
            buffer << -range << ' ' << y1 << ' ' << z1 << '\n';
            buffer << -range << ' ' << y1 << ' ' << z2 << '\n';

            buffer << -range << ' ' << y2 << ' ' << z1 << '\n';
            buffer << -range << ' ' << y1 << ' ' << z1 << '\n';
            buffer << -range << ' ' << y2 << ' ' << z2 << '\n';
        }
    }

    
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            
            y1 = i * unit - range;
            x1 = j * unit - range;
            y2 = (i+1) * unit - range;
            x2 = (j+1) * unit - range;

            buffer << x2 << ' ' << y2 << ' ' << range << '\n';
            buffer << x1 << ' ' << y1 << ' ' << range << '\n';
            buffer << x2 << ' ' << y1 << ' ' << range << '\n';

            buffer << x1 << ' ' << y2 << ' ' << range << '\n';
            buffer << x1 << ' ' << y1 << ' ' << range << '\n';
            buffer << x2 << ' ' << y2 << ' ' << range << '\n';

            buffer << x1 << ' ' << y1 << ' ' << -range << '\n';
            buffer << x2 << ' ' << y2 << ' ' << -range << '\n';
            buffer << x2 << ' ' << y1 << ' ' << -range << '\n';

            buffer << x1 << ' ' << y1 << ' ' << -range << '\n';
            buffer << x1 << ' ' << y2 << ' ' << -range << '\n';
            buffer << x2 << ' ' << y2 << ' ' << -range << '\n';
        }
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

    ofstream outfile(argv[argc-1]);

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
    else{
        cout << "!Error found in input prompt!\n";
        cout << "Generator supports the following syntax:\n\n";
        cout << "sphere [radius](float) [slices](int) [stacks](int) [output file]\n";
        cout << "box [length](float) [divisions](int) [output file]\n";
        cout << "cone [radius](float) [height](float) [slices](int) [stacks](int) [output file]\n";
        cout << "plane [length](float) [divisions](int) [output file]\n";
        return 0;
    }

    outfile.close();

}
