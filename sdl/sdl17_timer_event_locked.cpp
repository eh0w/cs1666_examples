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
SDL_Texture* gBG = nullptr;
std::vector<SDL_Rect> gPos;
SDL_TimerID gTimerId;
SDL_Texture* gBirdSheet = nullptr;
SDL_SpinLock gLock = 0;

// For the bird
//SDL_Rect gBirdRect = {0, 0, TILE_SIZE, TILE_SIZE};
// For the plane
//SDL_Rect gBirdRect = {TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
// For the UFO
SDL_Rect gBirdRect = {0, TILE_SIZE, TILE_SIZE, TILE_SIZE};
// For the helicopter
//SDL_Rect gBirdRect = {TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE};

bool init() {	
	// Need to init timer to use timer events
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}

	if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		std::cout << "Warning: Linear texture filtering not enabled!" << std::endl;
	}
		
	gWindow = SDL_CreateWindow("Time", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == nullptr) {
		std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return  false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == nullptr) {	
		std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return  false;
	}

	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
			
	int imgFlags = IMG_INIT_PNG;
	int retFlags = IMG_Init(imgFlags);
	if(retFlags != imgFlags) {
		std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		return false;
	}
	
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
	SDL_RemoveTimer(gTimerId);

	SDL_DestroyTexture(gBG);
	gBG = nullptr;

	SDL_DestroyTexture(gBirdSheet);
	gBirdSheet = nullptr;

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;

	IMG_Quit();
	SDL_Quit();
}

Uint32 myCallbackFunc(Uint32 interval, void* param) {
	// Note, we don't need a param here...
	int new_x = rand() % (SCREEN_WIDTH - TILE_SIZE);
	int new_y = rand() % (SCREEN_HEIGHT - TILE_SIZE);
	SDL_Rect new_rect = {new_x, new_y, TILE_SIZE, TILE_SIZE};

	// Lock before accessing shared data
	SDL_AtomicLock(&gLock);
	
	gPos.push_back(new_rect);
	
	// Unlock when done
	SDL_AtomicUnlock(&gLock);

	gTimerId = SDL_AddTimer(interval, myCallbackFunc, nullptr);

	return 0;
}

int main() {
	if (!init()) {
		std::cout <<  "Failed to initialize!" << std::endl;
		close();
		return 1;
	}

	gBG = loadImage("images/small_bg.png");
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

	gBirdSheet = loadImage("images/birds.png");

	// Start timer
	gTimerId = SDL_AddTimer(3000, myCallbackFunc, nullptr);

	SDL_Event e;
	bool gameon = true;
	while(gameon) {
		while(SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				gameon = false;
			}
		}

		SDL_RenderClear(gRenderer);
		SDL_RenderCopy(gRenderer, gBG, NULL, NULL);
		
		// Lock before accessing shared data
		SDL_AtomicLock(&gLock);

		// Note that this is not a great use of spinlock, as RenderCopy is relatively expensive
		// Should minimize the amout of work done while holding a spinlock
		for (auto p : gPos) {
			SDL_RenderCopy(gRenderer, gBirdSheet, &gBirdRect, &p);
		}

		// Unlock when done
		SDL_AtomicUnlock(&gLock);

		SDL_RenderPresent(gRenderer);
	}

	close();
}
