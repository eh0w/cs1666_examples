#include <SDL.h>
#include <iostream>

//Screen dimension constants
constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

int main() {

	SDL_Window* window = nullptr;
	SDL_Surface* screenSurface = nullptr;

	// init SDL Video
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize!  SDL_Error:  " << SDL_GetError() << std::endl;
	}
	else {
		// SDL IS GO

		/* Creating the window
		 * Arguments:
		 *   Window title
		 *   x position (we don't care in this example)
		 *   y position (we don't care in this example)
		 *   window width
		 *   window height
		 *   flags, see API for possible flags
		 */
		window = SDL_CreateWindow("Starting SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL) {
			std::cout << "Could not create window!  SDL_Error:  " << SDL_GetError() << std::endl;
		}
		else {
			// Get window surface
			// SDL_Surfaces contain pixel data for use by the software blitting (drawn by CPU)'
			screenSurface = SDL_GetWindowSurface(window);

			/* Fill Cyan
			 * Drawing an "image" that is just the color cyan to the surface of the window
			 * Note the 2nd argument (currently NULL) can be used to specify a subsection of the suface to fill
			 *   By specifying NULL, we fill the whole surface
			 * Also need to present the RGB value in the format used by the screen surface
			 *   256 color? High color? True color?
			 *   SDL_MapRGB does this for us
			 */
			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0xFF, 0xFF));

			// Update the display with the new "image" drawn to the window surface
			SDL_UpdateWindowSurface(window);

			// Ensure the window is up for 5s before exit
			SDL_Delay(5000);
		}
	}

	// Cleanup
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
