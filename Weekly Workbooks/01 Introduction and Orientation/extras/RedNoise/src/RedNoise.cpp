#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>

#define WIDTH 320
#define HEIGHT 240



void draw(DrawingWindow &window, std::vector<float> &grey) {
	window.clearPixels();
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			//float red = rand() % 256;
			//float green = 0.0;
			//float blue = 0.0;
			uint32_t colour = (255 << 24) + (int(grey[x]) << 16) + (int(grey[x]) << 8) + int(grey[x]);//this gets grey scale in task 3
			//uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
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

int main(int argc, char *argv[]) {
	//std::vector<float> result;
	//result = interpolateSingleFloats(2.2, 8.5, 7);
	//for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
	//std::cout << std::endl;

	//grey = interpolateSingleFloats(256, 0, window.width)
	//task 3
	std::vector<float> grey = interpolateSingleFloats(256, 0, window.width);


	glm::vec3 from(1.0, 4.0, 9.2);
	glm::vec3 to(4.0, 1.0, 9.8);
	std::vector<glm::vec3> result;
	result = interpolateThreeElementValues((from), (to), 4);
	for (const auto& vec : result) {
        std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
    }
	
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	//task 3
	//std::vector<float> grey = interpolateSingleFloats(256, 0, window.width);

	while(true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		draw(window, grey);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
