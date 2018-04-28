#include <iostream>
#include <string>
#include <algorithm>
#include <utility>
#include <vector>
#include <fstream>
#ifdef _DEBUG
	#undef _DEBUG
	#include <Python.h>
	#define _DEBUG
#else
	#include <Python.h>
#endif

#include "CGL/include/CGL/vector3D.h"
#include "CGL/include/CGL/vector2D.h"

using std::min;
using std::max;
using std::vector;
using namespace std;
using namespace CGL;

void callPython(long maze_w=16, long maze_h=8) {
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

std::vector<Vector3D> mazeboxCoords(Vector3D box_dimensions, string filename) {

	string line;
	ifstream mazeFile(filename);

	int maze_w = 0;
	int maze_h = 0;

	std::vector< std::vector<char> > arr;

	if (mazeFile.is_open()) {
		while (getline(mazeFile,line)) {
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
	} else {
		cout << "Unable to open file"; 
	}

	maze_w = arr[0].size();
	std::vector<Vector3D> boxes = std::vector<Vector3D>();

	ofstream coordinates ("coordinates.txt");
	if (coordinates.is_open()) {
		for (int r = 0; r < maze_h; ++r) {
			for (int c = 0; c < maze_w; ++c) {

				if (arr[r][c] == '1') {
					float x, y, z;
					x = r * box_dimensions.x + (box_dimensions.x / 2.0);
					y = c * box_dimensions.y + (box_dimensions.y / 2.0);
					z = box_dimensions.z / 2.0;
					boxes.push_back(Vector3D(x, y, z));
					coordinates << x << ", " << y << ", " << z << "\n";
				}
			}
		}

		coordinates.close();	
	} else {
		cout << "Unable to open file"; 
	}

	return boxes;

}


int main(int argc, char *argv[]) {
	/*if (argc != 2) {
		cout << "This function requires 1 parameter: the name of the file we are reading in" << endl;
		return 0;
	}*/
	callPython(16, 8);
	mazeboxCoords(Vector3D(3, 3, 3), "maze.txt");
	return 0;
}



