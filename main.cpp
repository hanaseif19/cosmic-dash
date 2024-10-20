#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <vector>
#include <ctime>
#include <SDL.H>
#include <SDL_mixer.h>
//-------------------------------- GENERAL CONFIGURATIONS-----------------------------------
int windowWidth = 700;
int windowHeight = 700;
float elapsedTime = 0.0f; // to keep track of time to display it throughout the game
bool gameEnded = false; // Variable to check if the game has ended
bool gameStarted = false; // Game state variable
std::chrono::time_point<std::chrono::steady_clock> recoilStartTime;
bool recoilActive = false;
//-------------------------------- GENERAL CONFIGURATIONS-----------------------------------
//-------------------------------- SOUND CONFIGURATIONS-----------------------------------
Uint32 freeSoundEffect(Uint32 interval, void* param);

Mix_Music* backgroundMusic = nullptr;
// to initialize the sdl mixer library and start playing the background music @volume 45 out of a maximum of 128
void initAudio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    } else {
        backgroundMusic = Mix_LoadMUS("gamebackgroundmusic.mp3"); // Load your background music
        if (backgroundMusic == NULL) {
            printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
        }
    }
    gameStarted = true;
    elapsedTime = 0.0f;
    Mix_VolumeMusic(45);
}
void playBackgroundMusic() {
    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(backgroundMusic, -1); // infinitely
    }
}
void cleanupAudio() {
    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
}

// Global variable to hold sound effect pointer (can be static)
Mix_Chunk* currentSoundEffect = nullptr;
void playSoundForDuration(const char* fileName, int durationMs, int volume) {
    // Load the sound file
    Mix_Chunk* soundEffect = Mix_LoadWAV(fileName);
    if (soundEffect == NULL) {
        std::cout << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return;
    }

    Mix_VolumeChunk(soundEffect, volume); // Set volume
    Mix_PlayChannel(-1, soundEffect, 0); // Play sound on the first available channel

    SDL_AddTimer(durationMs, freeSoundEffect, soundEffect); // Pass the sound effect to free it later
}

// Callback function to free the sound effect after the duration
Uint32 freeSoundEffect(Uint32 interval, void* param) {
    Mix_Chunk* soundEffect = static_cast<Mix_Chunk*>(param); // Cast param back to Mix_Chunk*
    Mix_FreeChunk(soundEffect); // Free the sound effect
    return 0; // One-time timer, return 0 to stop the timer
}
//-------------------------------- SOUND CONFIGURATIONS-----------------------------------

//-------------------------------- CHARACTER RELATED -----------------------------------
int score = 0; //score
int health = 5; // health
float characterX = 100.0f; // his current x coordinate (center of his head)
float characterY = 250.0f; // his current y coordinate (center of his head)
float fixedcharacterX = 100.0f; // his fixed original x coordinate (center of his head)
float fixedcharacterY = 250.0f; // his current y coordinate (center of his head) -> to bring him back to original position when he ducks
bool isJumping = false; // tracks whether he's jumping or not according to space key press
float maxjumpHeight = 400.0f; // the maximum height to which he can jump to le fo2
float gravity=3.3f; // kol mara benazel men el distance ad eh w howa beyenzel men el jump
float jumpSpeed=50.0f; // 3aks el gravity bas el fo2
bool isDucking = false; // tracks whether he's ducking or not according to lower arrow key press
float duckingDistance=0; // distance he ducks down
bool isFlying=false;
int scoreAdditionFactor=1;
//-------------------------------- CHARACTER RELATED -----------------------------------

//-------------------------------- OBSTACLES RELATED -----------------------------------

struct Obstacle {
    float x;
    float y;
    float height;
};
float speedMultiplier=0; // factor by which ill increase the distance moved by obstacles , power ups and collectibles to make everything faster as time progresses
std::vector<Obstacle> obstacles;
float obstacleTimer = 0.0f;
float obstacleTimerMaxValue=2.0f; // how often are obstacles generates (increases as time passes to make it more difficult)
float obstacleSpeed = 5.0f; // obstaclex-this (how much it moves every update)
//-------------------------------- OBSTACLES RELATED -----------------------------------
//-------------------------------- POWER-UPS RELATED -----------------------------------
int timerPowerUpDisplay=0;
char* powerUpMessage = nullptr; // message for countdown coin power up (double score)
char* powerUpMessage2 = nullptr; // message for countdown airplane power up (flying)
float coinAngle = 0.0f; // since coin rotates in place
struct powerUpCoin {
    float x;
    float y;
   
};
std::vector<powerUpCoin> coins; // data structure to hold of the all tha coins

