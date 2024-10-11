#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>//Week 2
#include <CanvasPoint.h>//Week 3
#include <Colour.h>
#include <algorithm>
#include <CanvasTriangle.h>
#include <random>

#define WIDTH 320
#define HEIGHT 240



void draw(DrawingWindow &window, std::vector<float> &grey) {
	window.clearPixels();
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			uint32_t colour = (255 << 24) + (int(grey[x]) << 16) + (int(grey[x]) << 8) + int(grey[x]);//this gets grey scale in task 3
			window.setPixelColour(x, y, colour);
		}
	}
}

//Task 2 week 2
std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues){
	float addition = (to - from)/(numberOfValues - 1);
	std::vector<float> vector_number;
	for (int i = 0; i < numberOfValues; i++){
		vector_number.push_back(from + i*addition);
	}
	return vector_number;
}

//Task 4 week 2
std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues){
	float addx = (to.x - from.x)/(numberOfValues - 1);
	float addy = (to.y - from.y)/(numberOfValues - 1);
	float addz = (to.z - from.z)/(numberOfValues - 1);
	std::vector<glm::vec3> listOfVec3;

	for (int i = 0; i < numberOfValues; i++){
		float xlist = from.x + i*addx;
		float ylist = from.y + i*addy;
		float zlist = from.z + i*addz;
		glm::vec3 storVec = {xlist, ylist, zlist};
		listOfVec3.push_back(storVec);
	}

	return listOfVec3;
}

//Task 5 week 2
void draw2(DrawingWindow &window, std::vector<glm::vec3> leftside, std::vector<glm::vec3> rightside){
	window.clearPixels();
	std::vector<glm::vec3> pixelRow;
	for (size_t y = 0; y < window.height; y++) {
		pixelRow = interpolateThreeElementValues((leftside[y]), (rightside[y]), window.width);
		for (size_t x = 0; x<window.width;x++){
			uint32_t colour = (255 << 24) + (int(pixelRow[x].x) << 16) + (int(pixelRow[x].y) << 8) + int(pixelRow[x].z);
			window.setPixelColour(x, y, colour);
		}
	}
}

uint32_t colourToUint32(const Colour &colour) {
    return (static_cast<uint32_t>(colour.red) << 16) | 
           (static_cast<uint32_t>(colour.green) << 8) | 
           (static_cast<uint32_t>(colour.blue));
}

Colour randColourGen(){
	std::random_device randC;
	std::mt19937 gen(randC());
	std::uniform_int_distribution<> distRGB(0, 255);
	int random_R = distRGB(gen);
	int random_G = distRGB(gen);
	int random_B = distRGB(gen);
	Colour randColour =  Colour(random_R, random_G, random_B);
	return randColour;
}

void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour){
	float diffX = to.x - from.x;
	float diffY = to.y - from.y;
	float numberOfSteps = std::max(abs(diffX), abs(diffY));
	float xStepSize = diffX/numberOfSteps;
	float yStepSize = diffY/numberOfSteps;

	for(float i = 0.0; i <=numberOfSteps; i++){
		float x = from.x + (xStepSize*i);
		float y = from.y + (yStepSize*i);
		window.setPixelColour(round(x), round(y), colourToUint32(colour));
	}
}

void drawTriangle(DrawingWindow &window, CanvasTriangle t, Colour colour){
	CanvasPoint a = t[0];
	CanvasPoint b = t[1];
	CanvasPoint c = t[2];
	drawLine(window, a, b, colour);
	drawLine(window, b, c, colour);
	drawLine(window, a, c, colour);
}

CanvasTriangle randTriangle(DrawingWindow &window){
	std::random_device rand;
	std::mt19937 gen(rand());
	std::uniform_int_distribution<> distX(0, window.width-1);
	std::uniform_int_distribution<> distY(0, window.height-1);
	CanvasTriangle tri;

	for (int i = 0; i < 3; i++) {
		int random_X = distX(gen);
		int random_Y = distY(gen);
		tri[i] = CanvasPoint(random_X, random_Y);
	}
	return tri;
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
		else if(event.key.keysym.sym == SDLK_u){
			CanvasTriangle tri = randTriangle(window);
			Colour randColour = randColourGen();
			drawTriangle(window, tri, randColour);
		}
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	while(true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
	
		window.renderFrame();
	}
}
