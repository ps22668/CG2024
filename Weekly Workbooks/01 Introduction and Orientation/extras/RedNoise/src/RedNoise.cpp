#include <vector>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <glm/glm.hpp>
#include <Colour.h>
#include <algorithm>
#include <random>
#include <string>
#include <iostream>
#include <ModelTriangle.h>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <CanvasPoint.h>
#include <CanvasTriangle.h>

#define WIDTH 320
#define HEIGHT 240

uint32_t colourToUint32(const Colour &colour)
{
	return (static_cast<uint32_t>(colour.red) << 16) |
		   (static_cast<uint32_t>(colour.green) << 8) |
		   (static_cast<uint32_t>(colour.blue));
}

void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour)
{
	float diffX = to.x - from.x;
	float diffY = to.y - from.y;
	float numberOfSteps = std::max(abs(diffX), abs(diffY));
	float xStepSize = diffX / numberOfSteps;
	float yStepSize = diffY / numberOfSteps;

	for (float i = 0.0; i <= numberOfSteps; i++)
	{
		float x = from.x + (xStepSize * i);
		float y = from.y + (yStepSize * i);
		window.setPixelColour(round(x), round(y), colourToUint32(colour));
	}
}

std::vector<CanvasPoint> drawLineArray(CanvasPoint from, CanvasPoint to, Colour colour)
{
	std::vector<CanvasPoint> pointsToDraw;
	float diffX = to.x - from.x;
	float diffY = to.y - from.y;
	float numberOfSteps = std::max(abs(diffX), abs(diffY));
	float xStepSize = diffX / numberOfSteps;
	float yStepSize = diffY / numberOfSteps;

	for (float i = 0.0; i <= numberOfSteps; i++)
	{
		float x = from.x + (xStepSize * i);
		float y = from.y + (yStepSize * i);
		CanvasPoint point = CanvasPoint(round(x),round(y));
		pointsToDraw.push_back(point);
		//window.setPixelColour(round(x), round(y), colourToUint32(colour));
	}
	return pointsToDraw;
}


float interpolationX(CanvasPoint from, CanvasPoint to, float currentY)
{
	float diffX = to.x - from.x;
	float diffY = to.y - from.y;
	float changeY = (currentY - from.y)/(diffY);
	float currentX = from.x + (diffX * changeY);
	return currentX;
}



void drawStroked(DrawingWindow &window, CanvasTriangle t, Colour colour)
{
	CanvasPoint a = t[0];
	CanvasPoint b = t[1];
	CanvasPoint c = t[2];
	drawLine(window, a, b, colour);
	drawLine(window, b, c, colour);
	drawLine(window, a, c, colour);
}

void drawFilled(DrawingWindow &window, CanvasTriangle t, Colour colour)
{
	// sort the cavaspoints by the y value (loweest top, highest bottom)
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2 - i; j++)
		{
			if (CanvasPoint(t[j]).y > CanvasPoint(t[j + 1]).y)
			{
				CanvasPoint temp = t[j];
				t[j] = t[j + 1];
				t[j+1] = temp;
			}
		}
	}
	CanvasPoint a = t[0]; // lower y near the top
	CanvasPoint b = t[1];
	CanvasPoint c = t[2]; // highest y at the bottom

	//find extra point
	CanvasPoint extra;
	std::vector<CanvasPoint> a2c = drawLineArray(a, c, colour);
	for (const auto point : a2c) {
        if (point.y == b.y) {
            extra = point;
			break;
        }
    }
	//Fill top triangle
	for (float y = a.y; y < b.y; y++){
		float x1 = interpolationX(a, extra, y);
		float x2 = interpolationX(a, b, y);
	
		if(x1 > x2){
			for (float x = round(x2); x <= round(x1); x++){
				window.setPixelColour(round(x), round(y), colourToUint32(colour));
				
			}
		}
		if(x2 >= x1){
			for (float x = round(x1); x <= round(x2); x++){
				window.setPixelColour(round(x), round(y), colourToUint32(colour));
			}
		}
	}
	//fill bottom triangle
	for (float y = b.y; y < c.y; y++){
		float x1 = interpolationX(extra, c, y);
		float x2 = interpolationX(b, c, y);
	
		if(x1 > x2){
			for (float x = round(x2); x <= round(x1); x++){
				window.setPixelColour(round(x), round(y), colourToUint32(colour));	
			}
		}
		if(x2 >= x1){
			for (float x = round(x1); x <= round(x2); x++){
				window.setPixelColour(round(x), round(y), colourToUint32(colour));
			}
		}
	}
}


CanvasPoint projectVertexOntoCanvasPoint(DrawingWindow &window, glm::vec3 cameraPosition, float focalLength, glm::vec3 vertexPostion){
	glm::vec3 relativePosition = vertexPostion - cameraPosition;
	float x = relativePosition.x;
	float y = relativePosition.y;
	float z = relativePosition.z;

	float u = (focalLength*(x/z)) + (window.width/2);
	float v = (focalLength*(y/z)) + (window.height/2);
	// float u = focalLength * (x / z);
    // float v = focalLength * (y / z);

    // // Map to screen space
    // float screenU = planeWidth / 2.0 + u;
    // float screenV = planeHeight / 2.0 - v;

	return CanvasPoint(u, v);
}


Colour findColour(std::string name, std::unordered_map<std::string, Colour> palette){
	if (palette.find(name) != palette.end()) {
		Colour materialColour = palette[name];
		//std::cout << "Found material: " << name << std::endl;
		return materialColour;
	} else {
		throw std::runtime_error("Material not found: " + name);
	}
}