struct powerUpFly {
    float x;
    float y;
   
};
float powerUpTimer=0.0;
std::vector<powerUpFly> flyingPowerUp; // data structure to hold of the all tha coins
float timeElapsedPlane = 0.0f; // related to planes animation
const float oscillationAmplitude = 5.0f; // for the shaking of the airplane in place
const float oscillationFrequency = 2.0f;
std::chrono::time_point<std::chrono::steady_clock> powerUpStartTime; // to keep them activated for a certain duration
bool powerUpActive = false;
std::chrono::time_point<std::chrono::steady_clock> powerUpStartTime2;
bool powerUpActive2 = false;
//-------------------------------- POWER-UPS RELATED -----------------------------------

//-------------------------------- COLLECTIBLES RELATED -----------------------------------

float collectibleRotationAngle = 0.0f;
struct Star {
    float x;
    float y;
    float size;
    float brightness;
    float speed;
};
std::vector<Star> stars; // data structure to hold of the all tha stars

struct Collectible {
    float x;
    float y;
    float height;
};
std::vector<Collectible> collecibles;
//----------------------------- COLLECTIBLES RELATED-----------------------------------

//-------------------------------- CODE LOGIC -----------------------------------

//-------------------------------- DRAWING HELPERS -----------------------------------

void drawCircle(float x, float y, float radius, int segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
}
void drawRocket(float x, float y) {
   // the body of the rocket
    glColor3f(0.1f, 0.1f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(x - 5, y);
        glVertex2f(x + 5, y);
        glVertex2f(x + 5, y + 30);
        glVertex2f(x - 5, y + 30);
    glEnd();

  // the nose of the rocket
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 5, y + 30);
        glVertex2f(x + 5, y + 30);
        glVertex2f(x, y + 45);
    glEnd();

  // the fins bto3 el rocket
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 5, y + 15);
        glVertex2f(x - 10, y);
        glVertex2f(x - 5, y);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2f(x + 5, y + 15);
        glVertex2f(x + 10, y);
        glVertex2f(x + 5, y);
    glEnd();
}

// to draw the sad face collectible
void drawSadFace(float x, float y, float size) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
glRotatef(collectibleRotationAngle, 0.0f, 0.0f, 1.0f);
        
        glTranslatef(-x, -y, 0.0f);
    // Draw Star (Polygon)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 1.0f, 1.0f);
    
    glVertex2f(x-size/2, y);
    glVertex2f(x+size/2, y);
    glVertex2f(x+size/2, y+size);
    glVertex2f(x-size/2, y+size);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glColor3d(1.0, 0.0, 0.0);
    glVertex2f(x - 5, y + 25);
    glVertex2f(x + 5, y + 25);
    glEnd();

    glColor3d(1.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 180; i++) {
        float angle = i * M_PI / 180.0f;
        float vertexX = x + (size * 0.4f) * cos(angle);
        float vertexY =   (y - (size * 0.2f) + (size * 0.2f) * sin(angle));
        glVertex2f(vertexX, vertexY+15);
    }
    
    glEnd();
    glPopMatrix();

   
}
// to draw the stars that animate up in the backgroubd
void drawDiamond(float x, float y, float size, float brightness) {
   
    brightness = std::max(0.5f, std::min(brightness, 1.0f));

    glColor3f(brightness, brightness, brightness);
    glBegin(GL_QUADS);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y);
        glVertex2f(x, y - size);
        glVertex2f(x - size, y);
    glEnd();
}

