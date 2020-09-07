#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
constexpr int BOX_WIDTH = 20;
constexpr int BOX_HEIGHT = 20;

// Function declarations
bool init();
bool check_collision(SDL_Rect* a, SDL_Rect* b);
void close();

// Globals
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

bool init() {	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}

	if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		std::cout << "Warning: Linear texture filtering not enabled!" << std::endl;
	}
	
	gWindow = SDL_CreateWindow("Rect Collision", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == nullptr) {
		std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return  false;
	}

	// Adding VSync to avoid absurd framerates
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == nullptr) {	
		std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return  false;
	}

	// Set renderer draw/clear color
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
			
	return true;
}

bool check_collision(SDL_Rect* a, SDL_Rect* b) {
	// Check vertical overlap
	if (a->y + a->h <= b->y)
		return false;
	if (a->y >= b->y + b->h)
		return false;

	// Check horizontal overlap
	if (a->x >= b->x + b->w)
		return false;
	if (a->x + a->w <= b->x)
		return false;

	// Must overlap in both
	return true;
}

void close() {
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;

	// Quit SDL subsystems
	SDL_Quit();
}

int main() {
	if (!init()) {
		std::cout <<  "Failed to initialize!" << std::endl;
		close();
		return 1;
	}
	
	// Moving box
	SDL_Rect moveBox = {SCREEN_WIDTH/2  - BOX_WIDTH/2, SCREEN_HEIGHT/2 - BOX_HEIGHT/2, BOX_WIDTH, BOX_HEIGHT};	
	int x_vel = 0;
	int y_vel = 0;
	int x_deltav = 0;
	int y_deltav = 0;

	// Static box
	SDL_Rect staticBox = {SCREEN_WIDTH/2 + (2 * BOX_WIDTH), SCREEN_HEIGHT/2 - BOX_HEIGHT/2, BOX_WIDTH, BOX_HEIGHT};

	SDL_Event e;
	bool gameon = true;
	while(gameon) {
		while(SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				gameon = false;
			}
		}

		x_deltav = 0;
		y_deltav = 0;
		const Uint8* keystate = SDL_GetKeyboardState(nullptr);
		if (keystate[SDL_SCANCODE_W])
			y_deltav -= 1;
		if (keystate[SDL_SCANCODE_A])
			x_deltav -= 1;
		if (keystate[SDL_SCANCODE_S])
			y_deltav += 1;
		if (keystate[SDL_SCANCODE_D])
			x_deltav += 1;

		if (x_deltav == 0) {
			if (x_vel > 0)
				x_deltav = -1;
			else if (x_vel < 0)
				x_deltav = 1;
		}
		if (y_deltav == 0) {
			if (y_vel > 0)
				y_deltav = -1;
			else if (y_vel < 0)
				y_deltav = 1;
		}
		
		x_vel += x_deltav;
		y_vel += y_deltav;

		if (x_vel < -5)
			x_vel = -5;
		else if (x_vel > 5)
			x_vel = 5;

		if (y_vel < -5)
			y_vel = -5;
		else if (y_vel > 5)
			y_vel = 5;

		// Try to move vertically
		moveBox.y += y_vel;
		if (moveBox.y < 0 || (moveBox.y + BOX_HEIGHT > SCREEN_HEIGHT) || check_collision(&staticBox, &moveBox))
			// Can't move, go back
			moveBox.y -= y_vel;

		// Try to move horizontally
		moveBox.x += x_vel;
		if (moveBox.x < 0 || (moveBox.x + BOX_WIDTH > SCREEN_WIDTH) || check_collision(&staticBox, &moveBox))
			moveBox.x -= x_vel;
	
		// Clear black
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);
		
		// Cyan move box
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0xFF, 0xFF);
		SDL_RenderFillRect(gRenderer, &moveBox);
		
		// Red static box
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderFillRect(gRenderer, &staticBox);

		SDL_RenderPresent(gRenderer);
	}

	// Out of game loop, clean up
	close();
}
