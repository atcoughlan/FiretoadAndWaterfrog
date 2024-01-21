#include "Game.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>

Game::Game()
{
    // Background sprite initialisation
    if (!m_backgroundTexture.loadFromFile("Assets/tiledBackgroundBig.png")) // https://imageonline.co/repeat-image-generator.php
    {
        system("pause");
    }
    m_backgroundSprite = sf::Sprite(m_backgroundTexture);
    m_backgroundSprite.setPosition(0.f, 0.f);

    // Fade out logic and rect initialisation
    m_isFadeOut = false;
    m_fadeAlpha = 0;
    m_doStateChange = false;
    m_fadeTimeAcc = 0.f;
    m_fadeRect.setSize(sf::Vector2f(static_cast<float>(m_screenSize.x), static_cast<float>(m_screenSize.y)));
    m_fadeRect.setPosition(sf::Vector2f(0.f, 0.f));
    m_fadeRect.setFillColor(sf::Color(0, 0, 0, m_fadeAlpha)); 

    // Create game window
    m_gameWindow = new sf::RenderWindow(sf::VideoMode(m_screenSize.x, m_screenSize.y), "Firetoad and Waterfrog");

    m_currLevel = 0;

    // TileMap initialisation
    m_tileMap = new TileMap(m_levelInfo.getTempPlatformsPositions(m_currLevel), m_levelInfo.getButtonPositions(m_currLevel));
    if (!m_tileMap->load("Assets/my-tileset.png", sf::Vector2u(32, 32), m_levelInfo.getLevels(m_currLevel), m_width, m_height))
    {
        system("pause");
    }
    m_waterfrog.setTileMap(m_tileMap);
    m_firetoad.setTileMap(m_tileMap);

    // Exit and flies initialisation
    m_fireExit = new Exit(Element::fire);
    m_waterExit = new Exit(Element::water);
    
    for (int i = 0; i < m_nbFlies; i++)
    {
        m_fireFlies[i] = new Fly(Element::fire);
        m_waterFlies[i] = new Fly(Element::water);
    }

    m_gameState = GameState::intro;

    initText();
}

// Main game loop
void Game::run()
{
    while (m_gameWindow->isOpen())
    {
        float deltaTime = m_gameLoopClock.restart().asSeconds();
        processEvents();
        update(deltaTime);
        render();
    }
}

//////////////////////////////////// All processEvents, update and render functions ///////////////////////////////////

// Main game's processEvents
void Game::processEvents()
{
    sf::Event event;
    while (m_gameWindow->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_gameWindow->close();
        }
    }
    
    switch (m_gameState)
    {
    case GameState::intro:
        processEventsIntro();
        break;

    case GameState::level:
        processEventsLevel();
        break;

    case GameState::outro:
        processEventsOutro();
        break;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        m_gameWindow->close();
    }
}

// Main game's update
void Game::update(float deltaTime)
{
    if (m_isFadeOut)
    {
        // Set the alpha of the fade rect based on how much time has elapsed since the fading started
        m_fadeTimeAcc += deltaTime;
        if (m_fadeTimeAcc >= 0.05f)
        {
            m_fadeAlpha += static_cast<int>(255 * m_fadeTimeAcc * (1.f / 1.5f)); // total fade time: 1.5 seconds
            m_fadeTimeAcc = 0.f;
        }

        // Once the rect is 100% opaque, stop fading
        if (m_fadeAlpha >= 255)
        {
            m_fadeAlpha = 255;
            m_isFadeOut = false;
        }
    }
    else
    {
        m_fadeAlpha = 0;
    }
    m_fadeRect.setFillColor(sf::Color(0, 0, 0, m_fadeAlpha));

    
    switch (m_gameState)
    {
    case GameState::intro: 
        updateIntro(deltaTime);
        break;

    case GameState::level:
        updateLevel(deltaTime);
        break;

    case GameState::outro: 
        updateOutro(deltaTime);
        break;
    }
}

// Main game's render
void Game::render()
{
    m_gameWindow->clear();

    switch (m_gameState)
    {
    case GameState::intro: 
        renderIntro();
        break;

    case GameState::level:
        renderLevel();
        break;

    case GameState::outro: 
        renderOutro();
        break;
    }

    m_gameWindow->draw(m_fadeRect);
    m_gameWindow->display();
}