void drawEllipse(float x, float y, float radiusX, float radiusY, int segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex2f(x + cos(angle) * radiusX, y + sin(angle) * radiusY);
    }
    glEnd();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void drawCharacter() {

    glPushMatrix();
    if (isFlying) {
          
        glTranslatef(0,characterY+60+125, 0.0f);
        glScalef(1.0f, -1.0f, 1.0f);
        glTranslatef(0, -(characterY+60), 0.0f);

        }
   // body
    glColor3f(0.2f, 0.2f, 0.6f);
    drawCircle(characterX, characterY - duckingDistance, 30.0f, 50);

    glColor3f(0.0f, 1.0f, 0.6f);
    drawTriangle(characterX - 15, characterY + 25 - duckingDistance,
                 characterX + 15, characterY + 25 - duckingDistance,
                 characterX, characterY + 60 - duckingDistance);

   //eyes
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(characterX - 10, characterY + 10 - duckingDistance, 10.0f, 30);
    drawCircle(characterX + 15, characterY + 10 - duckingDistance, 10.0f, 30);

    //pupils
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(characterX - 8.0, characterY + 12 - duckingDistance, 4.0f, 20);
    drawCircle(characterX + 17, characterY + 12 - duckingDistance, 4.0f, 20);

//legs
    glColor3f(0.4f, 0.3f, 0.7f);
    glBegin(GL_LINES);
    glVertex2f(characterX - 20, characterY - 10 - duckingDistance - 16); glVertex2f(characterX - 50, characterY - 150);
    glVertex2f(characterX - 10, characterY - 10 - duckingDistance - 16); glVertex2f(characterX - 20, characterY - 150);
    glVertex2f(characterX, characterY - 10 - duckingDistance - 16); glVertex2f(characterX + 10, characterY - 150);
    glVertex2f(characterX + 10, characterY - 10 - duckingDistance - 16); glVertex2f(characterX + 40, characterY - 150);
    glEnd();

   //points
    
    glColor3f(0.5f, 0.8f, 1.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glVertex2f(characterX - 50, characterY - 148);
    glVertex2f(characterX - 20, characterY - 148);
    glVertex2f(characterX + 10, characterY - 148);
    glVertex2f(characterX + 40, characterY - 148);
    glEnd();
    glPopMatrix();
}


void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    if (text != nullptr) {
        for (const char* c = text; *c != '\0'; ++c) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }
}


void drawHealth() {
   
        float spacing = 35.0f;
        float startX = 25.0f;

        for (int i = 0; i < health; i++) {
            drawRocket(startX + i * spacing, windowHeight-50);
        }
}
void drawCircleT(float x, float y, float radius, int numSegments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);

    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * M_PI * i / numSegments;
        float dx = radius * cos(angle);
        float dy = radius * sin(angle);
        glVertex2f(x + dx, y + dy);
    }

    glEnd();
}
void drawObstacle(float x, float y, float height) {
    glColor3f(0.5f, 0.5f, 0.5f);
    drawEllipse(x, y, 25.0f, 15.0f, 50);

    glColor3f(0.1f, 0.4f, 0.8f);
    drawCircleT(x, y + 10.0f, 10.0f, 30);

    glColor3f(0.0f, 1.0f, 0.0f);
    drawCircleT(x - 15.0f, y - 2.5f, 1.25f, 20);
    drawCircleT(x, y - 2.5f, 1.25f, 20);
    drawCircleT(x + 15.0f, y - 2.5f, 1.25f, 20);
    glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
    drawCircleT(x - 15.0f, y - 2.5f, 2.0f, 20);
    drawCircleT(x, y - 2.5f, 2.0f, 20);
    drawCircleT(x + 15.0f, y - 2.5f, 2.0f, 20);
}

