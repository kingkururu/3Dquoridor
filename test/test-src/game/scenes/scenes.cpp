#include "scenes.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// Base scene functions  
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

// Scene constructure sets up window and sprite respawn times 
Scene::Scene( sf::RenderWindow& gameWindow ) : window(gameWindow), quadtree(0.0f, 0.0f, Constants::WORLD_WIDTH, Constants::WORLD_HEIGHT){ 
    MetaComponents::view = sf::View(Constants::VIEW_RECT); 
    log_info("scene made"); 
}

void Scene::runScene() {
    if (FlagSystem::flagEvents.gameEnd) return; // Early exit if game ended
    
    setTime();

    handleInput();

    respawnAssets();

    handleGameEvents();
    handleGameFlags();
    handleSceneFlags();

    update();
    draw();
}

void Scene::draw(){
    window.clear(sf::Color::Black);
    window.display(); 
 }

void Scene::moveViewPortWASD(){
    // move view port 
    if(FlagSystem::flagEvents.aPressed){
        MetaComponents::view.move(sf::Vector2f(-10, 0)); 
    }
    if(FlagSystem::flagEvents.dPressed){
        MetaComponents::view.move(sf::Vector2f(10, 0)); 
    }
    if(FlagSystem::flagEvents.sPressed){
        MetaComponents::view.move(sf::Vector2f(0, 10)); 
    }
    if(FlagSystem::flagEvents.wPressed){
        MetaComponents::view.move(sf::Vector2f(0, -10)); 
    }
}

// Resets everything for scene to start again. The position, moveState, flagEvents, etc are all reset 
void Scene::restartScene() {
    // re-play background music

    // set sprite movestates to true
   
    // re-set sprite and text positions 

    // clear respawn time vectors or any other unecessary vectors 

    // re-set flagEvents
    sceneEvents.resetFlags(); 
}