// Intro screen
void Game::processEventsIntro()
{
    // When the user presses enter, do a fade out and switch to the first level

    if (!m_doStateChange)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
        {
            m_doStateChange = true;
            m_isFadeOut = true;
        }
    }
    else
    {
        if (!m_isFadeOut)
        {
            m_gameState = GameState::level;
            resetLevel();
            m_doStateChange = false;
        }
    }
}
void Game::updateIntro(float deltaTime)
{

}
void Game::renderIntro()
{
    m_gameWindow->draw(m_backgroundSprite);
    for (auto& text : m_introText)
    {
        m_gameWindow->draw(text);
    }
}

// Levels
void Game::processEventsLevel()
{
    if (!m_doStateChange)
    {
        // Firetoad controls
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            m_firetoad.keyPressed(Direction::left);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            m_firetoad.keyPressed(Direction::right);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            m_firetoad.keyPressed(Direction::up);
        }

        // Waterfrog controls
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            m_waterfrog.keyPressed(Direction::left);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            m_waterfrog.keyPressed(Direction::right);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            m_waterfrog.keyPressed(Direction::up);
        }
    }
}
void Game::updateLevel(float deltaTime)
{
    if (!m_isFadeOut)
    {
        m_waterfrog.tick(deltaTime);
        m_firetoad.tick(deltaTime);
    }

    m_fireExit->tick(deltaTime);
    m_waterExit->tick(deltaTime);

    for (int i = 0; i < m_nbFlies; i++)
    {
        m_fireFlies[i]->tick(deltaTime);
        m_waterFlies[i]->tick(deltaTime);
    }

    checkFlyCollection();

    if (!m_doStateChange)
    {
        if (checkLevelCompletion() || (m_firetoad.getIsDead() || m_waterfrog.getIsDead()))
        {
            // Start the fade out
            m_levelTimes[m_currLevel] = m_levelClock.getElapsedTime().asSeconds(); // records time before the fade starts
            m_doStateChange = true;
            m_isFadeOut = true;
        }
    }
    else
    {
        if (!m_isFadeOut)
        {
            if (checkLevelCompletion())
            {
                if (m_currLevel == 0)
                {
                    m_currLevel = 1;
                    resetLevel();
                }
                else
                {
                    m_gameState = GameState::outro;
                    endGameText();
                }
            }
            else
            {
                resetLevel();
            }
            m_doStateChange = false;
        }
    }
}
void Game::renderLevel()
{
    m_gameWindow->draw(m_backgroundSprite);

    m_gameWindow->draw(*m_tileMap);

    m_gameWindow->draw(m_fireExit->getSprite());
    m_gameWindow->draw(m_waterExit->getSprite());

    for (int i = 0; i < m_nbFlies; i++)
    {
        if (!m_fireFlies[i]->getIsCollected())
        {
            m_gameWindow->draw(m_fireFlies[i]->getSprite());
        }
        if (!m_waterFlies[i]->getIsCollected())
        {
            m_gameWindow->draw(m_waterFlies[i]->getSprite());
        }
    }

    m_gameWindow->draw(m_waterfrog.getSprite());
    m_gameWindow->draw(m_firetoad.getSprite());
}

// Outro screen
void Game::processEventsOutro()
{
    if (!m_doStateChange)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        {
            m_doStateChange = true;
            m_isFadeOut = true;
        }
    }
    else
    {
        if (!m_isFadeOut)
        {
            m_gameState = GameState::intro; // should also reset the currLevel to 0
            m_currLevel = 0;
            m_doStateChange = false;
        }
    }
}
void Game::updateOutro(float deltaTime)
{
    m_fireFlies[0]->setPosition(sf::Vector2f(m_screenSize.x / 4 - m_outroText[6].getGlobalBounds().width / 2 - 32.f, 256.f + 32.f));
    m_fireFlies[1]->setPosition(sf::Vector2f(m_screenSize.x / 4 * 3 - m_outroText[6].getGlobalBounds().width / 2 - 32.f, 256.f + 32.f));

    m_waterFlies[0]->setPosition(sf::Vector2f(m_screenSize.x / 4 - m_outroText[6].getGlobalBounds().width / 2 - 32.f, 320.f + 32.f));
    m_waterFlies[1]->setPosition(sf::Vector2f(m_screenSize.x / 4 * 3 - m_outroText[6].getGlobalBounds().width / 2 - 32.f, 320.f + 32.f));

    m_fireFlies[0]->tick(deltaTime);
    m_fireFlies[1]->tick(deltaTime);

    m_waterFlies[0]->tick(deltaTime);
    m_waterFlies[1]->tick(deltaTime);
}
void Game::renderOutro()
{
    m_gameWindow->draw(m_backgroundSprite);

    m_gameWindow->draw(m_fireFlies[0]->getSprite());
    m_gameWindow->draw(m_fireFlies[1]->getSprite());

    m_gameWindow->draw(m_waterFlies[0]->getSprite());
    m_gameWindow->draw(m_waterFlies[1]->getSprite());

    for (auto& text : m_outroText)
    {
        m_gameWindow->draw(text);
    }
}