void drawPlane(float x, float y) {
    timeElapsedPlane += 0.1f;

   
    float oscillation = oscillationAmplitude * sin(oscillationFrequency * timeElapsedPlane);
    float animatedY = y + oscillation;

    glPushMatrix();

   
    glScalef(0.2f, 0.2f, 1.0f);

    glColor3f(0.0f, 0.0f, 0.5f);

  
    glBegin(GL_QUADS);
    glVertex2f(x, animatedY);
    glVertex2f(x + 240.0f, animatedY);
    glVertex2f(x + 240.0f, animatedY + 50.0f);
    glVertex2f(x, animatedY + 50.0f);
    glEnd();

    
    glColor3f(0.5f, 0.7f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 240.0f, animatedY);
    glVertex2f(x + 260.0f, animatedY + 25.0f);
    glVertex2f(x + 240.0f, animatedY + 50.0f);
    glEnd();

    glColor3f(0.5f, 0.7f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, animatedY + 50.0f);
    glVertex2f(x - 40.0f, animatedY + 90.0f);
    glVertex2f(x, animatedY);
    glEnd();

    glColor3f(0.5f, 0.7f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 80.0f, animatedY);
    glVertex2f(x + 20.0f, animatedY - 90.0f);
    glVertex2f(x + 160.0f, animatedY);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(x + 80.0f, animatedY + 50.0f);
    glVertex2f(x + 20.0f, animatedY + 140.0f);
    glVertex2f(x + 160.0f, animatedY + 50.0f);
    glEnd();

    glColor3f(0.5f, 0.7f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, animatedY + 50.0f);
    glVertex2f(x - 40.0f, animatedY - 40.0f);
    glVertex2f(x, animatedY);
    glEnd();

    for (float i = x + 40.0f; i <= x + 200.0f; i += 60.0f) {
        glColor3f(0.5f, 0.7f, 1.0f);

        glBegin(GL_POLYGON);
        for (int j = 0; j < 100; j++) {
            float theta = (float)j * 2.0f * 3.14159f / 100.0f;
            glVertex2f(i + 10.0f * cos(theta), animatedY + 25.0f + 10.0f * sin(theta));
        }
        glEnd();
        glPointSize(2.0f);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POINTS);
        glVertex2f(i, animatedY + 25.0f);
        glEnd();
    }

    glPopMatrix();
}
void drawFlyingPowerUp(float x, float y) {

   
        glColor3f(1.0f, 0.75f, 0.8f);
        
    drawPlane(x,y);
    
}
void drawGround() {
    glColor3f(0.1f, 0.1f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 100.0f);
    glVertex2f(windowWidth, 100.0f);
    glVertex2f(windowWidth, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glEnd();

    glColor3f(0.3f, 0.5f, 0.7f);
    glBegin(GL_LINES);
    for (float x = 0.0f; x < windowWidth; x += 40.0f) {
        glVertex2f(x, 0.0f);
        glVertex2f(x, 100.0f);
    }
    glEnd();

    glBegin(GL_LINES);
    for (float y = 0.0f; y <= 100.0f; y += 20.0f) {
        glVertex2f(0.0f, y);
        glVertex2f(windowWidth, y);
    }
    glEnd();

    glColor3f(0.5f, 0.7f, 0.9f);
    glBegin(GL_TRIANGLES);
    for (float x = 20.0f; x < windowWidth; x += 80.0f) {
        glVertex2f(x, 50.0f);
        glVertex2f(x + 30.0f, 80.0f);
        glVertex2f(x + 60.0f, 50.0f);
    }
    glEnd();
    glColor3f(0.8f, 1.0f, 0.8f);
    glBegin(GL_POINTS);
    for (float x = 10.0f; x < windowWidth; x += 30.0f) {
        for (float y = 10.0f; y < 100.0f; y += 30.0f) {
            glVertex2f(x, y);
        }
    }
    glEnd();
}

void drawStar(float centerX, float centerY, float outerRadius, float innerRadius, int points) {
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < points * 2; i++) {
        float angle = i * M_PI / points;
        float r = (i % 2 == 0) ? outerRadius : innerRadius;

        glVertex2f(centerX + cos(angle) * r, centerY + sin(angle) * r);
    }
    glEnd();
}
void drawUpperFrame() {
    float rectWidth = 700.0f / 3.0f;

    glColor3f(0.2f, 0.6f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, windowHeight - 50.0f);
    glVertex2f(rectWidth, windowHeight - 50.0f);
    glVertex2f(rectWidth, windowHeight);
    glVertex2f(0.0f, windowHeight);
    glEnd();
    
    glColor3f(0.0f, 0.4f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(rectWidth, windowHeight - 50.0f);
    glVertex2f(rectWidth * 2.0f, windowHeight - 50.0f);
    glVertex2f(rectWidth * 2.0f, windowHeight);
    glVertex2f(rectWidth, windowHeight);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(rectWidth * 2.0f, windowHeight - 50.0f);
    glVertex2f(rectWidth * 3.0f, windowHeight - 50.0f);
    glVertex2f(rectWidth * 3.0f, windowHeight);
    glVertex2f(rectWidth * 2.0f, windowHeight);
    glEnd();


    drawStar(350, 675, 25, 10, 5);
}
void drawCoin(float x, float y) {
    glPushMatrix();

    glTranslatef(x, y, 0);
    
    float scale = 1.0f + 0.1f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.005f); // between 1 and 1.1
    glScalef(scale, scale, 1.0f);

    glColor3f(1.0f, 0.84f, 0.0f);
    drawCircle(0.0f, 0.0f, 25.0f, 25);
    glColor3f(0.8f, 0.7f, 0.0f);
    drawCircleT(0.0f, 0.0f, 15.0f, 15);

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-15.0f, 10.0f);
    glVertex2f(-5.0f, 10.0f);
    glVertex2f(-15.0f, 5.0f);
    glEnd();

    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);
    drawText("x 2", x - 10, y - 10);

    float squareSize = 10.0f;
    float offsetX = squareSize / 2;
    float offsetY = squareSize / 2;

    float outerSquareSize = 20.0f;
    float outerOffsetX = outerSquareSize / 2;
    float outerOffsetY = outerSquareSize / 2;

    float outerPoints[4][2] = {
        {x - outerOffsetX - 2, y + outerOffsetY},
        {x + outerOffsetX - 2, y + outerOffsetY},
        {x + outerOffsetX - 2, y - outerOffsetY - 2},
        {x - outerOffsetX - 2, y - outerOffsetY - 2}
    };

    glColor3f(0.8f, 0.52f, 0.25f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 4; i++) {
        glVertex2f(outerPoints[i][0], outerPoints[i][1]);
    }
    glEnd();
}