// Handles events from flagEvents
void Scene::handleGameFlags(){
    // if flagEvents.gameEnd is true or some event ... do somthing 
    if(FlagSystem::flagEvents.gameEnd){
       
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// Game Scene #1 down below 
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

// Gets called once before the main game loop 
void gamePlayScene::createAssets() {
    try {
        globalTimer.Reset();  

        std::weak_ptr<sf::Uint8[]> cloudBlueBitmaskWeakPtr = Constants::CLOUDBLUE_BITMASK;  
        cloudBlue.push_back(std::make_unique<Cloud>(Constants::CLOUDBLUE_POSITION, Constants::CLOUDBLUE_SCALE, Constants::CLOUDBLUE_TEXTURE, Constants::CLOUDBLUE_SPEED, Constants::CLOUDBLUE_ACCELERATION, cloudBlueBitmaskWeakPtr));
        
        std::weak_ptr<sf::Uint8[]> cloudPurpleBitmaskWeakPtr = Constants::CLOUDPURPLE_BITMASK;  
        cloudPurple.push_back(std::make_unique<Cloud>(Constants::CLOUDPURPLE_POSITION, Constants::CLOUDBLUE_SCALE, Constants::CLOUDPURPLE_TEXTURE, Constants::CLOUDBLUE_SPEED, Constants::CLOUDBLUE_ACCELERATION, cloudPurpleBitmaskWeakPtr));

        // Initialize sprites and music here 
        background = std::make_unique<Background>(Constants::BACKGROUND_POSITION, Constants::BACKGROUND_SCALE, Constants::BACKGROUND_TEXTURE);
        
        player = std::make_unique<Player>(Constants::SPRITE1_POSITION, Constants::SPRITE1_SCALE, Constants::SPRITE1_TEXTURE, Constants::SPRITE1_SPEED, Constants::SPRITE1_ACCELERATION, 
                                          Constants::SPRITE1_ANIMATIONRECTS, Constants::SPRITE1_INDEXMAX, utils::convertToWeakPtrVector(Constants::SPRITE1_BITMASK));
        player->setRects(0); 

        backgroundMusic = std::make_unique<MusicClass>(std::move(Constants::BACKGROUNDMUSIC_MUSIC), Constants::BACKGROUNDMUSIC_VOLUME);
      //  if(backgroundMusic) backgroundMusic->returnMusic().play(); 
       
        button1 = std::make_unique<Button>(Constants::BUTTON1_POSITION, Constants::BUTTON1_SCALE, Constants::BUTTON1_TEXTURE, 
                                   Constants::BUTTON1_ANIMATIONRECTS, Constants::BUTTON1_INDEXMAX, 
                                   utils::convertToWeakPtrVector(Constants::BUTTON1_BITMASK));
        button1->setRects(0); 
        
        // Initialize individual Tiles in the array
        for (int i = 0; i < Constants::TILES_NUMBER; ++i) {
            tiles1.at(i) = std::make_shared<Tile>(Constants::TILES_SCALE, Constants::TILES_TEXTURE, Constants::TILES_SINGLE_RECTS[i], Constants::TILES_BITMASKS[i], Constants::TILES_BOOLS[i]); 
        }
        tileMap1 = std::make_unique<TileMap>(tiles1.data(), Constants::TILES_NUMBER, Constants::TILEMAP_WIDTH, Constants::TILEMAP_HEIGHT, Constants::TILE_WIDTH, Constants::TILE_HEIGHT, Constants::TILEMAP_FILEPATH, Constants::TILEMAP_POSITION); 

        playerJumpSound = std::make_unique<SoundClass>(Constants::PLAYERJUMP_SOUNDBUFF, Constants::PLAYERJUMPSOUND_VOLUME); 
          
        text1 = std::make_unique<TextClass>(Constants::TEXT_POSITION, Constants::TEXT_SIZE, Constants::TEXT_COLOR, Constants::TEXT_FONT, Constants::TEXT_MESSAGE);
        
        globalTimer.End("initializing assets in scene 1"); 

        insertItemsInQuadtree(); 

        cloudBlueRespawnTime = Constants::CLOUDBLUE_INITIAL_RESPAWN_TIME;
        cloudPurpleRespawnTime = Constants::CLOUDPURPLE_INITIAL_RESPAWN_TIME;
    } 

    catch (const std::exception& e) {
        log_error("Exception in createAssets: " + std::string(e.what()));
    }
}

void gamePlayScene::insertItemsInQuadtree(){
    quadtree.insert(player);  
    quadtree.insert(button1); 

    //
    for (auto &cloud : cloudBlue) quadtree.insert(cloud);
    for (auto &cloud : cloudPurple) quadtree.insert(cloud);
}

void gamePlayScene::respawnAssets(){
    // use existing sprites or make new ones with pre-made textures
    if(cloudBlueRespawnTime <= 0){
        float newCloudBlueInterval = Constants::CLOUDBLUE_INITIAL_RESPAWN_TIME - MetaComponents::globalTime * 0.38;
        std::weak_ptr<sf::Uint8[]> cloudBlueBitmaskWeakPtr = Constants::CLOUDBLUE_BITMASK;  
        cloudBlue.push_back(std::make_unique<Cloud>(Constants::makeRandomPositionCloud(), Constants::CLOUDBLUE_SCALE, Constants::CLOUDBLUE_TEXTURE, Constants::CLOUDBLUE_SPEED, Constants::CLOUDBLUE_ACCELERATION, cloudBlueBitmaskWeakPtr));
        
        cloudBlueRespawnTime = std::max(newCloudBlueInterval, Constants::CLOUDBLUE_INITIAL_RESPAWN_TIME);
        std::cout << "made blue cloud" << std::endl;
    }
    if(cloudPurpleRespawnTime <= 0){
        float newCloudPurpleInterval = Constants::CLOUDPURPLE_INITIAL_RESPAWN_TIME - MetaComponents::globalTime * 0.38;
        std::weak_ptr<sf::Uint8[]> cloudPurpleBitmaskWeakPtr = Constants::CLOUDPURPLE_BITMASK;  
        cloudPurple.push_back(std::make_unique<Cloud>(Constants::makeRandomPositionCloud(), Constants::CLOUDBLUE_SCALE, Constants::CLOUDPURPLE_TEXTURE, Constants::CLOUDBLUE_SPEED, Constants::CLOUDBLUE_ACCELERATION, cloudPurpleBitmaskWeakPtr));
        
        cloudPurpleRespawnTime = std::max(newCloudPurpleInterval, Constants::CLOUDPURPLE_INITIAL_RESPAWN_TIME);
        std::cout << "made purple cloud" << std::endl;
    }
} 

void gamePlayScene::deleteInvisibleSprites() {
    // maybe do a sprite pooling 
}

/* Updating time from GameManager's deltatime; it updates sprite respawn times and also counts 
elapsed times from when bullets were spawned and when space button was pressed */
void gamePlayScene::setTime(){
    // count respawn time here 
    if (FlagSystem::flagEvents.spacePressed){
        MetaComponents::spacePressedElapsedTime += MetaComponents::deltaTime; 
    } else {
        MetaComponents::spacePressedElapsedTime = 0.0f; 
    }
    
    cloudBlueRespawnTime -= MetaComponents::deltaTime; 
    cloudPurpleRespawnTime -= MetaComponents::deltaTime;
} 

void gamePlayScene::handleInput() {
    handleMouseClick();
    handleSpaceKey(); 
    handleMovementKeys();
}

void gamePlayScene::handleMouseClick() {    
    if (FlagSystem::flagEvents.mouseClicked) {
        if (button1->getVisibleState() && 
            physics::collisionHelper(button1, MetaComponents::mouseClickedPosition_f)) {
            log_info("button clicked");

            button1->setClickedBool(true);
            FlagSystem::gameScene1Flags.sceneEnd = true;
            FlagSystem::gameSceneNextFlags.sceneStart = true;
            FlagSystem::gameSceneNextFlags.sceneEnd = false;

            window.clear();
        }
        if (FlagSystem::gameScene1Flags.playerFalling){
            player->changePosition(Constants::SPRITE1_POSITION); 
        }
    }
}

void gamePlayScene::handleSpaceKey() {
    if (FlagSystem::flagEvents.spacePressed) {
        if (player->getMoveState() && !FlagSystem::gameScene1Flags.playerFalling) {
            physics::spriteMover(player, physics::jump, MetaComponents::spacePressedElapsedTime, Constants::SPRITE1_JUMP_ACCELERATION);
            MetaComponents::view.move(3, 0);
        } 
    }
}

void gamePlayScene::handleMovementKeys() {
    sf::FloatRect background1Bounds = background->getViewBounds(background->returnSpritesShape());
    sf::FloatRect background2Bounds = background->getViewBounds(background->returnSpritesShape2());
    sf::FloatRect viewBounds = MetaComponents::getViewBounds();

    // Left movement
    if (FlagSystem::flagEvents.aPressed) {
        if (!physics::collisionHelper(player, tileMap1) || player->getSpritePos().x > tileMap1->getTileMapPosition().x) {
            physics::spriteMover(player, physics::moveLeft);
        }
    }
    // Right movement
    if (FlagSystem::flagEvents.dPressed) {
        if (!physics::collisionHelper(player, tileMap1) || player->getSpritePos().x + player->getRects().width < tileMap1->getTileMapPosition().x + tileMap1->getTileMapWidth() * tileMap1->getTileWidth()) {
            physics::spriteMover(player, physics::moveRight);
        }
    }
    // Down movement
    if (FlagSystem::flagEvents.sPressed) {
        if (!physics::collisionHelper(player, tileMap1) || player->getSpritePos().y + player->getRects().height < tileMap1->getTileMapPosition().y + tileMap1->getTileMapHeight() * tileMap1->getTileHeight()) {
            physics::spriteMover(player, physics::moveDown);
        }
    }
    // Up movement
    if (FlagSystem::flagEvents.wPressed) {
        if (!physics::collisionHelper(player, tileMap1) || player->getSpritePos().y > tileMap1->getTileMapPosition().y) {
            physics::spriteMover(player, physics::moveUp);
        }
        if (viewBounds.top > background1Bounds.top || viewBounds.top > background2Bounds.top) {
           // MetaComponents::view.move(sf::Vector2f(0, -1));
        }
    }
}

// Keeps sprites inside screen bounds, checks for collisions, update scores, and sets flagEvents.gameEnd to true in an event of collision 
void gamePlayScene::handleGameEvents() { 
  //  if (player) physics::spriteMover(player, physics::moveRight); 

    FlagSystem::gameScene1Flags.playerFalling = !physics::collisionHelper(player, tileMap1) && !FlagSystem::gameScene1Flags.playerJumping; // player must be not colliding with the tilemap, and it must not be jumping
    FlagSystem::gameScene1Flags.playerJumping = (MetaComponents::spacePressedElapsedTime > 0); 
} 

void gamePlayScene::handleSceneFlags(){
    if(FlagSystem::gameScene1Flags.playerFalling) physics::spriteMover(player, physics::freeFall); 
}

void gamePlayScene::update() {
    try {
        updateEntityStates();
        changeAnimation();
        updateDrawablesVisibility(); 
        deleteInvisibleSprites();

        updatePlayerAndView(); 
        quadtree.update(); 

        // Set the view for the window
        window.setView(MetaComponents::view);
        
    } catch (const std::exception& e) {
        log_error("Exception in updateSprites: " + std::string(e.what()));
    }
}

void gamePlayScene::updateEntityStates(){ // manually change the sprite's state
    player->setJumpingState(FlagSystem::gameScene1Flags.playerJumping);
    player->setFallingState(FlagSystem::gameScene1Flags.playerFalling); 
}

void gamePlayScene::changeAnimation(){ // change animation for sprites. change animation for texts if necessary 
    if (button1) button1->changeAnimation(); 
    if (background) background->updateBackground(Constants::BACKGROUND_SPEED, Constants::BACKGROUND_MOVING_DIRECTION);
    if (player) player->changeAnimation();
}

void gamePlayScene::updatePlayerAndView() {
    if(player->getJumpingState() || player->getFallingState()) return; // don't make the view focus on player if player is jumping or falling

    // // Get the player's position
    // sf::Vector2f playerPos = player->getSpritePos();

    // // Calculate the view size and game world boundaries
    // sf::Vector2f viewSize = MetaComponents::view.getSize();
    // sf::FloatRect worldBounds(0, 0, Constants::WORLD_WIDTH, Constants::WORLD_HEIGHT);

    // // Default view center is the player's position
    // sf::Vector2f viewCenter = playerPos;

    // // Check if the player is near the edges of the game world
    // if (playerPos.x < viewSize.x / 2.0f) {
    //     viewCenter.x = viewSize.x / 2.0f; // Lock the view to the left edge
    // } else if (playerPos.x > worldBounds.width - viewSize.x / 2.0f) {
    //     viewCenter.x = worldBounds.width - viewSize.x / 2.0f; // Lock the view to the right edge
    // }

    // if (playerPos.y < viewSize.y / 2.0f) {
    //     viewCenter.y = viewSize.y / 2.0f; // Lock the view to the top edge
    // } else if (playerPos.y > worldBounds.height - viewSize.y / 2.0f) {
    //     viewCenter.y = worldBounds.height - viewSize.y / 2.0f; // Lock the view to the bottom edge
    // }

    // // Offset to keep the player slightly off-center vertically
    // viewCenter.y -= Constants::SPRITE_OUT_OF_BOUNDS_OFFSET;

    // // Update the view center
    // MetaComponents::view.setCenter(viewCenter);
     // Calculate the center of the view based on the player's position
    float viewCenterX = player->getSpritePos().x;
    float viewCenterY = player->getSpritePos().y;

    // Set the new view center to follow the player's position
    MetaComponents::view.setCenter(viewCenterX, viewCenterY - Constants::SPRITE_OUT_OF_BOUNDS_OFFSET);
}


void gamePlayScene::updateDrawablesVisibility(){
    try{
        if (button1) button1->setVisibleState(physics::collisionHelper(button1, MetaComponents::view)); // set button's visibility if it is on or off screen 
    }
    catch(const std::exception & e){
        log_error("Exception in updateDrawablesVisibility: " + std::string(e.what()));
    }
}

// Draws only the visible sprite and texts
void gamePlayScene::draw() {
    try {
        window.clear(sf::Color::Blue); // set the base baskground color blue

        if (background && background->getVisibleState()) {
            window.draw(*background); 
        }

        if(tileMap1) window.draw(*tileMap1);

        if (button1 && button1->getVisibleState()) {
            window.draw(*button1); 
        }
        
        if (player && player->getVisibleState()) {
            window.draw(*player); 
        }

        for (auto &cloud : cloudBlue) {
            if (cloud && cloud->getVisibleState()) {
                window.draw(*cloud); 
            }
        }

        for (auto &cloud : cloudPurple) {
            if (cloud && cloud->getVisibleState()) {
                window.draw(*cloud); 
            }
        }

        if(text1) window.draw(*text1); 

        window.display(); 
    } 
    
    catch (const std::exception& e) {
         log_error("Exception in draw: " + std::string(e.what()));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// Game Scene #2 from down below 
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void gamePlayScene2::createAssets() {
 try {
        // Initialize sprites and music here 
        background = std::make_unique<Background>(Constants::BACKGROUND_POSITION, Constants::BACKGROUND_SCALE, Constants::BACKGROUND_TEXTURE2);
    } 

    catch (const std::exception& e) {
        log_error("Exception in createAssets: " + std::string(e.what()));
    }
}

void gamePlayScene2::handleInput() {
    moveViewPortWASD(); // change position of the view port based on keyboard input flags
}

void gamePlayScene2::draw() {
    try {
        window.clear(); // clear elements from previous screen 

        if (background && background->getVisibleState()) window.draw(*background); 
    
        window.display(); 
    } 
    
    catch (const std::exception& e) {
        log_error("Exception in gamePlayScene2 draw: " + std::string(e.what()));
    }
}

void gamePlayScene2::update() {
    try {
        deleteInvisibleSprites(); // do a sprite pooling or actually delete all

        window.setView(MetaComponents::view); 
        
    }
    catch (const std::exception& e) {
        log_error("Exception in updateSprites: " + std::string(e.what()));
    }
}
