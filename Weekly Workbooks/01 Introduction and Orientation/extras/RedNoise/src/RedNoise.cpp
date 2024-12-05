#include <vector>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
//#include <vector>
#include <glm/glm.hpp>	 //Week 2
#include <CanvasPoint.h> //Week 3
#include <Colour.h>
#include <algorithm>
#include <CanvasTriangle.h>
#include <random>
#include <TexturePoint.h>
#include <TextureMap.h>
#include <string>
#include <iostream>
#include <ModelTriangle.h>
#include <fstream>
#include <sstream>
#include <unordered_map>


std::vector<ModelTriangle> OBJFileLoader(std::string filename, float scale){
	std::vector<glm::vec3> vectorList;
	std::vector<ModelTriangle> triangles;
	std::cout << "SEcond test" << std::endl;
	std::string line;
	std::ifstream file(filename);
	if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        //return 1; // Exit with an error code
    }

	while (std::getline (file, line)) {
    // Output the text from the file
		//cout << text;
		//if line starts with v
		if (line.empty()) {continue;}
		else if (line[0] == 'v'){
			std::istringstream iss(line);
			std::string type = "v";  
			float x, y, z;
			iss >> type >> x >> y >> z;
			glm::vec3 tempVector;
			tempVector.x = x*scale;
			tempVector.y = y*scale;
			tempVector.z = z*scale;
			vectorList.push_back(tempVector);
		}
		else if(line[0] == 'f'){
			//if line starts with f
			//ModelTriangle tempTriangle;

			std::istringstream iss(line);
			char slash; 
			int ind1, ind2, ind3;
			iss.ignore(2); // Ignore 'f ' prefix
            iss >> ind1 >> slash >> ind2 >> slash >> ind3;

			Colour triangleColour(255, 0, 0);//red as defukt

			glm::vec3 v1 = vectorList[ind1 - 1];
			glm::vec3 v2 = vectorList[ind2 - 1];
			glm::vec3 v3 = vectorList[ind3 - 1];

			ModelTriangle tempTriangle(v1, v2, v3, triangleColour);
			triangles.push_back(tempTriangle);

		}
		else{continue;}
	}
	// For each line in the file:
	// If line starts with "v":
	// 	Parse x, y, z
	// 	Scale x, y, z using scalingFactor
	// 	Append new Vector3(x, y, z) to vertices
	
	// Else If line starts with "f":
	// 	Parse v1, v2, v3
	// 	Convert indices to 0-based
	// 	Retrieve vertices using v1, v2, v3
	// 	Create a new ModelTriangle using these vertices
	// 	Append the triangle to triangles
    
    // Return triangles

	file.close();
	return triangles;
}

std::vector<ModelTriangle> OBJFileLoader(std::string filename, float scale){
	
}

int main(int argc, char *argv[])
{
	//DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	//SDL_Event event;

	std::string objpath = "/home/katie/CGlabs/CG2024/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.obj";
    std::string materialpath = "/home/katie/CGlabs/CG2024/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.mtl";
	float scalingFactor = 0.35;
	std::cout << "First print" << std::endl;
	std::vector<ModelTriangle> triangles = OBJFileLoader(objpath, scalingFactor);
	for (const auto& triangle : triangles) {
        std::cout << triangle << std::endl;
    }

    return 0;

		// We MUST poll for events - otherwise the window will freeze !
		//if (window.pollForInputEvents(event))
		//	handleEvent(event, window);

		//window.renderFrame();
}