//-------------------------------- DRAWING HELPERS -----------------------------------

//-------------------------------- COLLISIONS -----------------------------------


bool checkCollision(float obstacleX,float obstacleY, float obstacleHeight) {
    float characterLeft=characterX-50;
    float characterRight=characterX+40;
    float characterBottom;
    float CharacterTop;
    if (!isFlying)
    {
        characterBottom=characterY-148;
        CharacterTop= characterY+60-duckingDistance;
       
    }
    
    else{
        characterBottom=650-210;
         CharacterTop= 650;

    }
    float obstacleRight= obstacleX+25;
    float obstacleTop=obstacleY+20;
    float obstacleBottom=obstacleY-15;
    float obstacleLeft=obstacleX-25;
    bool isColliding = !(characterRight < obstacleLeft ||
                         characterLeft > obstacleRight ||
                         CharacterTop < obstacleBottom ||
                         characterBottom > obstacleTop);
    return isColliding;

}
bool checkCollisionCollectible(float collectibleX,float collectibleY, float collectibleSize) {
    float characterLeft=characterX-50;
    float characterRight=characterX+40;
    float characterBottom;
    float CharacterTop;
    if (!isFlying)
    {
        characterBottom=characterY-148;
        CharacterTop= characterY+60-duckingDistance;
       
    }
    
    else{
        characterBottom=650-210;
         CharacterTop= 650;

    }
 
    float collectibleRight=collectibleX+15;
    float collectibleTop=collectibleY+15;
    float collectibleBottom=collectibleY-15;
    float collectibleLeft=collectibleX-15;
    bool isColliding = !(characterRight < collectibleLeft ||
                         characterLeft > collectibleRight ||
                         CharacterTop < collectibleBottom ||
                         characterBottom > collectibleTop);
    return isColliding;

}
bool checkCollisionFlying(float flyingX,float flyingY) {
    float characterLeft=characterX-50;
    float characterRight=characterX+40;
    float characterBottom;
    float CharacterTop;
    if (!isFlying)
    {
        characterBottom=characterY-148;
        CharacterTop= characterY+60-duckingDistance;
       
    }
    
    else{
        characterBottom=650-210;
         CharacterTop= 650;

    }
    float flyingRight=flyingX+15;
    float flyingTop=flyingY+15;
    float flyingBottom=flyingY-15;
    float flyingLeft=flyingX-15;
    bool isColliding = !(characterRight < flyingLeft ||
                         characterLeft > flyingRight ||
                         CharacterTop < flyingBottom ||
                         characterBottom > flyingTop);
    return isColliding;

}
bool checkCollisionCoin(float coinX,float coinY) {
    float characterLeft=characterX-50;
    float characterRight=characterX+40;
    float characterBottom;
    float CharacterTop;
    if (!isFlying)
    {
        characterBottom=characterY-148;
        CharacterTop= characterY+60-duckingDistance;
       
    }
    
    else{
        characterBottom=650-210;
         CharacterTop= 650;

    }
    float coinRight=coinX+12.5;
    float coinTop=coinY+12.5;
    float coinBottom=coinY-12.5;
    float coinLeft=coinX-12.5;
    bool isColliding = !(characterRight < coinLeft ||
                         characterLeft > coinRight ||
                         CharacterTop < coinBottom ||
                         characterBottom > coinTop);
    return isColliding;

}
//-------------------------------- COLLISIONS -----------------------------------
//----------------------------- BACKGROUND ----------------------------
void updateStars() {
    for (auto &star : stars) {
        star.brightness += star.speed;

        if (star.brightness > 1.0f || star.brightness < 0.0f) {
            star.speed = -star.speed;
            star.brightness = std::clamp(star.brightness, 0.0f, 1.0f);
        }
    }
}
void initStars(int numStars) {
    srand(static_cast<unsigned>(time(0)));
    for (int i = 0; i < numStars; i++) {
        Star star;
        star.x = rand() % windowWidth;
       
        star.y = rand() % 201 + 450;
        star.size = rand() % 3 + 2;
        star.brightness = static_cast<float>(rand() % 50 + 50) / 100.0f;

        star.speed = static_cast<float>(rand() % 20 + 10) / 1000.0f;
        stars.push_back(star);
    }
}
//----------------------------- BACKGROUND ----------------------------
//----------------------------- COLLECTIBLES ----------------------------

