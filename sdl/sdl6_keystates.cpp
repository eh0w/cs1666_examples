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
	
	gWindow = SDL_CreateWindow("Hello world!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
	
	// Current position to render the box
	// Start off with it in the middle
	int x_pos = SCREEN_WIDTH/2 - BOX_WIDTH/2;
	int y_pos = SCREEN_HEIGHT/2 - BOX_HEIGHT/2;

	// Current velocity of the box
	// Start off at reset
	int x_vel = 0;
	int y_vel = 0;

	// Change to velocity
	int x_deltav = 0;
	int y_deltav = 0;

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

		// Get array of current key states
		// Note that array is indexed by scancodes, not keycodes
		// https://wiki.libsdl.org/SDL_Scancode
		const Uint8* keystate = SDL_GetKeyboardState(nullptr);
		if (keystate[SDL_SCANCODE_W])
			y_deltav -= 1;
		if (keystate[SDL_SCANCODE_A])
			x_deltav -= 1;
		if (keystate[SDL_SCANCODE_S])
			y_deltav += 1;
		if (keystate[SDL_SCANCODE_D])
			x_deltav += 1;

		// If we don't want our dot to be in a frictionless vacuum...
		if (x_deltav == 0) {
			// No user-supplied "push", return to rest
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
		
		// Speed up/slow down
		x_vel += x_deltav;
		y_vel += y_deltav;

		// Check speed limits
		if (x_vel < -5)
			x_vel = -5;
		else if (x_vel > 5)
			x_vel = 5;

		if (y_vel < -5)
			y_vel = -5;
		else if (y_vel > 5)
			y_vel = 5;

		// Move box
		x_pos += x_vel;
		y_pos += y_vel;

		// Check you haven't moved off the screen
		if (x_pos < 0)
			x_pos = 0;
		else if (x_pos + BOX_WIDTH > SCREEN_WIDTH)
			x_pos = SCREEN_WIDTH - BOX_WIDTH;

		if (y_pos < 0)
			y_pos = 0;
		else if (y_pos + BOX_HEIGHT > SCREEN_HEIGHT)
			y_pos = SCREEN_HEIGHT - BOX_HEIGHT;
		
		// Draw box
		// Clear black
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);
		// Cyan box
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0xFF, 0xFF);
		SDL_Rect fillRect = {x_pos, y_pos, BOX_WIDTH, BOX_HEIGHT};
		SDL_RenderFillRect(gRenderer, &fillRect);
		SDL_RenderPresent(gRenderer);
	}

	// Out of game loop, clean up
	close();
}
