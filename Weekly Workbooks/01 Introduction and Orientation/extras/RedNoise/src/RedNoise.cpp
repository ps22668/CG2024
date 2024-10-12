#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>	 //Week 2
#include <CanvasPoint.h> //Week 3
#include <Colour.h>
#include <algorithm>
#include <CanvasTriangle.h>
#include <random>

#define WIDTH 320
#define HEIGHT 240

void draw(DrawingWindow &window, std::vector<float> &grey)
{
	window.clearPixels();
	for (size_t y = 0; y < window.height; y++)
	{
		for (size_t x = 0; x < window.width; x++)
		{
			uint32_t colour = (255 << 24) + (int(grey[x]) << 16) + (int(grey[x]) << 8) + int(grey[x]); // this gets grey scale in task 3
			window.setPixelColour(x, y, colour);
		}
	}
}

// Task 2 week 2
std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues)
{
	float addition = (to - from) / (numberOfValues - 1);
	std::vector<float> vector_number;
	for (int i = 0; i < numberOfValues; i++)
	{
		vector_number.push_back(from + i * addition);
	}
	return vector_number;
}

// Task 4 week 2
std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues)
{
	float addx = (to.x - from.x) / (numberOfValues - 1);
	float addy = (to.y - from.y) / (numberOfValues - 1);
	float addz = (to.z - from.z) / (numberOfValues - 1);
	std::vector<glm::vec3> listOfVec3;

	for (int i = 0; i < numberOfValues; i++)
	{
		float xlist = from.x + i * addx;
		float ylist = from.y + i * addy;
		float zlist = from.z + i * addz;
		glm::vec3 storVec = {xlist, ylist, zlist};
		listOfVec3.push_back(storVec);
	}

	return listOfVec3;
}

// Task 5 week 2
void draw2(DrawingWindow &window, std::vector<glm::vec3> leftside, std::vector<glm::vec3> rightside)
{
	window.clearPixels();
	std::vector<glm::vec3> pixelRow;
	for (size_t y = 0; y < window.height; y++)
	{
		pixelRow = interpolateThreeElementValues((leftside[y]), (rightside[y]), window.width);
		for (size_t x = 0; x < window.width; x++)
		{
			uint32_t colour = (255 << 24) + (int(pixelRow[x].x) << 16) + (int(pixelRow[x].y) << 8) + int(pixelRow[x].z);
			window.setPixelColour(x, y, colour);
		}
	}
}

uint32_t colourToUint32(const Colour &colour)
{
	return (static_cast<uint32_t>(colour.red) << 16) |
		   (static_cast<uint32_t>(colour.green) << 8) |
		   (static_cast<uint32_t>(colour.blue));
}

Colour randColourGen()
{
	std::random_device randC;
	std::mt19937 gen(randC());
	std::uniform_int_distribution<> distRGB(0, 255);
	int random_R = distRGB(gen);
	int random_G = distRGB(gen);
	int random_B = distRGB(gen);
	Colour randColour = Colour(random_R, random_G, random_B);
	return randColour;
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

CanvasTriangle randTriangle(DrawingWindow &window)
{
	std::random_device rand;
	std::mt19937 gen(rand());
	std::uniform_int_distribution<> distX(0, window.width - 1);
	std::uniform_int_distribution<> distY(0, window.height - 1);
	CanvasTriangle tri;

	for (int i = 0; i < 3; i++)
	{
		int random_X = distX(gen);
		int random_Y = distY(gen);
		tri[i] = CanvasPoint(random_X, random_Y);
	}
	return tri;
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
		else if (event.key.keysym.sym == SDLK_u)
		{
			CanvasTriangle tri = randTriangle(window);
			Colour randColour = randColourGen();
			drawStroked(window, tri, randColour);
		}
		else if (event.key.keysym.sym == SDLK_f)
		{
			Colour white(255, 255, 255);
			CanvasTriangle tri = randTriangle(window);
			Colour randColour = randColourGen();
			drawFilled(window, tri, randColour);
			drawStroked(window, tri, white);
		}
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[])
{
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	while (true)
	{
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event))
			handleEvent(event, window);

		window.renderFrame();
	}
}
