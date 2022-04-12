#include "libraries/graphics.hpp"

/*
 * Disclaimer: 
 * This is a very hastily written code under short time constraints to act as a GUI
 * All of the graphics are using primitives given by the SFML library
 * For more references on how the primitives work, check the SFML documentation
 * 
 * Two main classes:
 * Button class
 * GUI + main event loop
 *
 * The rest is just commands to interact with uart output.
 *
 */
 
Graphics::World::World(const std::string &filepath) : 
    done(false), lk(mu, std::defer_lock), path(filepath), to_send(false), error(false), rx_mode(true), mech_mode(true)
{
    panel_back.setPrimitiveType(sf::Quads);
    panel_back.resize(4);

    panel_front.setPrimitiveType(sf::Quads);
    panel_front.resize(4);

    box.setPrimitiveType(sf::Lines);
    box.resize(8);

    for(int i = 0; i < 8; ++i) 
        box[i].color = sf::Color::Red;

    panel_back[0].position = sf::Vector2f(20, 160);
    panel_back[1].position = sf::Vector2f(480, 160);
    panel_back[2].position = sf::Vector2f(480, 240);
    panel_back[3].position = sf::Vector2f(20, 240);

    panel_front[0].position = sf::Vector2f(25, 165);
    panel_front[1].position = sf::Vector2f(475, 165);
    panel_front[2].position = sf::Vector2f(475, 235);
    panel_front[3].position = sf::Vector2f(25, 235);

    for (int i = 0; i < 4; i++) {
        panel_back[i].color = sf::Color::Black;
        panel_front[i].color = sf::Color::White;
    }


    mfont.loadFromFile("open_sans.ttf");
    info = "ACK _______ 0 __";

    buttons.push_back(Button(25,250,110,65,265,"1",mfont,sf::Color(77,77,255)));
    buttons.push_back(Button(140,250,110,180,265,"2",mfont,sf::Color(77,77,255)));
    buttons.push_back(Button(255,250,110,295,265,"3",mfont,sf::Color(77,77,255)));
    buttons.push_back(Button(370,250,110,390,265,"TX",mfont,sf::Color(50,205,50)));

    buttons.push_back(Button(25,365,110,65,380,"4",mfont,sf::Color(77,77,255)));
    buttons.push_back(Button(140,365,110,180,380,"5",mfont,sf::Color(77,77,255)));
    buttons.push_back(Button(255,365,110,295,380,"6",mfont,sf::Color(77,77,255))); 
    //buttons.push_back(Button(370,365,110,390,380,"RX",mfont,sf::Color(255, 211, 0)));
    buttons.push_back(Button(370,365,110,390,380,"RX",mfont,sf::Color(255, 255, 102)));

    buttons.push_back(Button(25,480,110,65,495,"7",mfont,sf::Color(77,77,255)));
    buttons.push_back(Button(140,480,110,180,495,"8",mfont,sf::Color(77,77,255)));
    buttons.push_back(Button(255,480,110,295,495,"9",mfont,sf::Color(77,77,255)));
    buttons.push_back(Button(370,480,110,395,515,"DEL",mfont,sf::Color(183, 104, 162),30));

    buttons.push_back(Button(25,595,110,40,630,"CLEAR",mfont,sf::Color(227, 66, 52), 26));
    buttons.push_back(Button(140,595,110,180,610,"0",mfont,sf::Color(77,77,255)));
    buttons.push_back(Button(255,595,110,270,630,"MECH",mfont,sf::Color(227, 66, 52), 26));
    buttons.push_back(Button(370,595,110,385,630,"ENTER",mfont,sf::Color(242, 133, 0), 26));

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

    std::string freq, radio_mode, input_mode, status;
    int pos;

    std::string digit_str;

    while(window.isOpen())
    {
        sf::Event event;
        lk.lock();
        std::stringstream sstr(info);
        lk.unlock();

        sstr >> status;
        if(status.compare("ACK") != 0) {
            freq = "_______";
            pos =  0; 
            radio_mode = "__";
            input_mode = mech_mode ? "MECH" : "GUI";
        } else {
            sstr >> freq >> pos >> radio_mode >> input_mode;
            if(input_mode.compare("MECH") == 0)
                mech_mode = true;
            else
                mech_mode = false;
        }
        while(window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed: {
                    window.close();
                    break;
                }
                case sf::Event::MouseMoved: {
                    int posx = event.mouseMove.x,               
                        posy = event.mouseMove.y;

                    if(25 < posx && posx < 135 && 250 < posy && posy < 360)
                        buttons[0].set_color(sf::Color(137,207,240));
                    else
                        buttons[0].set_color(sf::Color(77,77,255));
                    
                    if(140 < posx && posx < 250 && 250 < posy && posy < 360)
                        buttons[1].set_color(sf::Color(137,207,240));
                    else
                        buttons[1].set_color(sf::Color(77,77,255));

                    if(255 < posx && posx < 365 && 250 < posy && posy < 360)
                        buttons[2].set_color(sf::Color(137,207,240));
                    else
                        buttons[2].set_color(sf::Color(77,77,255));

                    if(rx_mode) {
                        if(370 < posx && posx < 480 && 250 < posy && posy < 360) 
                            buttons[3].set_color(sf::Color(152, 251, 152));
                        else 
                            buttons[3].set_color(sf::Color(50, 205, 50));
                    }
                    
                    if(25 < posx && posx < 135 && 365 < posy && posy < 475)
                        buttons[4].set_color(sf::Color(137,207,240));
                    else
                        buttons[4].set_color(sf::Color(77,77,255));
                    
                    if(140 < posx && posx < 250 && 365 < posy && posy < 475)
                        buttons[5].set_color(sf::Color(137,207,240));
                    else
                        buttons[5].set_color(sf::Color(77,77,255));

                    if(255 < posx && posx < 365 && 365 < posy && posy < 475)
                        buttons[6].set_color(sf::Color(137,207,240));
                    else
                        buttons[6].set_color(sf::Color(77,77,255));

                    if(!rx_mode) {
                        if(370 < posx && posx < 480 && 365 < posy && posy < 475) 
                            buttons[7].set_color(sf::Color(255, 255, 102));
                        else 
                            buttons[7].set_color(sf::Color(255, 211, 0));
                    }


                    if(25 < posx && posx < 135 && 480 < posy && posy < 590)
                        buttons[8].set_color(sf::Color(137,207,240));
                    else
                        buttons[8].set_color(sf::Color(77,77,255));
                    
                    if(140 < posx && posx < 250 && 480 < posy && posy < 590)
                        buttons[9].set_color(sf::Color(137,207,240));
                    else
                        buttons[9].set_color(sf::Color(77,77,255));

                    if(255 < posx && posx < 365 && 480 < posy && posy < 590)
                        buttons[10].set_color(sf::Color(137,207,240));
                    else
                        buttons[10].set_color(sf::Color(77,77,255));

                    if(370 < posx && posx < 480 && 480 < posy && posy < 590) 
                        buttons[11].set_color(sf::Color(224, 176, 255));
                    else 
                        buttons[11].set_color(sf::Color(183, 104, 162));


                    if(25 < posx && posx < 135 && 595 < posy && posy < 705)
                        buttons[12].set_color(sf::Color(255, 127, 127));
                    else
                        buttons[12].set_color(sf::Color(227, 66, 52));
                    
                    if(140 < posx && posx < 250 && 595 < posy && posy < 705)
                        buttons[13].set_color(sf::Color(137,207,240));
                    else
                        buttons[13].set_color(sf::Color(77,77,255));

                    if(255 < posx && posx < 365 && 595 < posy && posy < 705){
                        if(mech_mode) 
                            buttons[14].set_color(sf::Color(227, 66, 52));
                        else
                            buttons[14].set_color(sf::Color(255, 127, 127));
                    } else {
                        if(mech_mode)
                            buttons[14].set_color(sf::Color(255, 127, 127));
                        else
                            buttons[14].set_color(sf::Color(227, 66, 52));
                    }
                       

                    if(370 < posx && posx < 480 && 595 < posy && posy < 705) 
                        buttons[15].set_color(sf::Color(255, 229, 180));
                    else 
                        buttons[15].set_color(sf::Color(242, 133, 0));
                
                } break;
                case sf::Event::MouseButtonPressed: {
                    if(event.mouseButton.button == sf::Mouse::Left) {
                        int posx = event.mouseButton.x, 
                            posy = event.mouseButton.y;
                        if(25 < posx && posx < 135 && 250 < posy && posy < 360) {
                            if(error) {
                                error = false;
                                digit_str.clear();
                            }
                            if(digit_str.size() <= 5) {
                                digit_str.push_back('1');
                            }
                        }
                        else if(140 < posx && posx < 250 && 250 < posy && posy < 360) {
                            if(error) {
                                error = false;
                                digit_str.clear();
                            }
                            if(digit_str.size() <= 5) 
                                digit_str.push_back('2');
                        }
                        else if(255 < posx && posx < 365 && 250 < posy && posy < 360) {
                            if(error) {
                                error = false;
                                digit_str.clear();
                            }
                            if(digit_str.size() <= 5) 
                                digit_str.push_back('3');
                        }
                        else if(370 < posx && posx < 480 && 250 < posy && posy < 360) {
                            rx_mode = false;
                            send_str = "TX\n";
                            to_send = true;
                            buttons[3].set_color(sf::Color(152, 251, 152));
                            buttons[7].set_color(sf::Color(255, 211, 0));
                        }
                        else if(25 < posx && posx < 135 && 365 < posy && posy < 475) {
                            if(error) {
                                error = false;
                                digit_str.clear();
                            }
                            if(digit_str.size() <= 5) 
                                digit_str.push_back('4');
                        }
                        else if(140 < posx && posx < 250 && 365 < posy && posy < 475) {
                            if(error) {
                                error = false;
                                digit_str.clear();
                            }
                            if(digit_str.size() <= 5) 
                                digit_str.push_back('5');
                        }
                        else if(255 < posx && posx < 365 && 365 < posy && posy < 475) {
                            if(error) {
                                error = false;
                                digit_str.clear();
                            }
                            if(digit_str.size() <= 5) 
                                digit_str.push_back('6');
                        }
                        else if(370 < posx && posx < 480 && 365 < posy && posy < 475) {
                            rx_mode = true;
                            send_str = "RX\n";
                            to_send = true;
                            buttons[3].set_color(sf::Color(50, 205, 50));
                            buttons[7].set_color(sf::Color(255, 255, 102));
                        }
                        else if(25 < posx && posx < 135 && 480 < posy && posy < 590) {
                            if(error) {
                                error = false;
                                digit_str.clear();
                            }
                            if(digit_str.size() <= 5) 
                                digit_str.push_back('7');
                        }
                        else if(140 < posx && posx < 250 && 480 < posy && posy < 590) {
                            if(error) {
                                error = false;
                                digit_str.clear();
                            }
                            if(digit_str.size() <= 5) 
                                digit_str.push_back('8');
                        }
                        else if(255 < posx && posx < 365 && 480 < posy && posy < 590) {
                            if(error) {
                                error = false;
                                digit_str.clear();
                            }
                            if(digit_str.size() <= 5) 
                                digit_str.push_back('9');
                        }
                        else if(140 < posx && posx < 250 && 595 < posy && posy < 705) {
                            if(error) {
                                error = false;
                                digit_str.clear();
                            }
                            if(digit_str.size() <= 5 && !digit_str.empty()) 
                                digit_str.push_back('0');
                        }

                        else if(370 < posx && posx < 480 && 480 < posy && posy < 590) {
                            if(error) {
                                error = false;
                                digit_str.clear();
                            }

                            if(!digit_str.empty()) 
                                digit_str.pop_back();
                        }

                        else if(370 < posx && posx < 480 && 595 < posy && posy < 705) {
                            if(out_str.compare("ERROR: Not in Range") != 0) {
                                float val = std::stof(out_str);
                                if(8.0f <= val && val < 16.0f) {
                                    send_str = out_str + "\n";
                                    to_send = true;
                                } else 
                                    error = true;
                                
                            }
                        }

                        else if(25 < posx && posx < 135 && 595 < posy && posy < 705) {
                            error = false;
                            digit_str.clear();
                        }
                        else if(255 < posx && posx < 365 && 595 < posy && posy < 705) {
                            mech_mode = !mech_mode;
                            send_str = (mech_mode ? "MECH" : "GUI");
                            send_str += "\n";
                            to_send = true;

                            if(mech_mode) {
                                buttons[14].set_color(sf::Color(255, 127, 127));
                            } else {
                                buttons[14].set_color(sf::Color(227, 66, 52));
                            }
                        }
                    }
                } break;
                default: break;
            }
        }
        window.clear(sf::Color(0xff,0xe4,0xe1));
        
        
        sf::Text text("LO frequency:    " + freq + "MHz" , mfont, 30);
        text.setFillColor(sf::Color::Black);
        text.setPosition(50, 30);
        window.draw(text);
        text.setString("Transmission Mode:     " + radio_mode);
        text.setPosition(50, 70);
        window.draw(text);
        text.setString("Input Mode:     " + input_mode);
        text.setPosition(50, 110);
        window.draw(text);

        text.setString("This software is a property of ECE295 Team 54");
        text.setCharacterSize(12);
        text.setPosition(120, 720);
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
        window.draw(panel_back);
        window.draw(panel_front);
       
        if(error) {
            out_str = "ERROR: Not in Range";
            text.setPosition(55, 170);
            text.setCharacterSize(40);
        } else {
            out_str = digit_str;
            while(out_str.size() <= 5)
                out_str.insert(0, "0");

            out_str.insert(2,".");
            text.setCharacterSize(65);
            text.setPosition(230, 160);
        }
        text.setString(out_str.c_str());
        window.draw(text);

        for(const auto &x: buttons)
            window.draw(x);
        
        window.display();
    }
    done = true;
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

    tio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                       | INLCR | IGNCR | ICRNL | IXON);
    tio.c_oflag &= ~OPOST;
    tio.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tio.c_cflag &= ~(CSIZE | PARENB);
    tio.c_cflag |= CS8;


    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;

    tty_fd=open(path.c_str(), O_RDWR | O_NONBLOCK);
    cfsetospeed(&tio,B2400);
    cfsetispeed(&tio,B2400);

    tcsetattr(tty_fd,TCSANOW,&tio);

    char c = 'D';
    char buffer[64];

    std::thread t1([&]{
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
    });

    std::thread t2([&]{
        while (true)
        {
            if(to_send) {
                write(tty_fd, send_str.c_str(), send_str.size());
                to_send = false;
            }
            if(done) break;
        }
    });

    t1.join();
    t2.join();
    
    close(tty_fd);
}

