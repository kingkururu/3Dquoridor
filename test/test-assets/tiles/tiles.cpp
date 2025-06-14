#include "tiles.hpp"

Tile::Tile(sf::Vector2f scale, std::weak_ptr<sf::Texture> texture, sf::IntRect textureRect, 
           std::weak_ptr<sf::Uint8[]> bitmask, bool walkable)
    : scale(scale), texture(texture), textureRect(textureRect), bitmask(bitmask), walkable(walkable) {
    
    try {
        tileSprite = std::make_unique<sf::Sprite>(); // Use unique_ptr for tileSprite

        if (auto sharedTexture = texture.lock()) {
            sf::Vector2u textureSize = sharedTexture->getSize(); 
            if (textureSize.x == 0 || textureSize.y == 0) {
                throw std::runtime_error("Loaded tile texture has size 0");
            }

            tileSprite->setTexture(*sharedTexture); // Set the texture
            tileSprite->setScale(scale); // Set the scale
            tileSprite->setTextureRect(textureRect); // Set the texture rectangle
        } else {
            throw std::runtime_error("Tile texture is not available");
        }
    } catch (const std::exception& e) {
        log_error(e.what()); // Log any exceptions that occur
    }
}

// Custom copy constructor
Tile::Tile(const Tile& other)
    : position(other.position), scale(other.scale),
      texture(other.texture), textureRect(other.textureRect),
      bitmask(other.bitmask), walkable(other.walkable) {

    // Create a new sprite with the same texture and scale
    tileSprite = std::make_unique<sf::Sprite>();

    // Check if the texture is still valid
    if (auto texturePtr = other.texture.lock()) {
        // Set the texture and texture rectangle for the new sprite
        tileSprite->setTexture(*texturePtr);
        tileSprite->setTextureRect(other.textureRect);
        tileSprite->setScale(scale); // Apply the scale
        tileSprite->setPosition(other.tileSprite->getPosition()); // Copy the position if needed
    } else {
        throw std::runtime_error("Texture for copied tile is not available");
    }
}
 
TileMap::TileMap(std::shared_ptr<Tile>* tileTypesArray, unsigned int tileTypesNumber, size_t tileMapWidth, size_t tileMapHeight, float tileWidth, float tileHeight, std::filesystem::path filePath, sf::Vector2f tileMapPosition) 
    : tileTypesNumber(tileTypesNumber), tileMapWidth(tileMapWidth), tileMapHeight(tileMapHeight), tileWidth(tileWidth), tileHeight(tileHeight), tileMapPosition(tileMapPosition) {

    try{
        tiles.reserve( tileMapWidth * tileMapHeight ); 

        std::ifstream fileStream(filePath);
        
        if (!fileStream.is_open()) {
            throw std::runtime_error("Unable to open file: " + filePath.string());
        }

        std::string line;
        unsigned int currentY = 0; // Track the current row

        while (std::getline(fileStream, line) && currentY < tileMapHeight) {
            std::istringstream lineStream(line);
            std::string tileIndexStr;
            unsigned int currentX = 0; // Track the current column

            while (lineStream >> tileIndexStr && currentX < tileMapWidth) {
                unsigned int tileIndex = std::stoul(tileIndexStr); // Convert to unsigned int
                
                if (tileIndex < tileTypesNumber) {
                    auto tile = tileTypesArray[tileIndex]->clone();
                    tile->getTileSprite().setPosition(tileMapPosition.x + currentX * tileWidth, tileMapPosition.y + currentY * tileHeight); 
                    tiles.emplace_back(std::move(tile));
                    
                } else {
                    throw std::out_of_range("Tile index out of bounds: " + std::to_string(tileIndex));
                }
                currentX++; // Increment column index
            } 
            currentY++; // Increment row index
        }

        fileStream.close();

        log_info("Tile map initialized successfully");
    } catch (const std::exception& e) {
        log_warning("Error in making tilemap: " + std::string(e.what()));
    }
}

BoardTileMap::BoardTileMap(std::array<std::shared_ptr<Tile>, 4> tileTypesArr) { 
    // tileTypesArr[0] = wall tile
    // tileTypesArr[1] = path tile  
    // tileTypesArr[2] = goal tile for p1
    // tileTypesArr[3] = goal tile for p2

    this->tileTypesArr = tileTypesArr;

    wallTileSize = sf::Vector2i{tileTypesArr[0]->getTileSprite().getTextureRect().width, tileTypesArr[0]->getTileSprite().getTextureRect().height};
    pathTileSize = sf::Vector2i{tileTypesArr[1]->getTileSprite().getTextureRect().width, tileTypesArr[1]->getTileSprite().getTextureRect().height};
    goalTileSize = sf::Vector2i{tileTypesArr[2]->getTileSprite().getTextureRect().width, tileTypesArr[2]->getTileSprite().getTextureRect().height}; // should be the same for tile at index3, for player 2
    
    for(int i = 0; i < 21; ++i) tiles[i] = tileTypesArr[0]; // Initialize first row to wall tiles
    for(int i = 378; i < 399; ++i) tiles[i] = tileTypesArr[0]; // Initialize bottom row to wall tiles

    for(int i = 21; i < 378; i += 21) {
        tiles[i] = tileTypesArr[2]; // Initialize left column / starting index for p1
        tiles[i + 20] = tileTypesArr[3]; // Initialize right column / starting index for p2
    }

    for(int i = 21; i < 278; ++i) {
        if(tiles[i] == nullptr) {
            tiles[i] = tileTypesArr[1]; // Initialize inner tiles to path tiles
        }
    }

    log_info("BoardTileMap initialized");
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& tile : tiles) {
        if (tile) {
            target.draw(tile->getTileSprite(), states);
        }
    }
}

// Add a tile to the map at the specified grid position (x, y)
void TileMap::addTile(unsigned int x, unsigned int y, std::unique_ptr<Tile> tile) {
    try{
        if (x >= tileMapWidth || y >= tileMapHeight) {
            throw std::out_of_range("Tile position out of bounds: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
        }

        // Calculate the index in the tiles vector
        unsigned int index = y * tileMapWidth + x;

        // Ensure the tile is valid (not nullptr)
        if (!tile) {
            throw std::runtime_error("Attempted to add a null tile.");
        }

        // Move the tile into the tiles vector at the calculated index
        tiles[index] = std::move(tile);

        // Optionally set the position of the tile if the Tile class has a method for that
        tiles[index]->getTileSprite().setPosition(tileMapPosition.x + x * tileWidth, tileMapPosition.y + y * tileHeight);
    } catch (const std::exception& e) {
        log_error(e.what()); // Log any exceptions that occur
    }
}

std::unique_ptr<Tile>& TileMap::getTile(size_t index) {
    if (index < tiles.size()) {
        return tiles[index]; // Return the tile at the specified index
    } else {
        // Handle the case where the index is out of bounds, throw an exception or return a nullptr
        throw std::out_of_range("Index is out of range in getTile");
    }
}