//
//  tilemap.hpp
//
//

#pragma once

#include <stdio.h>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../../test-logging/log.hpp"


class Tile {
public:
    // Constructor with position, scale, texture, and a texture rect (to support tilesets)
    explicit Tile(sf::Vector2f scale, std::weak_ptr<sf::Texture> texture, sf::IntRect textureRect, std::weak_ptr<sf::Uint8[]> bitmask, bool walkableState = true); 
    sf::Sprite& getTileSprite() const { return *tileSprite; } 

    sf::IntRect const getTextureRect() const { return textureRect; }
    std::weak_ptr<sf::Uint8[]>  const getBitMask() const { return bitmask; }
 
    bool getWalkable() const { return walkableState; }
    void setWalkable(bool walkableState) { this->walkableState = walkableState; }

    bool getVisibleState() const { return visibleState; }
    void setVisibleState(bool visibleState) { this->visibleState = visibleState; }
    
    // making copies for use in tilemap
    virtual std::unique_ptr<Tile> clone() const {
        return std::unique_ptr<Tile>(new Tile(*this)); // Create a new instance with the same properties
    }
    virtual ~Tile() {} 
    Tile(const Tile& other); 

private:
    sf::Vector2f position {};
    std::unique_ptr<sf::Sprite> tileSprite {};
    sf::Vector2f scale {};
    std::weak_ptr<sf::Texture> texture;
    sf::IntRect textureRect {};   // Texture portion for this tile
    std::weak_ptr<sf::Uint8[]> bitmask; 
    bool walkableState {};
    bool visibleState {};
};

class TileMap : public sf::Drawable {
public:
    // Constructor now accepts a shared_ptr to a default tile, and initializes the map with it
    explicit TileMap(std::shared_ptr<Tile>* tileTypesArray, unsigned int tileTypesNumber, size_t tileMapWidth, size_t tileMapHeight, float tileWidth, float tileHeight, std::filesystem::path filePath, sf::Vector2f tileMapPosition);

    ~TileMap() = default;
    
    // Add a tile to the map at the specified grid position (x, y)
    void addTile(unsigned int x, unsigned int y, std::unique_ptr<Tile> tile); 
    float const getTileWidth() const { return tileWidth; }
    float const getTileHeight() const { return tileHeight; }
    size_t const getTileMapWidth() const { return tileMapWidth; }
    size_t const getTileMapHeight() const { return tileMapHeight; }
    sf::Vector2f const getTileMapPosition() const { return tileMapPosition; }
    unsigned int const getTileTypesNumber() const { return tileTypesNumber; }
    bool const getVisibleState() const { return visibleState; }
    void setVisibleState(bool newVisibleState) { visibleState = newVisibleState; }
    std::unique_ptr<Tile>& getTile(size_t index);

private:
    unsigned int tileTypesNumber {};
    size_t tileMapWidth{};
    size_t tileMapHeight{}; 

    float tileWidth {};
    float tileHeight {};

    std::vector<std::unique_ptr<Tile>> tiles; 
    sf::Vector2f tileMapPosition; 
    bool visibleState = true;

    // Override the draw function of sf::Drawable to draw all tiles
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

class BoardTileMap : public sf::Drawable {
public:
    explicit BoardTileMap(std::array<std::shared_ptr<Tile>, 11> tileTypesArr, size_t rowsTotal, size_t colsTotal); // need wall tile index, path tile index, p1 start tile index, p2 start tile index
    bool const getVisibleState() const { return true; } // entire board, not each tiles
    std::shared_ptr<Tile>& getTile(size_t index);
    size_t getTileMapNumber() const { return tiles.size(); } // returns the number of tiles in the board (483)
    size_t getTileIndex(sf::Vector2i position); 
    size_t getTileIndex(sf::Vector2f postiion);
    bool isGreyTile(size_t index) const; 
    bool isVerticalWallTile(size_t index) const;
    bool isP1StartTile(size_t index) const;
    bool isP2StartTile(size_t index) const;

private:
    size_t rowsTotal {};
    size_t colsTotal {};
    std::array<std::shared_ptr<Tile>, 399> tiles; // board with 19 x 21 tiles including walls
    std::array<std::shared_ptr<Tile>, 11> tileTypesArr; // wall, path, goal, additional tile type
    sf::Vector2i wallTileXSize; 
    sf::Vector2i wallTileYSize;
    sf::Vector2i blankWallTileSize;
    sf::Vector2i pathTileSize; // size of path tile
    sf::Vector2i goalTileSize; // size of goal tile for p1 and p2
    sf::Vector2i blankp1TileSize; // size of blank tile for p1
    sf::Vector2i blankp2TileSize; // size of blank tile for p

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override; // temporary
};