void generateCollectibleForFlying() {
    
    
   
        Collectible collectible;
        collectible.x = windowWidth;
        collectible.height = 20;
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        
    collectible.y = 500 + std::rand() % (610 - 500 + 1);
        collecibles.push_back(collectible);
   
}

//----------------------------- COLLECTIBLES ----------------------------

//--------------------------OBSTACLE GENERATION AND UPDATE ----------------------------
void generateObstacle() {
    Obstacle obstacle;
    obstacle.x = windowWidth;
    obstacle.height = 50;
    
    if (rand() % 2 == 0) {
        obstacle.y = 120;
    } else {
        obstacle.y = 270;
    }
    obstacles.push_back(obstacle);
    timerPowerUpDisplay++;
    if (timerPowerUpDisplay==2 || timerPowerUpDisplay==22 || timerPowerUpDisplay==17)
    {
        powerUpCoin p;
               p.x = windowWidth;
       
               p.y = (obstacle.y == 120) ? 400 : 190;
               coins.push_back(p);
        
    }
    else if (timerPowerUpDisplay== 8 || timerPowerUpDisplay== 31)
    {
        powerUpFly pf;
               pf.x = windowWidth;
       
               pf.y = (obstacle.y == 120) ? 400 : 190;
                 flyingPowerUp.push_back(pf);
    }
  
    else
    {
        int randomItem = rand() % 2;
        
        if (randomItem == 0) {
            Collectible collectible;
            collectible.x = windowWidth;
            collectible.height = 20;
            collectible.y = (obstacle.y == 120) ? 350 : 140;
            collecibles.push_back(collectible);
         
        }
        else
        {
            Collectible collectible2;
            collectible2.x = windowWidth+175;
            collectible2.height = 20;
            collectible2.y = 140 + rand() % (450 - 140 + 1);
            collecibles.push_back(collectible2);
        }
       
        
    }


}

