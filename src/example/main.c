#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "actors.h"
#include "events.h"
#include "chunks.h"
#include "utils.h"

#include "services/transform.h"
#include "services/renderer.h"
#include "actors/player.h"
#include "actors/missile.h"

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);

	srand(time(NULL));

	register_service(&transform_service);
	register_service(&renderer_service);

	struct player *p = spawn(&player_spawner);
	for(int i = 0; i < 20; ++i) {
		spawn(&missile_spawner);
	}

	SDL_Event e;
	int running = 1;
	while(running) {
		while(SDL_PollEvent(&e)) {
			switch(e.type) {
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYDOWN:
					if(e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
						running = 0;
					}
					break;
			}
		}

		tick();

		if(PLAYER_ID == -1) {
			running = 0;
		}
	}

	destroy_the_world();
	SDL_Quit();
	return 0;
}
