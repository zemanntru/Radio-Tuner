#ifndef H_GRAPHICS_HEADER
#define H_GRAPHICS_HEADER

#include "pch.hpp"

#define WINDOW_WIDTH        600
#define WINDOW_HEIGHT       200

namespace Graphics
{
    class World {
        public:
            World(const std::string &filepath);
            virtual ~World();
            World(const World& src) = delete;
            World& operator=(const World& rhs) = delete;
            void render();
            void reader();
        private:
            std::thread mDisplayThread;
            sf::VertexArray surface;
            sf::Font mfont;
            std::string path;
            std::atomic<bool>done;
            std::unique_lock<std::mutex>lk1;
            std::unique_lock<std::mutex>lk2;
            char enable;
            std::string freq;
            std::mutex muData;
    };
}


#endif