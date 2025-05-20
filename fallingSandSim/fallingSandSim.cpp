//graphics
#include <SFML/graphics.hpp>

//image proccessing
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//other
#include <iostream>
#include <vector>


constexpr const char* image_folder_path = "..\\..\\images\\";


const int WIDTH = 151;
const int HEIGHT = 225;
const int PIXEL_SIZE = 4;

const float UPDATE_TIME = 1.0f / 20.0f; // update 60 times/sec

enum ParticleType
{
    EMPTY,
    SAND,
    WALL
};

struct Cell
{
    ParticleType type;
    sf::Color color;

    Cell(ParticleType t, sf::Color c)
    {
        type = t;
        color = c;
    }
};

std::vector<std::vector<Cell>> grid(HEIGHT, std::vector<Cell>(WIDTH, Cell(EMPTY, sf::Color::Black)));

sf::Vector3<int> addImageToGrid(const char* path, int yOffset);

void setWalls(int yStart, int length);
void setReverseWalls(int yStart, int length);
void setBox(int yStart, int sizeX, int sizeY);

void update();
void draw(sf::RenderWindow& window);
int roll(int min, int max);


int main()
{
    std::string path = image_folder_path;
    path += "32x32_heart.png";
    auto imageInfo = addImageToGrid(path.c_str(), 0);

    setWalls(100, 60);
    
    setReverseWalls(75, 25);
    setBox(HEIGHT - 1 - imageInfo.y, imageInfo.x + 2, imageInfo.y); //get width and height of image

    sf::RenderWindow window(sf::VideoMode(WIDTH * PIXEL_SIZE, HEIGHT * PIXEL_SIZE), "Falling Sand");
    window.setPosition(sf::Vector2i(2500, -300));

    sf::Clock clock;
    float timeSinceLastUpdate = 0.0f;

    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        timeSinceLastUpdate += deltaTime; //set clock


        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // Add sand where the mouse is
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            int x = mousePos.x / PIXEL_SIZE;
            int y = mousePos.y / PIXEL_SIZE;

            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
            {
                int offset = 1;
                if (x == WIDTH - 1 || y >= HEIGHT - 1 || x == 0 || y <= 1)
                {
                    offset = 0;
                }

                grid[y][x].type = SAND;
                grid[y + offset][x].type = SAND;
                grid[y][x + offset].type = SAND;
                grid[y - offset][x].type = SAND;
                grid[y][x - offset].type = SAND;
                grid[y + offset][x + offset].type = SAND;
                grid[y - offset][x - offset].type = SAND;
                grid[y - offset][x + offset].type = SAND;
                grid[y + offset][x - offset].type = SAND;


                grid[y][x].color = sf::Color::Yellow;
                grid[y + offset][x].color = sf::Color::Yellow;
                grid[y][x + offset].color = sf::Color::Yellow;
                grid[y - offset][x].color = sf::Color::Yellow;
                grid[y][x - offset].color = sf::Color::Yellow;
                grid[y + offset][x + offset].color = sf::Color::Yellow;
                grid[y - offset][x - offset].color = sf::Color::Yellow;
                grid[y - offset][x + offset].color = sf::Color::Yellow;
                grid[y + offset][x - offset].color = sf::Color::Yellow;
            }
        }

        
        while (timeSinceLastUpdate >= UPDATE_TIME)
        {
            update();
            timeSinceLastUpdate -= UPDATE_TIME; //only opdate when clock
        }

        window.clear();
        draw(window);
        window.display();
    }

    return 0;
}

//adds an image to the grid at the center of the screen in x, and yOffset. returns Vector3(width, height, channelCount)
sf::Vector3<int> addImageToGrid(const char* path, int yOffset)
{
    int w = 32, h = 32, channelCount = 4;
    //image?
    unsigned char* data = stbi_load(path, &w, &h, &channelCount, 3);

    if (data == nullptr)
    {
        std::cout << "unable to read image!";
        return sf::Vector3(-1, -1, -1);
    }
    else
    {
        std::cout << "image specs: width-" << w << ", height-" << h << ", channels-" << channelCount << "\n";
    }
    int xOffset = (WIDTH / 2) - (w / 2);
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int index = (y * w + x) * 3; // 3 bytes per pixel (R, G, B)

            unsigned char r = data[index + 0];
            unsigned char g = data[index + 1];
            unsigned char b = data[index + 2];

            sf::Color color = sf::Color(r, g, b, 255);

            if (color == sf::Color::Black)
            {
                continue;
            }

            grid[y + yOffset][x + xOffset].type = SAND;
            grid[y + yOffset][x + xOffset].color = color;
        }
    }

    stbi_image_free(data); // Always free the image memory
    return sf::Vector3<int>(w, h, channelCount);
}