/////////////////////////////////////////////////// Helper Functions //////////////////////////////////////////////////

void Game::resetLevel()
{
    delete m_tileMap;
    m_tileMap = new TileMap(m_levelInfo.getTempPlatformsPositions(m_currLevel), m_levelInfo.getButtonPositions(m_currLevel));
    if (!m_tileMap->load("Assets/my-tileset.png", sf::Vector2u(32, 32), m_levelInfo.getLevels(m_currLevel), m_width, m_height))
    {
        system("pause");
    }
    m_waterfrog.setTileMap(m_tileMap);
    m_firetoad.setTileMap(m_tileMap);

    m_firetoad.setPosition(m_levelInfo.getFiretoadStart(m_currLevel));
    m_waterfrog.setPosition(m_levelInfo.getWaterfrogStart(m_currLevel));

    m_fireExit->setPosition(m_levelInfo.getFireExitStart(m_currLevel));
    m_waterExit->setPosition(m_levelInfo.getWaterExitStart(m_currLevel));

    for (int i = 0; i < m_nbFlies; i++)
    {
        m_fireFlies[i]->setPosition(m_levelInfo.getFireFliesStart(m_currLevel)[i]);
        m_waterFlies[i]->setPosition(m_levelInfo.getWaterFliesStart(m_currLevel)[i]);

        m_fireFlies[i]->setIsCollected(false);
        m_waterFlies[i]->setIsCollected(false);
    }

    m_firetoad.setIsDead(false);
    m_waterfrog.setIsDead(false);
    m_firetoad.setVelocityY(0.f);
    m_waterfrog.setVelocityY(0.f);

    m_finalFireFlyCount[m_currLevel] = 0;
    m_finalWaterFlyCount[m_currLevel] = 0;

    m_levelClock.restart();
}

// Check to see if both players are in front of their respective exits
bool Game::checkLevelCompletion()
{
    sf::Sprite waterfrogSprite = m_waterfrog.getSprite();
    sf::Sprite firetoadSprite = m_firetoad.getSprite();

    sf::Sprite waterExitSprite = m_waterExit->getSprite();
    sf::Sprite fireExitSprite = m_fireExit->getSprite();

    if (isExitContainsPlayer(waterExitSprite, waterfrogSprite) &&
        isExitContainsPlayer(fireExitSprite, firetoadSprite) &&
        !m_waterfrog.getIsMoving() &&
        !m_firetoad.getIsMoving())
    {
        return true;
    }
    return false;
}

bool Game::isExitContainsPlayer(sf::Sprite exit, sf::Sprite player)
{
    sf::FloatRect exitRect = exit.getGlobalBounds();
    sf::FloatRect playerRect = player.getGlobalBounds();

    if (exitRect.contains(playerRect.left, playerRect.top) &&
        exitRect.contains(playerRect.left + playerRect.width - 1, playerRect.top + playerRect.height - 1))
    {
        return true;
    }
    return false;
}

void Game::checkFlyCollection()
{
    for (int i = 0; i < m_nbFlies; i++)
    {
        if (!m_fireFlies[i]->getIsCollected() &&
            m_firetoad.getSprite().getGlobalBounds().intersects(m_fireFlies[i]->getSprite().getGlobalBounds()))
        {
            m_fireFlies[i]->setIsCollected(true);
            m_finalFireFlyCount[m_currLevel]++;
        }
        
        if (!m_waterFlies[i]->getIsCollected() &&
            m_waterfrog.getSprite().getGlobalBounds().intersects(m_waterFlies[i]->getSprite().getGlobalBounds()))
        {
            m_waterFlies[i]->setIsCollected(true);
            m_finalWaterFlyCount[m_currLevel]++;
        }
    }
}