Graphics::Button::Button(float xloc, float yloc, 
                         float len, 
                         float sxloc, float syloc,
                         const std::string &src,
                         sf::Font &font,
                         sf::Color col,
                         int txt_sz)
{
    float rad = len / 6;
    sf::CircleShape circ(rad);
    circ.setFillColor(sf::Color::White);
    circ.setPosition(xloc, yloc); 
    circ_out.push_back(circ);
    circ.setPosition(xloc + len - 2*rad, yloc); 
    circ_out.push_back(circ);
    circ.setPosition(xloc, yloc + len - 2*rad);
    circ_out.push_back(circ);
    circ.setPosition(xloc + len - 2*rad, yloc + len - 2*rad);
    circ_out.push_back(circ);

    sf::VertexArray rect;
    rect.setPrimitiveType(sf::Quads);
    rect.resize(4);

    for(int i = 0; i < 4; ++i) 
        rect[i].color = sf::Color::White;

    rect[0].position = sf::Vector2f(xloc + rad, yloc + rad);
    rect[1].position = sf::Vector2f(xloc + rad, yloc + len - rad);
    rect[2].position = sf::Vector2f(xloc + len - rad, yloc + len - rad);
    rect[3].position = sf::Vector2f(xloc + len - rad, yloc + rad);
    rect_out.push_back(rect);

    rect[0].position = sf::Vector2f(xloc, yloc + rad);
    rect[1].position = sf::Vector2f(xloc + rad, yloc + rad);
    rect[2].position = sf::Vector2f(xloc + rad, yloc + len - rad);
    rect[3].position = sf::Vector2f(xloc, yloc + len - rad);
    rect_out.push_back(rect);

    rect[0].position = sf::Vector2f(xloc + rad, yloc);
    rect[1].position = sf::Vector2f(xloc + len - rad, yloc);
    rect[2].position = sf::Vector2f(xloc + len - rad, yloc + rad);
    rect[3].position = sf::Vector2f(xloc + rad, yloc + rad);
    rect_out.push_back(rect);

    rect[0].position = sf::Vector2f(xloc + len - rad, yloc + rad);
    rect[1].position = sf::Vector2f(xloc + len, yloc + rad);
    rect[2].position = sf::Vector2f(xloc + len, yloc + len - rad);
    rect[3].position = sf::Vector2f(xloc + len - rad, yloc + len - rad);
    rect_out.push_back(rect);

    rect[0].position = sf::Vector2f(xloc + rad, yloc + len - rad);
    rect[1].position = sf::Vector2f(xloc + len - rad, yloc + len - rad);
    rect[2].position = sf::Vector2f(xloc + len - rad, yloc + len);
    rect[3].position = sf::Vector2f(xloc + rad, yloc + len);
    rect_out.push_back(rect);

    rad = len / 9;
    float offset = len / 6 - len / 9;
    circ.setRadius(rad);
    circ.setFillColor(col);

    circ.setPosition(xloc + offset, yloc + offset); 
    circ_in.push_back(circ);
    circ.setPosition(xloc - offset + len - 2*rad, yloc + offset); 
    circ_in.push_back(circ);
    circ.setPosition(xloc + offset, yloc + len - 2*rad - offset);
    circ_in.push_back(circ);
    circ.setPosition(xloc + len - 2*rad - offset, yloc + len - 2*rad - offset);
    circ_in.push_back(circ);

    for(int i = 0; i < 4; ++i) 
        rect[i].color = col;

    rect[0].position = sf::Vector2f(xloc + rad + offset, yloc + rad + offset);
    rect[1].position = sf::Vector2f(xloc + rad + offset, yloc + len - rad - offset);
    rect[2].position = sf::Vector2f(xloc + len - rad - offset, yloc + len - rad - offset);
    rect[3].position = sf::Vector2f(xloc + len - rad - offset, yloc + rad + offset);
    rect_in.push_back(rect);

    rect[0].position = sf::Vector2f(xloc + offset, yloc + rad + offset);
    rect[1].position = sf::Vector2f(xloc + offset + rad, yloc + rad + offset);
    rect[2].position = sf::Vector2f(xloc + offset + rad, yloc + len - rad - offset);
    rect[3].position = sf::Vector2f(xloc + offset, yloc + len - rad - offset);
    rect_in.push_back(rect);

    rect[0].position = sf::Vector2f(xloc + rad + offset, yloc + offset);
    rect[1].position = sf::Vector2f(xloc + len - rad - offset, yloc + offset);
    rect[2].position = sf::Vector2f(xloc + len - rad - offset, yloc + rad + offset);
    rect[3].position = sf::Vector2f(xloc + rad + offset, yloc + rad + offset);
    rect_in.push_back(rect);

    
    rect[0].position = sf::Vector2f(xloc + len - offset - rad, yloc + rad + offset);
    rect[1].position = sf::Vector2f(xloc + len - offset, yloc + rad + offset);
    rect[2].position = sf::Vector2f(xloc + len - offset, yloc + len - offset - rad);
    rect[3].position = sf::Vector2f(xloc + len - offset - rad, yloc + len - offset - rad);
    rect_in.push_back(rect);

    rect[0].position = sf::Vector2f(xloc + rad + offset, yloc + len - rad - offset);
    rect[1].position = sf::Vector2f(xloc + len - rad - offset, yloc + len - rad - offset);
    rect[2].position = sf::Vector2f(xloc + len - rad - offset, yloc + len - offset);
    rect[3].position = sf::Vector2f(xloc + rad + offset, yloc + len - offset);
    
    rect_in.push_back(rect);
    if(txt_sz == 0) txt_sz = len / 2;
    sf::Text txt(src.c_str(), font, txt_sz);
    txt.setFillColor(sf::Color::White);
    txt.setPosition(sxloc, syloc);
    disp_txt = txt;
}

void Graphics::Button::draw(sf::RenderTarget& target, sf::RenderStates states) const 
{           
    states.transform *= getTransform();     
    for(const auto& x : circ_out)            
        target.draw(x);

    for(const auto& x : rect_out)            
        target.draw(x);

    for(const auto& x : circ_in)            
        target.draw(x);

    for(const auto& x : rect_in)            
        target.draw(x);

    target.draw(disp_txt);
}

void Graphics::Button::set_color(sf::Color col)
{   
    for(auto& x : circ_in)            
        x.setFillColor(col);

    for(auto& x : rect_in) {
        for(int i = 0; i < 4; ++i)
            x[i].color = col;
    }       
}

sf::Color Graphics::Button::get_color() const
{
    return circ_in[0].getFillColor();
}