std::vector<ModelTriangle> OBJFileLoader(std::string filename, float scale, std::unordered_map<std::string, Colour> palette){
	std::vector<glm::vec3> vectorList;
	std::vector<ModelTriangle> triangles;
	std::string line;
	std::ifstream file(filename);
	if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        //return 1; // Exit with an error code
    }
	Colour triColour;
	while (std::getline (file, line)) {
    // Output the text from the file
		//cout << text;
		//if line starts with v
		if (line.empty()) {continue;}
		else if (line[0] == 'u'){
			std::istringstream iss(line);
			std::string type = "usemtl";  
			std::string colourType;
			iss >> type >> colourType;

			
			triColour = findColour(colourType, palette);
		}
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

			Colour triangleColour = triColour;

			glm::vec3 v1 = vectorList[ind1 - 1];
			glm::vec3 v2 = vectorList[ind2 - 1];
			glm::vec3 v3 = vectorList[ind3 - 1];

			ModelTriangle tempTriangle(v1, v2, v3, triangleColour);
			triangles.push_back(tempTriangle);
			Colour triColour;

		}
		else{continue;}
	}
	file.close();
	return triangles;
}

std::unordered_map<std::string, Colour> materialFileLoader(std::string filename){
	std::unordered_map<std::string, Colour> palette;
	std::string line;
	std::ifstream file(filename);
	if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
    }
	std::string newMaterialName;
	while (std::getline (file, line)) {
		if (line.empty()) {continue;}
		else if (line[0] == 'n'){
			std::string materialName;
			std::istringstream iss(line);
			std::string type = "newmtl";  
			iss>>type>>materialName;
			newMaterialName = materialName;
		}
		else if (line[0] == 'K'){
			std::istringstream iss(line);
			std::string type = "Kd"; 
			float r, g, b;
			
			iss>>type>>r>>g>>b;
			
			Colour newColour = Colour((r*255), (g*255), (b*255));
			palette[newMaterialName] = newColour;
			std::string newMaterialName;
		}
		else{continue;}
	}
	file.close();
	return palette;
}

void handleEvent(SDL_Event event, DrawingWindow &window)
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_LEFT)
			std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT)
			std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP)
			std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN)
			std::cout << "DOWN" << std::endl;
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

void renderPointCloud(DrawingWindow &window, glm::vec3 cameraPosition, float focalLength, std::vector<ModelTriangle> triangles, float imagePlaneScaling){
	window.clearPixels();
	// float scaleX = window.width / planeWidth;
    // float scaleY = window.height / planeHeight;
	float scaleX = window.width / 200;
    float scaleY = window.height / 150;

	for (ModelTriangle triangle : triangles){
		
		CanvasPoint vertexProjA = projectVertexOntoCanvasPoint(window, cameraPosition, focalLength, triangle.vertices[0]);
		CanvasPoint vertexProjB = projectVertexOntoCanvasPoint(window, cameraPosition, focalLength, triangle.vertices[1]);
		CanvasPoint vertexProjC = projectVertexOntoCanvasPoint(window, cameraPosition, focalLength, triangle.vertices[2]);
		Colour filledColour = triangle.colour;
		// std::cout << "FilledColour" << filledColour << std::endl;
		//float screenX = round(window.width - (vertexProjected.x * scaleX));
		//upside down when (window.height - vertexProjected.y) * scaleY)
		//Right side up but mirroed when its (0 + vertexProjected.y)
		//float screenY = round((vertexProjected.y * scaleY));
		float screenX1 = round(window.width - (vertexProjA.x * scaleX));
		float screenY1 = round((vertexProjA.y * scaleY));

		float screenX2 = round(window.width - (vertexProjB.x * scaleX));
		float screenY2 = round((vertexProjB.y * scaleY));

		float screenX3 = round(window.width - (vertexProjC.x * scaleX));
		float screenY3 = round((vertexProjC.y * scaleY));

		CanvasTriangle canvasTriangle(CanvasPoint(screenX1, screenY1), CanvasPoint(screenX2, screenY2), CanvasPoint(screenX3, screenY3));

		//Colo
		//std::cout << "screenX: " << screenX << ", screenY: " << screenY << std::endl;

		//Colour white = Colour(255, 255, 255);

		// if (screenX >= 0 && screenX < window.width && screenY >= 0 && screenY < window.height) {
		// 	window.setPixelColour(screenX, screenY, colourToUint32(white));
		// } 
		// else{
		// 	std::cout << "Not setting pixels" << std::endl;
		// }

		//drawStroked(window, canvasTriangle, white);
		drawFilled(window, canvasTriangle, filledColour);
	}

}

int main(int argc, char *argv[])
{
	//DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	//SDL_Event event;

	std::string objpath = "/home/katie/CGlabs/CG2024/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.obj";
    std::string materialpath = "/home/katie/CGlabs/CG2024/Weekly Workbooks/04 Wireframes and Rasterising/models/cornell-box.mtl";
	float scalingFactor = 0.35;
	//std::cout << "First print" << std::endl;
	std::unordered_map<std::string, Colour> palette = materialFileLoader(materialpath);
	std::vector<ModelTriangle> triangles = OBJFileLoader(objpath, scalingFactor, palette);
	// for (const auto& triangle : triangles) {
    //     std::cout << triangle << std::endl;
    // }

	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 4.0f);

	while (true)
	{
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		
		//void renderPointCloud(DrawingWindow &window, glm::vec3 cameraPosition, float focalLength, std::vector<glm::vec3> vertices, float imagePlaneScaling
		renderPointCloud(window, cameraPosition, 300, triangles, 160); //too big was 200 and 160

		window.renderFrame();
	}
}