// Initialise all the text for the intro and outro screens that never change
void Game::initText()
{
    sf::Font tempFont;
    if (!tempFont.loadFromFile("Assets/SuperFuntime.ttf"))
    {
        system("pause");
    }
    m_fonts.push_back(tempFont);
    if (!tempFont.loadFromFile("Assets/MouldyCheese.ttf"))
    {
        system("pause");
    }
    m_fonts.push_back(tempFont);
    if (!tempFont.loadFromFile("Assets/SuperWoobly.ttf"))
    {
        system("pause");
    }
    m_fonts.push_back(tempFont);

    // intro text (0-3)
    sf::Text tempText;
    tempText.setFont(m_fonts[Font::superFuntime]);
    tempText.setString("Firetoad & Waterfrog");
    tempText.setCharacterSize(96);
    tempText.setFillColor(sf::Color::White);
    tempText.setPosition(sf::Vector2f(m_screenSize.x / 2 - tempText.getGlobalBounds().width / 2, 224.f));
    m_introText.push_back(tempText);

    tempText.setFont(m_fonts[Font::mouldyCheese]);
    tempText.setString("in");
    tempText.setCharacterSize(64);
    tempText.setFillColor(sf::Color::White);
    tempText.setPosition(sf::Vector2f(m_screenSize.x / 2 - tempText.getGlobalBounds().width / 2, 256.f + 96.f));
    m_introText.push_back(tempText);

    tempText.setFont(m_fonts[Font::superWoobly]);
    tempText.setString("The Swamp Realm");
    tempText.setCharacterSize(96);
    tempText.setFillColor(sf::Color::Green);
    tempText.setPosition(sf::Vector2f(m_screenSize.x / 2 - tempText.getGlobalBounds().width / 2, 256.f + 192.f));
    m_introText.push_back(tempText);

    tempText.setFont(m_fonts[Font::mouldyCheese]);
    tempText.setString("Press ENTER to play");
    tempText.setCharacterSize(64);
    tempText.setFillColor(sf::Color::White);
    tempText.setPosition(sf::Vector2f(m_screenSize.x / 2 - tempText.getGlobalBounds().width / 2, 256.f + 448.f));
    m_introText.push_back(tempText);

    // outro text (0-5)
    tempText.setFont(m_fonts[Font::superFuntime]);
    tempText.setString("You made it!");
    tempText.setCharacterSize(96);
    tempText.setFillColor(sf::Color::White);
    tempText.setPosition(sf::Vector2f(m_screenSize.x / 2 - tempText.getGlobalBounds().width / 2, 32.f));
    m_outroText.push_back(tempText);
    
    tempText.setFont(m_fonts[Font::mouldyCheese]);
    tempText.setString("Level 1");
    tempText.setCharacterSize(64);
    tempText.setFillColor(sf::Color::White);
    tempText.setPosition(sf::Vector2f(m_screenSize.x / 4 - tempText.getGlobalBounds().width / 2, 160.f + 32.f));
    m_outroText.push_back(tempText);

    tempText.setFont(m_fonts[Font::mouldyCheese]);
    tempText.setString("Level 2");
    tempText.setCharacterSize(64);
    tempText.setFillColor(sf::Color::White);
    tempText.setPosition(sf::Vector2f(m_screenSize.x / 4 * 3 - tempText.getGlobalBounds().width / 2, 160.f + 32.f));
    m_outroText.push_back(tempText);

    tempText.setFont(m_fonts[Font::superWoobly]);
    tempText.setString("Thanks for playing");
    tempText.setCharacterSize(64);
    tempText.setFillColor(sf::Color::Green);
    tempText.setPosition(sf::Vector2f(m_screenSize.x / 2 - tempText.getGlobalBounds().width / 2, 640.f));
    m_outroText.push_back(tempText);
    
    tempText.setFont(m_fonts[Font::mouldyCheese]);
    tempText.setString("Press R to restart");
    tempText.setCharacterSize(32);
    tempText.setFillColor(sf::Color::White);
    tempText.setPosition(sf::Vector2f(m_screenSize.x / 2 - tempText.getGlobalBounds().width / 2, 640.f + 128.f));
    m_outroText.push_back(tempText);

    tempText.setFont(m_fonts[Font::mouldyCheese]);
    tempText.setString("Press ESC to quit");
    tempText.setCharacterSize(32);
    tempText.setFillColor(sf::Color::White);
    tempText.setPosition(sf::Vector2f(m_screenSize.x / 2 - tempText.getGlobalBounds().width / 2, 640.f + 192.f));
    m_outroText.push_back(tempText);


    // end game text (6-11)
    for (int i = 0; i < 8; i++)
    {
        tempText.setFont(m_fonts[Font::mouldyCheese]);
        tempText.setCharacterSize(32);
        tempText.setFillColor(sf::Color::White);
        m_outroText.push_back(tempText);
    }
}

