#ifndef H_GRAPHICS_HEADER
#define H_GRAPHICS_HEADER

#include "pch.hpp"

#define WINDOW_WIDTH        500
#define WINDOW_HEIGHT       180

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
            sf::VertexArray box;
            sf::Font mfont;
            std::string path;
            std::atomic<bool>done;
            char enable;
            std::mutex mu;
            std::unique_lock<std::mutex> lk;
            std::string info;
    };
}


#endif