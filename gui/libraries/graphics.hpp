#ifndef H_GRAPHICS_HEADER
#define H_GRAPHICS_HEADER

#include "pch.hpp"

#define WINDOW_WIDTH        500
#define WINDOW_HEIGHT       740

namespace Graphics
{
    class Button : public sf::Drawable, public sf::Transformable {
        public:
            Button(
                float xloc, 
                float yloc, 
                float len, 
                float sxloc,
                float syloc,
                const std::string& src,
                sf::Font &font,
                sf::Color col,
                int txt_sz = 0
            );
            void set_color(sf::Color col);
            sf::Color get_color() const;
        private:
            virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
            sf::Text disp_txt;
            std::vector<sf::CircleShape>circ_out;
            std::vector<sf::VertexArray>rect_out;
            std::vector<sf::CircleShape>circ_in;
            std::vector<sf::VertexArray>rect_in;
    };

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
            sf::VertexArray box;
            sf::VertexArray panel_back;
            sf::VertexArray panel_front;
            sf::Font mfont;
            std::string path;
            std::string out_str;
            std::string send_str;
            std::atomic<bool>done;
            std::atomic<bool>to_send;
            char enable;
            bool error;
            bool rx_mode;
            bool mech_mode;
            std::mutex mu;
            std::unique_lock<std::mutex> lk;
            std::string info;
            std::vector<Button> buttons;
    };

    
}


#endif