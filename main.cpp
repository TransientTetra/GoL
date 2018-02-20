#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class cell
{
private:
	int x;
	int y;
public:
	bool alive;
	bool previouslyAlive;
	void setCell(int xOrigin, int yOrigin, bool aliveOrigin)
	{
		x = xOrigin;
		y = yOrigin;
		alive = aliveOrigin;
	}
	void die();
	void live();
	void draw(SDL_Surface *screen)
	{
		if (alive == true)
		{
			Uint32 color = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
			SDL_Rect pixel;
			pixel.x = x;
			pixel.y = y;
			pixel.w = 1;
			pixel.h = 1;

			SDL_FillRect(screen, &pixel, color);
		}
	}
};

class application
{
private:
	SDL_Event event;
	SDL_Window *window;
	SDL_Surface *screen;
	TTF_Font *font;
	SDL_Surface *fontSurface;
	int windowWidth;
	int windowHeight;
	cell **cells;
public:
	application(const char *title, int screenWidth, int screenHeight, const char *fontLocation, int fontSize);
	void initCells(int x, int y);
	int run();
	void end()
	{
		for (int i = 0; i < windowWidth; ++i)
		{
			delete(cells[i]);
		}
		delete(cells);
		SDL_FreeSurface(fontSurface);
		SDL_FreeSurface(screen);
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
	}
};

application::application(const char *title, int screenWidth, int screenHeight, const char *fontLocation, int fontSize)
{
	window = NULL;
	screen = NULL;

	windowWidth = screenWidth;
	windowHeight = screenHeight;

	initCells(screenWidth, screenHeight);

	//sdl init
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << '\n';
		exit(1);
	}

	if (TTF_Init() != 0)
	{
		std::cerr << "Ttf extension failed on load" << '\n';
		exit(1);
	}

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << '\n';
		exit(1);
	}

	screen = SDL_GetWindowSurface(window);
	if (screen == NULL)
	{
		std::cerr << "Screen could not be created! SDL_Error: " << SDL_GetError() << '\n';
		SDL_DestroyWindow(window);
		exit(1);
	}

	//font loading
	font = TTF_OpenFont(fontLocation, fontSize);
	if (font == NULL)
	{
		std::cerr << "Font couldn't be loaded" << '\n';
		SDL_FreeSurface(screen);
		SDL_DestroyWindow(window);
		exit(1);
	}
	SDL_Color color = {255, 255, 255};
	fontSurface = TTF_RenderText_Solid(font, "*PAUSED*", color);
	TTF_CloseFont(font);
}

void application::initCells(int x, int y)
{	
	//cells init
	// dynamic cell array creation
	cells = new cell *[x];
	for (int i = 0; i < x; ++i)
	{
		cells[i] = new cell [y];
	}
	// zeroing cells
	for (int i = 0; i < x; ++i)
	{
		for (int j = 0; j < y; ++j)
		{
			cells[i][j].setCell(i, j, false);
			cells[i][j].previouslyAlive = false;
		}
	}
}

int application::run()
{
	bool pause = true;
	bool quit = false;

	SDL_Rect srcRect;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = fontSurface->w;
	srcRect.h = fontSurface->h;

	SDL_Rect dstRect;
	dstRect.x = windowWidth / 2 - fontSurface->w / 2;
	dstRect.y = 0;
	dstRect.w = fontSurface->w;
	dstRect.h = fontSurface->h;


	cells[400][300].alive = true;
	cells[399][302].alive = true;
	cells[400][302].alive = true;
	cells[402][301].alive = true;
	cells[403][302].alive = true;
	cells[404][302].alive = true;
	cells[405][302].alive = true;

	for (int i = 0; i < windowWidth; ++i)
	{
		for (int j = 0; j < windowHeight; ++j)
		{
			cells[i][j].previouslyAlive = cells[i][j].alive;
		}
	}

	while (quit != true)
	{
		//drawing
		SDL_FillRect(screen,NULL, 0x000000);
		for (int i = 0; i < windowWidth; ++i)
		{
			for (int j = 0; j < windowHeight; ++j)
			{
				cells[i][j].draw(screen);
			}
		}

		if (pause == false)
		{
			//rule checking
			for (int i = 0; i < windowWidth; ++i)
			{
				for (int j = 0; j < windowHeight; ++j)
				{
					int counter = 0;
					if (i < windowWidth - 1)
					{
						if (cells[i + 1][j].previouslyAlive == true) ++counter;
						if (j < windowHeight - 1)
						{
							if (cells[i + 1][j + 1].previouslyAlive == true) ++counter;
						}
						if (j > 0)
						{
							if (cells[i + 1][j - 1].previouslyAlive == true) ++counter;
						}
					}
					if (i > 0)
					{					
						if (cells[i - 1][j].previouslyAlive == true) ++counter;
						if (j < windowHeight - 1)
						{
							if (cells[i - 1][j + 1].previouslyAlive == true) ++counter;
						}
						if (j > 0)
						{
							if (cells[i - 1][j - 1].previouslyAlive == true) ++counter;
						}
					}
					if (j < windowHeight - 1)
					{
						if (cells[i][j + 1].previouslyAlive == true) ++counter;
					}
					if (j > 0)
					{
						if (cells[i][j - 1].previouslyAlive == true) ++counter;
					}

					if (counter < 2 || counter > 3) cells[i][j].alive = false;
					else if (counter == 3) cells[i][j].alive = true;
				}
			}

			//array swapping
			for (int i = 0; i < windowWidth; ++i)
			{
				for (int j = 0; j < windowHeight; ++j)
				{
					cells[i][j].previouslyAlive = cells[i][j].alive;
				}
			}
		}
		else
		{
			SDL_BlitSurface(fontSurface, &srcRect, screen, &dstRect);
		}
		
		SDL_UpdateWindowSurface(window);

		while(SDL_PollEvent(&event) != 0)
		{
			switch(event.type)
			{
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							quit = true;
							break;
						case SDLK_SPACE:
							if (pause == false) pause = true;
							else pause = false;
							break;
					}					
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						int x = 0, y = 0;
						SDL_GetMouseState(&x, &y);
						if (cells[x][y].alive == true)
						{
							cells[x][y].alive = false;
						}
						else cells[x][y].alive = true;
					}
					break;
			}
		}
	}
}

void cell::die()
{
	alive = false;
}

void cell::live()
{
	alive = true;
}

int main(int argc, char const *argv[])
{
	application game("gol", 800, 600, "arial.ttf", 15);
	game.run();
	game.end();
	return 0;
}