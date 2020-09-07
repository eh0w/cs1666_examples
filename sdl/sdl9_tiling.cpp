#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
constexpr int TILE_SIZE = 100;

// Function declarations
bool init();
SDL_Texture* loadImage(std::string fname);
void close();

// Globals
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Texture* gBrickSheet;
SDL_Texture* gBirdSheet;
SDL_Rect gBrickRects[4];
SDL_Rect gBirdRects[4];

bool init() {	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}

	if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		std::cout << "Warning: Linear texture filtering not enabled!" << std::endl;
	}
	
	gWindow = SDL_CreateWindow("Tiling", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

	SDL_DestroyTexture(gBrickSheet);
	gBrickSheet = nullptr;

	SDL_DestroyTexture(gBirdSheet);
	gBirdSheet = nullptr;

	// Quit SDL subsystems
	SDL_Quit();
}

int main() {
	if (!init()) {
		std::cout <<  "Failed to initialize!" << std::endl;
		close();
		return 1;
	}
	
	gBrickSheet = loadImage("images/bricks.png");
	
	for (int i = 0; i < 4; i++) {
		gBrickRects[i].x = i * TILE_SIZE;
		gBrickRects[i].y = 0;
		gBrickRects[i].w = TILE_SIZE;
		gBrickRects[i].h = TILE_SIZE;
	}

	gBirdSheet = loadImage("images/birds.png");

	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			gBirdRects[x + 2*y].x = x * TILE_SIZE;
			gBirdRects[x + 2*y].y = y * TILE_SIZE;
			gBirdRects[x + 2*y].w = TILE_SIZE;
			gBirdRects[x + 2*y].h = TILE_SIZE;
		}
	}

	SDL_Rect birdOuts[8];
	birdOuts[0] = {10, 20, TILE_SIZE, TILE_SIZE};
	birdOuts[1] = {20, 200, TILE_SIZE, TILE_SIZE};
	birdOuts[2] = {500, 100, TILE_SIZE, TILE_SIZE};
	birdOuts[3] = {350, 20, TILE_SIZE, TILE_SIZE};
	birdOuts[4] = {100, 300, TILE_SIZE, TILE_SIZE};
	birdOuts[5] = {250, 150, TILE_SIZE, TILE_SIZE};
	birdOuts[6] = {100, 20, TILE_SIZE, TILE_SIZE};
	birdOuts[7] = {450, 250, TILE_SIZE, TILE_SIZE};

	SDL_Event e;
	bool gameon = true;
	int c;
	SDL_Rect cur_out;
	while(gameon) {
		while(SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				gameon = false;
			}
		}
		
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);
		
		c = 0;
		while (c * TILE_SIZE < SCREEN_WIDTH) {
			cur_out = {c * TILE_SIZE, SCREEN_HEIGHT - TILE_SIZE, TILE_SIZE, TILE_SIZE};
			SDL_RenderCopy(gRenderer, gBrickSheet, &gBrickRects[c % 4], &cur_out);
			c += 1;
		}
		
		for (int i = 0; i < 8; i++) {
			SDL_RenderCopy(gRenderer, gBirdSheet, &gBirdRects[i % 4], &birdOuts[i]);
		}
		

		SDL_RenderPresent(gRenderer);
	}

	close();
}