// Sets all the texts in the outro screen based on the user's performance in the two levels
void Game::endGameText() 
{
    m_outroText[6].setString(to_string(m_finalFireFlyCount[0]) + "/5");
    m_outroText[6].setPosition(sf::Vector2f(m_screenSize.x / 4 - m_outroText[6].getGlobalBounds().width / 2 + 32.f, 256.f + 32.f));

    m_outroText[7].setString(to_string(m_finalFireFlyCount[1]) + "/5");
    m_outroText[7].setPosition(sf::Vector2f(m_screenSize.x / 4 * 3 - m_outroText[7].getGlobalBounds().width / 2 + 32.f, 256.f + 32.f));

    m_outroText[8].setString(to_string(m_finalWaterFlyCount[0]) + "/5");
    m_outroText[8].setPosition(sf::Vector2f(m_screenSize.x / 4 - m_outroText[8].getGlobalBounds().width / 2 + 32.f, 320.f + 32.f));

    m_outroText[9].setString(to_string(m_finalWaterFlyCount[1]) + "/5");
    m_outroText[9].setPosition(sf::Vector2f(m_screenSize.x / 4 * 3 - m_outroText[9].getGlobalBounds().width / 2 + 32.f, 320.f + 32.f));

    m_outroText[10].setString("Time : " + secondsToMinutes(m_levelTimes[0]));
    m_outroText[10].setPosition(sf::Vector2f(m_screenSize.x / 4 - m_outroText[10].getGlobalBounds().width / 2, 384.f + 32.f));

    m_outroText[11].setString("Time : " + secondsToMinutes(m_levelTimes[1]));
    m_outroText[11].setPosition(sf::Vector2f(m_screenSize.x / 4 * 3 - m_outroText[11].getGlobalBounds().width / 2, 384.f + 32.f));

    m_outroText[12].setString("Rank : " + rankCalculation(0));
    m_outroText[12].setPosition(sf::Vector2f(m_screenSize.x / 4 - m_outroText[12].getGlobalBounds().width / 2, 480.f + 32.f));
    
    m_outroText[13].setString("Rank : " + rankCalculation(1));
    m_outroText[13].setPosition(sf::Vector2f(m_screenSize.x / 4 * 3 - m_outroText[13].getGlobalBounds().width / 2, 480.f + 32.f));

}

string Game::secondsToMinutes(float time)
{
    int hours = static_cast<int>(time / 60);
    int minutes = static_cast<int>(time) % 60;

    return to_string(hours) + ":" + (minutes >= 10 ? "" : "0") + to_string(minutes);
}

// Calculate letter grade ABC for a given level
string Game::rankCalculation(int level)
{
    string rank = "";

    float time = m_levelTimes[level];
    int nbFireFlies = m_finalFireFlyCount[level];
    int nbWaterFlies = m_finalWaterFlyCount[level];
    
    float timeA = 0.f;
    float timeB = 0.f;

    switch (level)
    {
    case 0:
        timeA = 35.f;
        timeB = 50.f;
        break;

    case 1:
        timeA = 60.f;
        timeB = 105.f;
        break;
    }

    if (time <= timeA)
    {
        if (nbFireFlies < m_nbFlies || nbWaterFlies < m_nbFlies)
        {
            rank = "B";
        }
        else
        {
            rank = "A";
        }
    }
    else if (time <= timeB)
    {
        rank = "B";
    }
    else
    {
        rank = "C";
    }

    return rank;
}

