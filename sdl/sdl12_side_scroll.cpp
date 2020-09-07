#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

constexpr int CAM_WIDTH = 640;
constexpr int CAM_HEIGHT = 480;
constexpr int LEVEL_LEN = 2000;
constexpr int TILE_SIZE = 100;

// Function declarations
bool init();
SDL_Texture* loadImage(std::string fname);
void close();

// Globals
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Texture* gBackground;
SDL_Texture* gBirdSheet;
SDL_Texture* gBrickSheet;

// For the bird
//SDL_Rect gBirdRect = {0, 0, TILE_SIZE, TILE_SIZE};
// For the plane
//SDL_Rect gBirdRect = {TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
// For the UFO
//SDL_Rect gBirdRect = {0, TILE_SIZE, TILE_SIZE, TILE_SIZE};
// For the helicopter
SDL_Rect gBirdRect = {TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE};

bool init() {	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}

	if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		std::cout << "Warning: Linear texture filtering not enabled!" << std::endl;
	}
	
	gWindow = SDL_CreateWindow("Side scroll", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, CAM_WIDTH, CAM_HEIGHT, SDL_WINDOW_SHOWN);
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

	int imgFlags = IMG_INIT_PNG;
	int retFlags = IMG_Init(imgFlags);
	if(retFlags != imgFlags) {
		std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		return false;
	}

	// Set renderer draw/clear color
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
			
	return true;
}

SDL_Texture* loadImage(std::string fname) {
	SDL_Texture* newText = nullptr;

	SDL_Surface* startSurf = IMG_Load(fname.c_str());
	if (startSurf == nullptr) {
		std::cout << "Unable to load image " << fname << "! SDL Error: " << SDL_GetError() << std::endl;
		return nullptr;
	}

	newText = SDL_CreateTextureFromSurface(gRenderer, startSurf);
	if (newText == nullptr) {
		std::cout << "Unable to create texture from " << fname << "! SDL Error: " << SDL_GetError() << std::endl;
	}

	SDL_FreeSurface(startSurf);

	return newText;
}


void close() {
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;

	SDL_DestroyTexture(gBackground);
	gBackground = nullptr;

	SDL_DestroyTexture(gBirdSheet);
	gBirdSheet = nullptr;

	SDL_DestroyTexture(gBrickSheet);
	gBrickSheet = nullptr;

	// Quit SDL subsystems
	SDL_Quit();
}

int main() {
	if (!init()) {
		std::cout <<  "Failed to initialize!" << std::endl;
		close();
		return 1;
	}
	
	gBackground = loadImage("images/small_bg.png");

	int scroll_offset = 0;
	int rem = 0;
	int c = 0;

	gBirdSheet = loadImage("images/birds.png");
	gBrickSheet = loadImage("images/bricks.png");

	// Moving box
	SDL_Rect moveBox = {CAM_WIDTH/2 - TILE_SIZE/2, CAM_HEIGHT/2 - TILE_SIZE/2, TILE_SIZE, TILE_SIZE};	
	int x_vel = 0;
	int y_vel = 0;
	int x_deltav = 0;
	int y_deltav = 0;

	int lthird = (CAM_WIDTH / 3) - TILE_SIZE/2;
	int rthird = (2 * CAM_WIDTH / 3) - TILE_SIZE/2;

	SDL_Rect birdcam = {CAM_WIDTH/2 - TILE_SIZE/2, CAM_HEIGHT/2 - TILE_SIZE/2, TILE_SIZE, TILE_SIZE};
	SDL_Rect bgRect = {0, 0, CAM_WIDTH, CAM_HEIGHT};
	SDL_Rect brickcam = {0, CAM_HEIGHT - TILE_SIZE, TILE_SIZE, TILE_SIZE};
	SDL_Rect brickRects[4];
	for (int i = 0; i < 4; i++) {
		brickRects[i].x = i * TILE_SIZE;
		brickRects[i].y = 0;
		brickRects[i].w = TILE_SIZE;
		brickRects[i].h = TILE_SIZE;
	}
	

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

		// Move bird vertically
		moveBox.y += y_vel;
		// Vertical movement constrained by camera height and tiles at bottom
		if (moveBox.y < 0 || (moveBox.y + TILE_SIZE > CAM_HEIGHT - TILE_SIZE))
			moveBox.y -= y_vel;

		// Move bird horizontally
		moveBox.x += x_vel;
		// Horizontal movement constrained by length of level
		if (moveBox.x < 0 || (moveBox.x + TILE_SIZE > LEVEL_LEN))
			moveBox.x -= x_vel;
			
		// Make sure bird is in the middle of the camera
		if (moveBox.x > (scroll_offset + rthird))
			scroll_offset = moveBox.x - rthird;
		else if (moveBox.x < (scroll_offset + lthird))
			scroll_offset = moveBox.x - lthird;

		if (scroll_offset < 0)
			scroll_offset = 0;

		if (scroll_offset + CAM_WIDTH > LEVEL_LEN)
			scroll_offset = LEVEL_LEN - CAM_WIDTH;

		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);
		
		// Draw the portion of the background currently highlighted by the camera
		rem = scroll_offset % CAM_WIDTH;
		bgRect.x = -rem;
		SDL_RenderCopy(gRenderer, gBackground, nullptr, &bgRect);
		bgRect.x += CAM_WIDTH;
		SDL_RenderCopy(gRenderer, gBackground, nullptr, &bgRect);

		// Draw bricks on ground
		rem = scroll_offset % TILE_SIZE;
		brickcam.x = -rem;
		
		// Why not c = 0 here?
		c = (scroll_offset % (TILE_SIZE * 4)) / TILE_SIZE;

		while (brickcam.x < CAM_WIDTH) {
			SDL_RenderCopy(gRenderer, gBrickSheet, &brickRects[c], &brickcam);

			brickcam.x += TILE_SIZE;
			c = (c + 1) % 4;
		}

		// Draw the bird
		birdcam.x = moveBox.x - scroll_offset;
		birdcam.y = moveBox.y;
		SDL_RenderCopy(gRenderer, gBirdSheet, &gBirdRect, &birdcam);

		SDL_RenderPresent(gRenderer);
	}

	close();
}