void setWalls(int yStart, int length)
{
    int x1 = WIDTH - 1;
    int x2 = 0;

    for (int y = 0; y < length; y++)
    {
        grid[yStart + y][x1] = Cell(WALL, sf::Color(200,200,200,255));
        grid[yStart + y - 1][x1] = Cell(WALL, sf::Color(200,200,200,255));

        grid[yStart + y][x2] = Cell(WALL, sf::Color(200,200,200,255));
        grid[yStart + y - 1][x2] = Cell(WALL, sf::Color(200,200,200,255));

        x1--;
        x2++;
    }
}

void placeReverseTriangle(int startX, int startY, int length)
{
    for (int i = 0; i < length; i++)
    {
        int x1 = startX;
        int x2 = startX;

        for (int y = 0; y < length; y++)
        {
            grid[startY + y][x1] = Cell(WALL, sf::Color(200, 200, 200, 255));
            grid[startY + y - 1][x1] = Cell(WALL, sf::Color(200, 200, 200, 255));

            grid[startY + y][x2] = Cell(WALL, sf::Color(200, 200, 200, 255));
            grid[startY + y - 1][x2] = Cell(WALL, sf::Color(200, 200, 200, 255));

            x1++;
            x2--;
        }

    }
    /*if (length <= 1)
    {
        return;
    }
    placeReverseTriangle(startX - length, startY + length + 5, length / 2);
    placeReverseTriangle(startX + length, startY + length + 5, length / 2);*/
}


void setReverseWalls(int yStart, int length)
{
    int start = WIDTH / 2;
    placeReverseTriangle(start, yStart, length);
}

void setBox(int yStart, int sizeX, int sizeY)
{
    int x1 = (WIDTH / 2) - (sizeX / 2); //get the centered left side
    int x2 = (WIDTH / 2) + (sizeX / 2); //get the centered right side

    for (int y = 0; y < sizeY; y++)
    {
        grid[yStart + y][x1] = Cell(WALL, sf::Color(200, 200, 200, 255));
        grid[yStart + y][x2] = Cell(WALL, sf::Color(200, 200, 200, 255)); //set walls
    }
    for (int x = x1; x <= x2; x++)
    {
        grid[yStart + sizeY][x] = Cell(WALL, sf::Color(200, 200, 200, 255)); //set bottom floor
    }

}

void update()
{
    for (int y = HEIGHT - 2; y >= 0; y--)
    {
        
        if ((rand() % 2 == 0))
        {
            for (int x = WIDTH - 1; x > 0; x--)
            {
                if (grid[y][x].type != SAND)
                {
                    continue;
                }

                //fall if one below is empty
                if (grid[y + 1][x].type == EMPTY)
                {
                    std::swap(grid[y][x], grid[y + 1][x]);
                }
                else //sand has hit another sand
                {

                    if (grid[y + 1][x - 1].type == EMPTY) // try to go down-left or down-right based on dir
                    {
                        std::swap(grid[y][x], grid[y + 1][x - 1]);
                    }
                }
            }
        }

        else
        {
            for (int x = 0; x < WIDTH - 1; x++)
            {
                if (grid[y][x].type != SAND)
                {
                    continue;
                }

                //fall if one below is empty
                if (grid[y + 1][x].type == EMPTY)
                {
                    std::swap(grid[y][x], grid[y + 1][x]);
                }
                else //sand has hit another sand
                {

                    if (grid[y + 1][x + 1].type == EMPTY) // try to go down-left or down-right based on dir
                    {
                        std::swap(grid[y][x], grid[y + 1][x + 1]);
                    }
                }
            }
            
        }
    }
}

void draw(sf::RenderWindow& window)
{
    sf::RectangleShape pixel(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE));

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            pixel.setFillColor(grid[y][x].color);

            pixel.setPosition(x * PIXEL_SIZE, y * PIXEL_SIZE);
            window.draw(pixel);
        }
    }
}


//min-max (small inclusive)
int roll(int min, int max)
{
    // x is in [0,1[
    double x = rand() / static_cast<double>(RAND_MAX + 1);

    // [0,1[ * (max - min) + min is in [min,max[
    int that = min + static_cast<int>(x * (max - min));

    return that;
}