void updateObstacles(int value) {
   
if (!gameEnded)
{
    if (powerUpActive) {
        auto currentTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - powerUpStartTime).count();
        int remainingTime = 12 - duration;
        
        if (remainingTime > 0) {
            const char* baseMessage = "Double Score Active:  ";
            
            int messageLength = strlen(baseMessage) + 20;
            powerUpMessage = new char[messageLength];
            
            sprintf(powerUpMessage, "%s%d seconds remaining", baseMessage, remainingTime);
        } else {
            delete[] powerUpMessage;
            powerUpMessage = nullptr;
        }
        
        if (duration >= 12) {
            scoreAdditionFactor = 1;
            powerUpActive = false;
            time_t now = time(0);
            tm *ltm = localtime(&now);
            
            
            printf("I JUST ACTIVATED COIN POWERUP at %02d:%02d:%02d\n",
                   ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
        }
    }
    if (powerUpActive2) {
        
        auto currentTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - powerUpStartTime2).count();
        int remainingTime = 10 - duration;
        
        if (remainingTime > 0) {
            const char* baseMessage = "Flying Active:  ";
            
            int messageLength = strlen(baseMessage) + 20;
            powerUpMessage2 = new char[messageLength];
            sprintf(powerUpMessage2, "%s%d seconds remaining", baseMessage, remainingTime);
        } else {
            delete[] powerUpMessage2;
            powerUpMessage2 = nullptr;
        }
        if (duration >= 10) {
            isFlying=false;
            characterY = fixedcharacterY;
            powerUpActive2 = false;
            time_t now = time(0);
            tm *ltm = localtime(&now);
            
            printf("I JUST RESET FLYING POWERUP at %02d:%02d:%02d\n",
                   ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
        }
        
    }
    if (recoilActive) {
          auto currentTime = std::chrono::steady_clock::now();
          // Calculate the duration in milliseconds
          auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - recoilStartTime).count();
          
          // Print the duration that has passed
          printf("Duration since recoil started: %lld ms\n", duration);

          if (duration >= 1200) { // Check for 50 milliseconds recoil duration
              recoilActive = false;
              time_t now = time(0);
              tm *ltm = localtime(&now);
              
              printf("I JUST RESET RECOIL at %02d:%02d:%02d\n",
                     ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
          }
     
      }
    updateStars();
    obstacleTimer += 0.02f;
    powerUpTimer+=0.02;
    gravity+=0.0002;
   
    speedMultiplier+=0.001;
    
    if (powerUpTimer>=1.0f)
    {
        if (powerUpActive2)
        {
            generateCollectibleForFlying();
        }
        powerUpTimer=0.0f;
    }
    auto currentTime = std::chrono::steady_clock::now();
           // Calculate the duration in milliseconds
           auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - recoilStartTime).count();
//           
//           // Print the duration that has passed
//           printf("Duration since recoil started: %lld ms\n", duration);

          
    if (obstacleTimer >= obstacleTimerMaxValue && !(isFlying) && duration>=5000) {
        generateObstacle();
        obstacleTimer = 0.0f;
        
    }
    obstacleTimerMaxValue-=0.0001;
    if (obstacleTimerMaxValue<=0.5)
    {
        obstacleTimerMaxValue=0.5;
    }
    
    collectibleRotationAngle += 0.5f;
    coinAngle += 1.0f;
    if (collectibleRotationAngle >= 360.0f) {
        collectibleRotationAngle -= 360.0f;
    }
    for (int i = 0; i < obstacles.size(); ++i) {
        obstacles[i].x += (recoilActive ? 5 : -(obstacleSpeed + speedMultiplier));

        
        if (obstacles[i].x < -30.0f) {
            obstacles.erase(obstacles.begin() + i);
            --i;
            continue;
        }
        
        if (!recoilActive && checkCollision(obstacles[i].x,obstacles[i].y, obstacles[i].height)) {
            health--;
            recoilStartTime = std::chrono::steady_clock::now();
                      recoilActive = true;
            playSoundForDuration("collision.wav", 5000, 128);
           
            
            printf("HEALTH NOW %d\n",health);
//            obstacles.erase(obstacles.begin() + i);
//            --i;
            if (health <= 0) {
                gameEnded = true;
                printf("I SET GAME ENDED TO TRUE: %s\n", gameEnded ? "true" : "false");
                glutPostRedisplay();
            }
              
               
           
        }
       
    }
    
    
    
    for (int i = 0; i < collecibles.size(); ++i) {
        collecibles[i].x += (recoilActive ? 5 : -(obstacleSpeed + speedMultiplier));

        
       
        if (collecibles[i].x < -30.0f) {
            collecibles.erase(collecibles.begin() + i);
            --i;
            continue;
        }
        if (checkCollisionCollectible(collecibles[i].x,collecibles[i].y, collecibles[i].height)) {
            score+=scoreAdditionFactor;
            playSoundForDuration("pickUp.mp3", 5000, 128);
            collecibles.erase(collecibles.begin() + i);
            --i;
        }
    }
    for (int i = 0; i < coins.size(); ++i) {
        coins[i].x+= (recoilActive ? 5 : -(obstacleSpeed + speedMultiplier));

        
        if (coins[i].x < -30.0f) {
            coins.erase(coins.begin() + i);
            --i;
            continue;
        }
        if (checkCollisionCoin(coins[i].x,coins[i].y)) {
            playSoundForDuration("coin.wav", 5000, 128);

            scoreAdditionFactor=2;
            powerUpStartTime = std::chrono::steady_clock::now();
            powerUpActive = true;
            time_t now = time(0);
            tm *ltm = localtime(&now);
            
            printf("I JUST ACTIVATED COIN POWERUP at %02d:%02d:%02d\n",
                   ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
            coins.erase(coins.begin() + i);
            --i;
        }
    }
    for (int i = 0; i < flyingPowerUp.size(); ++i) {
        flyingPowerUp[i].x += (recoilActive ? 5 : -(obstacleSpeed + speedMultiplier));

        
        if (flyingPowerUp[i].x < -30.0f) {
            flyingPowerUp.erase(flyingPowerUp.begin() + i);
            --i;
            continue;
        }
        if (checkCollisionFlying(flyingPowerUp[i].x, flyingPowerUp[i].y)) {
            playSoundForDuration("powerupplane.wav", 5000, 128);

            isFlying=true;
            powerUpStartTime2 = std::chrono::steady_clock::now();
            powerUpActive2 = true;
            time_t now = time(0);
            tm *ltm = localtime(&now);
            
            printf("I JUST ACTIVATED FLYING POWERUP at %02d:%02d:%02d\n",
                   ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
            flyingPowerUp.erase(flyingPowerUp.begin() + i);
            --i;
        }
    }
    
}
    glutTimerFunc(16, updateObstacles, 0);

}

//----------------------- OBSTACLE GENERATION AND UPDATE ----------------------------


//----------------------------- KEYBOARD ACTIONS ----------------------------

void keyPress(unsigned char key, int x, int y) {
    if (key == ' ') {
        if (!isJumping && characterY ==fixedcharacterY) {
            isJumping = true;
        }
    }
    if (key == GLUT_KEY_DOWN) {
           isDucking = true;
       }
}

// Key release handler
void keyUp(unsigned char key, int x, int y) {
    if (key == ' ') {
        isJumping = false;
    }
    if (key == GLUT_KEY_DOWN) {
           isDucking = false;
       }
}
void specialKeyPress(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_DOWN:
            isDucking = true;
            duckingDistance=80;
            glutPostRedisplay();
            break;
       
    }
}
void specialKeyUp(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_DOWN:
            isDucking = false;
            duckingDistance=0;
            glutPostRedisplay();
            break;
       
    }
}

