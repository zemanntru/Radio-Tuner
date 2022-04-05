#include "libraries/graphics.hpp"

Graphics::World::World(const std::string &filepath) : done(false), lk1(muData, std::defer_lock), lk2(muData, std::defer_lock), 
freq("fodder"), path(filepath)
{
    surface.setPrimitiveType(sf::Quads);
    surface.resize(4);

    surface[0].position = sf::Vector2f(0, 0); 
    surface[1].position = sf::Vector2f(WINDOW_WIDTH, 0);
    surface[2].position = sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    surface[3].position = sf::Vector2f(0, WINDOW_HEIGHT);

    for (int i = 0; i < 4; i++) surface[i].color = sf::Color::Green;

    mfont.loadFromFile("open_sans.ttf");
    mDisplayThread = std::thread{ &Graphics::World::reader, this};
}

Graphics::World::~World() {
    if(mDisplayThread.joinable())
        mDisplayThread.join();
}

void Graphics::World::render()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), 
            "ECE295 - Team 54 SDR GUI", sf::Style::Titlebar | sf::Style::Close);

   // std::string msg = "LO frequency: ";
    //sf::Text text(msg, mfont, 28);
    //text.setPosition(100, 50);
    //text.setFillColor(sf::Color::Black);
    
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
                    int posx = event.mouseMove.x,               // set the bounds so the cursor tile will always fit on the screen
                        posy = event.mouseMove.y;

                } break;
                case sf::Event::MouseButtonPressed: {
                } break;
                default: break;
            }
        }
        window.clear(sf::Color(192,192,192));

        lk2.lock();
        sf::Text text("LO frequency: " + freq + "Hz" , mfont, 28);
        lk2.unlock();
        text.setPosition(100, 50);
        text.setFillColor(sf::Color::Black);

        window.draw(text);
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
        if(done) break;
   
        int cnt = 0;
        while(true) {
            if(read(tty_fd, &c, 1) > 0) {
                if(c == '\n') break;  
                //printf("what: %c\n\r", c); 
                buffer[cnt++] = c;
            }
        } 
        buffer[cnt] = 0;           
        lk1.lock();
        enable = buffer[0];
        freq = buffer;
        //freq = freq.substr(1, cnt - 1);
        //std::cout << freq << "\n\r";   
        lk1.unlock(); 
        
    }
    close(tty_fd);
}