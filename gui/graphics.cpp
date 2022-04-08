#include "libraries/graphics.hpp"

Graphics::World::World(const std::string &filepath) : done(false), lk(mu, std::defer_lock), path(filepath)
{
    surface.setPrimitiveType(sf::Quads);
    surface.resize(4);

    box.setPrimitiveType(sf::Lines);
    box.resize(8);

    for(int i = 0; i < 8; ++i) 
        box[i].color = sf::Color::Red;

    surface[0].position = sf::Vector2f(0, 0);
    surface[1].position = sf::Vector2f(WINDOW_WIDTH, 0);
    surface[2].position = sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    surface[3].position = sf::Vector2f(0, WINDOW_HEIGHT);

    for (int i = 0; i < 4; i++) surface[i].color = sf::Color::Green;

    mfont.loadFromFile("open_sans.ttf");
    info = "_______ 0 __";

    mDisplayThread = std::thread{ &Graphics::World::reader, this};
}

Graphics::World::~World() {

    mDisplayThread.join();
    std::cout << "thread ended" << std::endl;
}

void Graphics::World::render()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
            "ECE295 - Team 54 SDR GUI", sf::Style::Titlebar | sf::Style::Close);


    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed: {
                    done = true;
                    window.close();
                    break;
                }
                case sf::Event::MouseMoved: {
                    int posx = event.mouseMove.x,               
                        posy = event.mouseMove.y;

                } break;
                case sf::Event::MouseButtonPressed: {
                } break;

                
                default: break;
            }
        }
        window.clear(sf::Color(192,192,192));

        std::string freq;
        std::string mode;
        int pos;
        lk.lock();
        std::stringstream sstr(info);
        sstr >> freq >> pos >> mode;
        lk.unlock();
        sf::Text text("LO frequency:    " + freq + "MHz" , mfont, 30);
        text.setFillColor(sf::Color::Black);
        text.setPosition(50, 30);
        window.draw(text);
        text.setString("Operating mode:     " + mode);
        text.setPosition(50, 70);
        window.draw(text);

        text.setString("This software is a property of ECE295 Team 54");
        text.setCharacterSize(12);
        text.setPosition(120, 150);
        window.draw(text);

        if(pos == 4) {
            box[0].position = sf::Vector2f(280, 30);
            box[1].position = sf::Vector2f(280, 70);

            box[2].position = sf::Vector2f(280, 30);
            box[3].position = sf::Vector2f(315, 30);

            box[4].position = sf::Vector2f(315, 70);
            box[5].position = sf::Vector2f(280, 70);

            box[6].position = sf::Vector2f(315, 30);
            box[7].position = sf::Vector2f(315, 70);

        } else {
            box[0].position = sf::Vector2f(320 + (3 - pos) * 18, 30);
            box[1].position = sf::Vector2f(320 + (3 - pos) * 18, 70);

            box[2].position = sf::Vector2f(320 + (3 - pos) * 18, 30);
            box[3].position = sf::Vector2f(337 + (3 - pos) * 18, 30);

            box[4].position = sf::Vector2f(337 + (3 - pos) * 18, 70);
            box[5].position = sf::Vector2f(320 + (3 - pos) * 18, 70);

            box[6].position = sf::Vector2f(337 + (3 - pos) * 18, 30);
            box[7].position = sf::Vector2f(337 + (3 - pos) * 18, 70);

            
        }
        window.draw(box);
        window.display();
    }
}

void Graphics::World::reader()
{
    struct termios tio;
    struct termios stdio;
    int tty_fd;
    fd_set rdset;

    memset(&stdio,0,sizeof(stdio));
    stdio.c_iflag=0;
    stdio.c_oflag=0;
    stdio.c_cflag=0;
    stdio.c_lflag=0;
    stdio.c_cc[VMIN]=1;
    stdio.c_cc[VTIME]=0;
    tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
    tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    memset(&tio,0,sizeof(tio));
    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CS8|CREAD|CLOCAL;
    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;

    tty_fd=open(path.c_str(), O_RDWR | O_NONBLOCK);
    cfsetospeed(&tio,B2400);
    cfsetispeed(&tio,B2400);

    tcsetattr(tty_fd,TCSANOW,&tio);

    char c = 'D';
    char buffer[64];

    while (true)
    {
        int cnt = 0;
        if(done) break;
        while(true) {
            if(done) break;
            if(read(tty_fd, &c, 1) > 0) {
                if(c == '\n') break;
                buffer[cnt++] = c;
            }
        }
        buffer[cnt] = 0;
        info = buffer;
    }
    close(tty_fd);
}