//----------------------------- KEYBOARD ACTIONS ----------------------------


//----------------------------- CHARACTER ----------------------------

void updateCharacter(int value) {
    if (!gameEnded)
    {
        if (isJumping) {
            if (characterY < maxjumpHeight) {
                characterY += jumpSpeed;
            } else {
                isJumping = false;
            }
        } else if (characterY > fixedcharacterY && !isFlying) {
            characterY -= gravity;
        } else {
            characterY = fixedcharacterY;
            
            
        }

        
        glutPostRedisplay(); // Request a redraw
    }
        glutTimerFunc(16, updateCharacter, 0);
    
}
//----------------------------- CHARACTER ----------------------------

//--------------------------------DISPLAY -----------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (!gameEnded) {
           playBackgroundMusic();
       }
    if (gameEnded) {
        printf("GAME ENDED HEREE");
        glClear(GL_COLOR_BUFFER_BIT);
        if (Mix_PlayingMusic()) {
            Mix_HaltMusic();
        }
        if (health == 0) {
           
            char endMessage[50];
            sprintf(endMessage, "YOUU LOST:(( , Score : %d", score);
            glColor3f(1.0f, 1.0f, 1.0f);
            drawText(endMessage, windowWidth / 2 - 100, windowHeight / 2);
            playSoundForDuration("gameWin.wav", 5000, 128);
        } else {
            char endMessage[50];
            sprintf(endMessage, "Game End! Score: %d", score);
            glColor3f(1.0f, 1.0f, 1.0f);
            drawText(endMessage, windowWidth / 2 - 100, windowHeight / 2);
            playSoundForDuration("gameover.wav", 5000, 128);
        }
      
    }
    else {
       
           glClearColor(0.0, 0.0, 0.0, 1.0f);
    

        drawUpperFrame();
        drawHealth();
        drawCharacter();

        for (const auto &star : stars) {
                drawDiamond(star.x, star.y, star.size, star.brightness);
            }
        for (const auto& obstacle : obstacles) {
            drawObstacle(obstacle.x, obstacle.y, obstacle.height);
        }
        for (const auto& collectible : collecibles) {
            drawSadFace(collectible.x, collectible.y, 30);
        }
        for (const auto& coin : coins) {
            drawCoin(coin.x,coin.y);
        }
        for (const auto& flying : flyingPowerUp) {
            drawFlyingPowerUp(flying.x*5,flying.y*5);
        }
        drawGround();

        static float lastTime = 0.0f;
        static bool timerInitialized = false;
      
        if (!timerInitialized) {
            lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
            timerInitialized = true;
        }

        float totalTime = 120.0f;
        float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
        elapsedTime += currentTime - lastTime;
        lastTime = currentTime;

     
        float remainingTime = totalTime - elapsedTime;

        if (remainingTime < 0) {
            remainingTime = 0;
        }

        char scoreText[50];
        char timeString[50];
        sprintf(timeString, "Remaining Time: %.2f sec", remainingTime);
        sprintf(scoreText, "Score: %d", score);

        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(scoreText, windowWidth - 230.0f, windowHeight - 20.0f);
        drawText(timeString, windowWidth - 230.0f, windowHeight - 40.0f);
        drawText(powerUpMessage, windowWidth - 400.0f, windowHeight - 70.0f);
        drawText(powerUpMessage2, windowWidth - 400.0f, windowHeight - 90.0f);

      // 2 minutes is the game time
        if (elapsedTime >= 120.0f) {
            gameEnded = true;
            
        }
    }

    glFlush();
}

//--------------------------------DISPLAY -----------------------------------


//-------------------------------- MAIN METHOD -----------------------------------

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("UFO ALIEN GAME");
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
    initStars(80);
    initAudio();
    glutDisplayFunc(display);
    glutTimerFunc(0, updateCharacter, 0);
    glutTimerFunc(16, updateObstacles, 0);
    glutKeyboardFunc(keyPress);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKeyPress);
    glutSpecialUpFunc(specialKeyUp);
    glutMainLoop();
    cleanupAudio();
    return 0;
}







