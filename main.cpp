#include "Game.h"
#include "BeltInsert.h"
#include "Miner.h"
#include "Base.h"
#include <random>
#include <SDL2/SDL_mixer.h>

int main (int argc, const char * argv[])
{
  SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  TTF_Init ();
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

  time_t now;
  time(&now);
  std::srand(now);
  std::cout << "seed: " << now << std::endl;

  Mix_Chunk* theme = Mix_LoadWAV("resources/theme.wav");
  Mix_PlayChannel (-1, theme, -1);

  Game g {};

  {
    g.km.map('n', "q", [&g](){ g.quit = true; });

    auto held_x = [&g]()
    {
      if (!g.is_x_held) return;
      g.km.exec('n', "x");
    };

    // Cursor Movement
    g.km.map('n', "h", [&](){ g.cur.x--; g.keepCursorInFrame(); held_x(); });
    g.km.map('n', "l", [&](){ g.cur.x++; g.keepCursorInFrame(); held_x(); });
    g.km.map('n', "k", [&](){ g.cur.y--; g.keepCursorInFrame(); held_x(); });
    g.km.map('n', "j", [&](){ g.cur.y++; g.keepCursorInFrame(); held_x(); });

    // Camera Movement
    g.km.map('n', "H", [&g](){ g.cam.x--; g.moveCursorIntoFrame(); });
    g.km.map('n', "L", [&g](){ g.cam.x++; g.moveCursorIntoFrame(); });
    g.km.map('n', "K", [&g](){ g.cam.y--; g.moveCursorIntoFrame(); });
    g.km.map('n', "J", [&g](){ g.cam.y++; g.moveCursorIntoFrame(); });

    g.km.map('n', "zh", [&g](){ g.scrolloff(g.cur.x, g.cur.y, DIR::W); });
    g.km.map('n', "zl", [&g](){ g.scrolloff(g.cur.x, g.cur.y, DIR::E); });
    g.km.map('n', "zk", [&g](){ g.scrolloff(g.cur.x, g.cur.y, DIR::N); });
    g.km.map('n', "zj", [&g](){ g.scrolloff(g.cur.x, g.cur.y, DIR::S); });

    g.km.nmap ("<c-y>", "K");
    g.km.nmap ("<c-e>", "J");

    g.km.nmap ("zt", "zk");
    g.km.nmap ("zb", "zj");

    g.km.map('n', "<c-u>",[&g](){
      g.cam.y -= (g.ws.h/(g.u*g.cam.z))/2;
      g.moveCursorIntoFrame ();
    });
    g.km.map('n', "<c-d>",[&g](){
      g.cam.y += (g.ws.h/(g.u*g.cam.z))/2;
      g.moveCursorIntoFrame ();
    });

    // Camera Zoom
    g.km.map('n', "-", [&g](){
      if (g.cam.z <= 0.1) return;
      g.cam.z *= 0.75;
    });

    g.km.map('n', "+", [&g](){
      if (g.cam.z >= 2) return;
      g.cam.z *= 1.25;
      g.keepCursorInFrame();
    });

    // Centering
    g.km.map('n', "zz", [&g](){
      g.cam.x = g.cur.x;
      g.cam.y = g.cur.y;
    });

    // -------- Editing

    // Delete under cursor
    g.km.map('n', "x", [&g](){
      if (auto q = g.world.at (g.cur.x, g.cur.y); q != nullptr)
        g.world.remove(q);
    });

    // Toggle splitter priority
    g.km.map('n', "o", [&g](){ // switch priority
      if (auto q = dynamic_cast<Splitter*>(g.world.at (g.cur.x, g.cur.y)); q != nullptr)
        q->opri = (Priority) ((q->opri+1)%3);
    });

    beltInsert (g);
  }

  g.world.base = new Base (2, 2);
  g.world.qt.insert(g.world.base);

  g.km.setMode('n');
  // g.km.put(Key('z', Key::MOD_CHAR));
  // g.km.debug();

  g.loop ();

  return 0;
}

