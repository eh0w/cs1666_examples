#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
// Need to import the SDL threading library
#include <SDL_thread.h>

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
constexpr int TILE_SIZE = 100;
constexpr int BUFFER_SIZE = 100;
// Producing/consuming 10 million items
constexpr int PRODUCTION = 10000000;

// Function declarations
bool init();
SDL_Texture* loadImage(std::string fname);
void close();

// Globals
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Texture* gBG = nullptr;
std::vector<SDL_Rect*> gPos;
SDL_TimerID gTimerId;
SDL_Texture* gBirdSheet = nullptr;

// Production targets
int gToProduce = PRODUCTION;
int gToConsume = PRODUCTION;

// Semaphores to control access to the gPos buffer
SDL_sem* gEmpty = nullptr;
SDL_sem* gFull = nullptr;
SDL_sem* gMutex = nullptr;

// For the bird
//SDL_Rect gBirdRect = {0, 0, TILE_SIZE, TILE_SIZE};
// For the plane
SDL_Rect gBirdRect = {TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
// For the UFO
//SDL_Rect gBirdRect = {0, TILE_SIZE, TILE_SIZE, TILE_SIZE};
// For the helicopter
//SDL_Rect gBirdRect = {TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE};

bool init() {	
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

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
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

	gEmpty = SDL_CreateSemaphore(BUFFER_SIZE);
	gFull = SDL_CreateSemaphore(0);
	gMutex = SDL_CreateSemaphore(1);
	if (gEmpty == nullptr || gFull == nullptr || gMutex == nullptr) {
		std::cout << "A semaphore could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return  false;
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
	// Be sure to destroy semaphores!
	SDL_DestroySemaphore(gEmpty);
	SDL_DestroySemaphore(gFull);
	SDL_DestroySemaphore(gMutex);
	gEmpty = nullptr;
	gFull = nullptr;
	gMutex = nullptr;

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

// Signature for a function being called as an SDL_Thread
//int myThreadFunc(void* data);

int myProducerFunc(void* data) {
	int in = 0;
	SDL_Rect* new_rect = nullptr;

	bool loopin = true;
	while (loopin) {
		// Produce next item
		new_rect = new SDL_Rect;
		new_rect->x = rand() % (SCREEN_WIDTH - TILE_SIZE);
		new_rect->y = rand() % (SCREEN_HEIGHT - TILE_SIZE);
		new_rect->h = TILE_SIZE;
		new_rect->w = TILE_SIZE;

		// Attempt to enter critical region
		
		// wait, aka: down, P, proberen
		SDL_SemWait(gEmpty);
		SDL_SemWait(gMutex);

		// Now in the critical region
		
		// Place new item in buffer
		gPos[in] = new_rect;
		in = (in + 1) % BUFFER_SIZE;
		new_rect = nullptr;

		// If we've produced all we should, exit
		gToProduce--;
		if (gToProduce <= 0)
			loopin = false;
		
		// About to exit the critical region

		// post, aka: up, signal, V, verhogen
		SDL_SemPost(gMutex);
		SDL_SemPost(gFull);

		// Outside of the critical region
	}

	// Why add another critical region?
	SDL_SemWait(gMutex);
	std::cout << "Producer finished" << std::endl;
	SDL_SemPost(gMutex);
	return 0;
}

int myConsumerFunc(void* data) {
	int out = 0;
	SDL_Rect* next_rect = nullptr;

	bool loopin = true;
	while (loopin) {
		// Attmept to enter critical region

		SDL_SemWait(gFull);
		SDL_SemWait(gMutex);

		// Now in the critical region

		// Grab next item from the buffer
		next_rect = gPos[out];
		gPos[out] = nullptr;
		out = (out + 1) % BUFFER_SIZE;

		gToConsume--;
		if (gToConsume <= 0)
			loopin = false;

		// About to exit the critical region

		SDL_SemPost(gMutex);
		SDL_SemPost(gEmpty);

		// Outside of the critical region

		// Consume the current item
		delete next_rect;
		next_rect = nullptr;
	}

	SDL_SemWait(gMutex);
	std::cout << "Consumer finished" << std::endl;
	SDL_SemPost(gMutex);
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

	// Initialize the buffer to contain BUFFER_SIZE nullptr SDL_Rect pointers
	gPos.resize(BUFFER_SIZE);
	for (auto i : gPos) {
		i = nullptr;
	}

	// Start producer and consumer
	SDL_Thread* prod = SDL_CreateThread(myProducerFunc, "Producer", nullptr);
	SDL_Thread* cons = SDL_CreateThread(myConsumerFunc, "Consumer", nullptr);

	// Keep track of fps
	int frame_count = 0;
	Uint32 start_time = SDL_GetTicks();
	Uint32 cur_time = 0;

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
	
		// Need to acquire mutex to access gPos
		SDL_SemWait(gMutex);
		if (gToProduce <= 0 && gToConsume <= 0)
			gameon = false;
		else {
			for (auto p : gPos) {
				if (p != nullptr)
					SDL_RenderCopy(gRenderer, gBirdSheet, &gBirdRect, p);
			}
		}
		SDL_SemPost(gMutex);

		SDL_RenderPresent(gRenderer);

		frame_count++;
	}

	cur_time = SDL_GetTicks();
	std::cout << "Took " << ((cur_time - start_time) / 1000.0) << "s to finish" << std::endl;
	std::cout << "Average FPS:  " << frame_count / ((cur_time - start_time) / 1000.0) << std::endl;
	
	SDL_WaitThread(prod, nullptr);
	SDL_WaitThread(cons, nullptr);
	
	close();
}
