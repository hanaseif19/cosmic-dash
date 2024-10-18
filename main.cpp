//
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <vector>

//-------------------------------- GENERAL CONFIGURATIONS-----------------------------------
int windowWidth = 700;
int windowHeight = 700;
float elapsedTime = 0.0f; // to keep track of time to display it throughout the game
bool gameEnded = false; // Variable to check if the game has ended
float timeforBackgroundColor=0.0f;

//-------------------------------- GENERAL CONFIGURATIONS-----------------------------------

//-------------------------------- CHARACTER RELATED -----------------------------------
int score = 0; //score
int health = 5; // health
float characterX = 100.0f; // his current x coordinate (center of his head)
float characterY = 250.0f; // his current y coordinate (center of his head)
float fixedcharacterX = 100.0f; // his fixed original x coordinate (center of his head)
float fixedcharacterY = 250.0f; // his current y coordinate (center of his head) -> to bring him back to original position when he ducks
bool isJumping = false; // tracks whether he's jumping or not according to space key press
float maxjumpHeight = 400.0f; // the maximum height to which he can jump to le fo2
float gravity=3.0f; // kol mara benazel men el distance ad eh w howa beyenzel men el jump
float jumpSpeed=50.0f; // 3aks el gravity bas el fo2
bool isDucking = false; // tracks whether he's ducking or not according to lower arrow key press
float duckingDistance=0; // distnce he ducks down
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
float obstacleSpeed = 4.0f;
//-------------------------------- OBSTACLES RELATED -----------------------------------
//-------------------------------- POWER-UPS RELATED -----------------------------------
//float powerUp = 0.0f;
struct powerUpCoin {
    float x;
    float y;
   
};
std::vector<powerUpCoin> coins; // data structure to hold of the all tha coins

struct powerUpFly {
    float x;
    float y;
   
};
std::vector<powerUpFly> flyingPowerUp; // data structure to hold of the all tha coins

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
//-------------------------------- COLLECTIBLES RELATED -----------------------------------

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
    // Rocket body
    glColor3f(1.0f, 0.0f, 0.0f);  // Red color for the body
    glBegin(GL_QUADS);  // Draw the main body
        glVertex2f(x - 5, y);        // Bottom-left
        glVertex2f(x + 5, y);        // Bottom-right
        glVertex2f(x + 5, y + 30);   // Top-right
        glVertex2f(x - 5, y + 30);   // Top-left
    glEnd();

    // Rocket nose cone
    glColor3f(0.0f, 0.0f, 1.0f);  // Blue color for the nose
    glBegin(GL_TRIANGLES);  // Draw the nose
        glVertex2f(x - 5, y + 30); // Bottom-left of triangle
        glVertex2f(x + 5, y + 30); // Bottom-right of triangle
        glVertex2f(x, y + 45);      // Top of triangle (nose)
    glEnd();

    // Rocket fins
    glColor3f(0.0f, 1.0f, 0.0f);  // Green color for the fins
    glBegin(GL_TRIANGLES);  // Left fin
        glVertex2f(x - 5, y + 15); // Bottom-left of fin
        glVertex2f(x - 10, y);      // Bottom-left corner
        glVertex2f(x - 5, y);        // Top-left corner
    glEnd();

    glBegin(GL_TRIANGLES);  // Right fin
        glVertex2f(x + 5, y + 15); // Bottom-right of fin
        glVertex2f(x + 10, y);      // Bottom-right corner
        glVertex2f(x + 5, y);        // Top-right corner
    glEnd();
}
// Function to draw the star collectible
void drawStarCollectible(float x, float y, float size) {
    glPushMatrix();  // Save the current matrix state
    glTranslatef(x, y, 0.0f);
glRotatef(collectibleRotationAngle, 0.0f, 0.0f, 1.0f);
        
        // Move back to the origin to draw the star
        glTranslatef(-x, -y, 0.0f);
    // Draw Star (Polygon)
    glBegin(GL_POLYGON); // Start drawing the star shape
    glColor3f(0.0f, 1.0f, 1.0f); // Star color: cyan
    
        glVertex2f(x-size/2, y); // Define the vertex
    glVertex2f(x+size/2, y); // Define the vertex
    glVertex2f(x+size/2, y+size);
    glVertex2f(x-size/2, y+size);
    glEnd(); // End the star drawing

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
        float angle = i * M_PI / 180.0f; // Angle for smile arc
        float vertexX = x + (size * 0.4f) * cos(angle); // X coordinate for smile
        float vertexY =   (y - (size * 0.2f) + (size * 0.2f) * sin(angle)); // Y coordinate for smile
        glVertex2f(vertexX, vertexY+15);
    }
    
    glEnd();
    glPopMatrix();  // Restore the previous matrix state

   
}
void drawDiamond(float x, float y, float size, float brightness) {
    glColor3f(brightness, brightness, brightness); // Set color based on brightness
    glBegin(GL_QUADS);
        glVertex2f(x, y + size);     // Top vertex
        glVertex2f(x + size, y);     // Right vertex
        glVertex2f(x, y - size);     // Bottom vertex
        glVertex2f(x - size, y);     // Left vertex
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
    glPushMatrix(); // Save the current transformation state

    // Check if the character is flying and adjust its position
    if (isFlying) {
        characterY = windowHeight - 200.0f; // Position the character at the ceiling
       //printf("HES FLYINGG");
        
        // Move the character to its position before scaling
        glTranslatef(characterX, characterY, 0.0f);
        
        // If flying, reflect the character vertically
        glScalef(1.0f, -1.0f, 1.0f); // Flip in the Y direction
    } else {
        // For normal walking position, just translate
        glTranslatef(characterX, characterY, 0.0f);
    }

    // Draw the character
    glColor3f(0.3f, 0.8f, 0.3f);
    drawCircle(0.0f, -duckingDistance, 30.0f, 50); // Centered at the origin

    glColor3f(1.0f, 0.4f, 0.5f);
    drawTriangle(-15.0f, 25.0f - duckingDistance, 15.0f, 25.0f - duckingDistance, 0.0f, 60.0f - duckingDistance);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(-10.0f, 10.0f - duckingDistance, 10.0f, 30);
    drawCircle(15.0f, 10.0f - duckingDistance, 10.0f, 30);
    
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(-8.0f, 12.0f - duckingDistance, 4.0f, 20);
    drawCircle(17.0f, 12.0f - duckingDistance, 4.0f, 20);
    
    glColor3f(0.3f, 0.8f, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(-20.0f, -10.0f - duckingDistance); glVertex2f(-50.0f, -150.0f);
    glVertex2f(-10.0f, -10.0f - duckingDistance); glVertex2f(-20.0f, -150.0f);
    glVertex2f(0.0f, -10.0f - duckingDistance); glVertex2f(10.0f, -150.0f);
    glVertex2f(10.0f, -10.0f - duckingDistance); glVertex2f(40.0f, -150.0f);
    glEnd();

    glColor3f(1.0f, 0.0f, 1.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glVertex2f(-50.0f, -148);
    glVertex2f(-20.0f, -148);
    glVertex2f(10.0f, -148);
    glVertex2f(40.0f, -148);
    glEnd();

    glPopMatrix(); // Restore the previous transformation state
}


void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawHealth() {
   
        float spacing = 35.0f; // Spacing between rockets
        float startX = 25.0f;  // Starting x position

        for (int i = 0; i < health; i++) {
            drawRocket(startX + i * spacing, windowHeight-50); // Draw rockets horizontally
        }
}
void drawObstacle(float x, float y, float height) {

   
        glColor3f(0.7f, 0.7f, 0.0f);
        drawEllipse(x, y, 25.0f, 15.0f, 50);

        glColor3f(0.0f, 0.7f, 0.0f);
        drawCircle(x, y + 10.0f, 10.0f, 30);

        glColor3f(1.0f, 0.0f, 0.0f);
        drawCircle(x - 15.0f, y - 2.5f, 1.25f, 20);
        drawCircle(x, y - 2.5f, 1.25f, 20);
        drawCircle(x + 15.0f, y - 2.5f, 1.25f, 20);
    
}
void drawFlyingPowerUp(float x, float y) {

   
        glColor3f(1.0f, 0.75f, 0.8f);
        
        drawCircle(x, y , 15.0f, 30);
    
}
void drawGround() {
    // Draw the filled rectangle (ground background)
    glColor3f(0.3f, 0.2f, 0.1f);  // Brown color
    glBegin(GL_QUADS);  // Draw a rectangle with 4 vertices
    glVertex2f(0.0f, 100.0f);
    glVertex2f(windowWidth, 100.0f);
    glVertex2f(windowWidth, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glEnd();

    // Draw lines inside the ground for texture
    glColor3f(0.5f, 0.3f, 0.2f);  // Darker brown
    glBegin(GL_LINES);
    for (float x = 0.0f; x < windowWidth; x += 40.0f) {
        glVertex2f(x, 0.0f);
        glVertex2f(x, 100.0f);  // Vertical lines
    }
    glEnd();

    glBegin(GL_LINES);
    for (float y = 0.0f; y <= 100.0f; y += 20.0f) {
        glVertex2f(0.0f, y);
        glVertex2f(windowWidth, y);  // Horizontal lines
    }
    glEnd();

    // Add some triangles for extra pattern in the middle
    glColor3f(0.6f, 0.4f, 0.2f);  // Slightly lighter color
    glBegin(GL_TRIANGLES);
    for (float x = 20.0f; x < windowWidth; x += 80.0f) {
        glVertex2f(x, 50.0f);
        glVertex2f(x + 30.0f, 80.0f);
        glVertex2f(x + 60.0f, 50.0f);
    }
    glEnd();

    // Draw points in between the lines and triangles for texture
    glColor3f(0.8f, 0.6f, 0.4f);  // Light brown
    glBegin(GL_POINTS);
    for (float x = 10.0f; x < windowWidth; x += 30.0f) {
        for (float y = 10.0f; y < 100.0f; y += 30.0f) {
            glVertex2f(x, y);
        }
    }
    glEnd();
}

void drawStar(float centerX, float centerY, float outerRadius, float innerRadius, int points) {
    glColor3f(1.0f, 1.0f, 1.0f); // Set star color to yellow

    glBegin(GL_LINE_LOOP); // Start drawing a line loop
    for (int i = 0; i < points * 2; i++) {
        float angle = i * M_PI / points; // Calculate angle
        float r = (i % 2 == 0) ? outerRadius : innerRadius;

        glVertex2f(centerX + cos(angle) * r, centerY + sin(angle) * r);
    }
    glEnd();
}
void drawUpperFrame() {
    // Set the width for each rectangle
    float rectWidth = 700.0f / 3.0f; // Width of each rectangle

    glColor3f(0.678f, 0.847f, 0.902f);  // A light sky blue

    // 1. Top-left rectangle
    glBegin(GL_QUADS);
    glVertex2f(0.0f, windowHeight - 50.0f);
    glVertex2f(rectWidth, windowHeight - 50.0f);  // Width of the left rectangle
    glVertex2f(rectWidth, windowHeight);
    glVertex2f(0.0f, windowHeight);
    glEnd();
    
    glColor3f(0.0f, 0.478f, 0.651f);  // A medium teal blue

    // 2. Middle rectangle
    glBegin(GL_QUADS);
    glVertex2f(rectWidth, windowHeight - 50.0f);  // Starts where the left rectangle ends
    glVertex2f(rectWidth * 2.0f, windowHeight - 50.0f);  // Width of the middle rectangle
    glVertex2f(rectWidth * 2.0f, windowHeight);
    glVertex2f(rectWidth, windowHeight);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.545f);  // A dark navy blue

    // 3. Top-right rectangle
    glBegin(GL_QUADS);
    glVertex2f(rectWidth * 2.0f, windowHeight - 50.0f);  // Starts where the middle rectangle ends
    glVertex2f(rectWidth * 3.0f, windowHeight - 50.0f);  // Width of the right rectangle
    glVertex2f(rectWidth * 3.0f, windowHeight);
    glVertex2f(rectWidth * 2.0f, windowHeight);
    glEnd();
    
    drawStar(350, 675, 25, 10, 5);
}


void drawCoin(float x, float y) {
// Draw the outer edge of the coin (golden color)
glColor3f(1.0f, 0.84f, 0.0f); // Gold color
drawCircle(x, y, 25.0f,25); // Outer circle

// Draw the inner part of the coin (slightly darker)
glColor3f(0.8f, 0.7f, 0.0f); // Slightly darker gold color
drawCircle(x, y, 15.0f,15); // Inner circle

// Draw the text "x2" in black
glColor3f(0.0f, 0.0f, 0.0f); // Black color
drawText( "x 2",x - 10, y - 10); // Position the text

// Define the points for the inner square
float squareSize = 10.0f; // Size of the inner square
float offsetX = squareSize / 2;
float offsetY = squareSize / 2;



// Define the points for the outer square (higher above the inner square)
float outerSquareSize = 20.0f; // Size of the outer square
float outerOffsetX = outerSquareSize / 2;
float outerOffsetY = outerSquareSize / 2;

// Define the four points for the outer square (above each vertex)
float outerPoints[4][2] = {
{x - outerOffsetX, y + outerOffsetY}, // Top left (above bottom left)
{x + outerOffsetX, y + outerOffsetY}, // Top right (above bottom right)
{x + outerOffsetX, y - outerOffsetY}, // Bottom right (above top right)
{x - outerOffsetX, y - outerOffsetY} // Bottom left (above top left)
};

// Draw the points above each vertex of the inner square
glColor3f(1.0f, 0.0f, 0.0f); // Red color for points
glPointSize(3.0f); // Set the point size to be larger
glBegin(GL_POINTS);
for (int i = 0; i < 4; i++) {
glVertex2f(outerPoints[i][0], outerPoints[i][1]); // Draw each outer point
}
glEnd();
//    glBegin(GL_LINES);
//    glColor3f(0.0f, 0.0f, 1.0f); // Red color for points
//
//    glVertex2f(x ,y +20);
//    glVertex2f(x+20 ,y);
//    glEnd();
}

//-------------------------------- DRAWING HELPERS -----------------------------------

//-------------------------------- COLLISIONS -----------------------------------


bool checkCollision(float obstacleX,float obstacleY, float obstacleHeight) {
    float characterLeft=characterX-50;
    float characterRight=characterX+40;
    float characterBottom=characterY-148;
    float CharacterTop= characterY+60-duckingDistance;
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
    float characterBottom=characterY-148;
    float CharacterTop= characterY+60-duckingDistance;
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
    float characterBottom=characterY-148;
    float CharacterTop= characterY+60-duckingDistance;
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
    float characterBottom=characterY-148;
    float CharacterTop= characterY+60-duckingDistance;
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
//----------------------------- COLLECTIBLES ----------------------------
void updateStars() {
    for (auto &star : stars) {
        star.brightness += star.speed; // Update brightness

        // Reverse speed if brightness goes out of bounds
        if (star.brightness > 1.0f || star.brightness < 0.0f) {
            star.speed = -star.speed;
            star.brightness = std::clamp(star.brightness, 0.0f, 1.0f); // Keep within bounds
        }
    }
}
//----------------------------- COLLECTIBLES ----------------------------



//----------------------------- OBSTACLE GENERATION AND UPDATE ----------------------------
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
    
    int randomItem = rand() % 4; // Generates 0, 1, or 2
    
    if (randomItem == 0) { // Generate a collectible
        Collectible collectible;
        collectible.x = windowWidth; // Start from the right side of the screen
        collectible.height = 20; // Set a fixed height for collectibles
        // Set the collectible's Y position based on obstacle's Y position
        collectible.y = (obstacle.y == 120) ? 350 : 140;
        collecibles.push_back(collectible);
    } else if (randomItem == 1) { // Generate a power-up
        powerUpCoin p;
        p.x = windowWidth; // Start from the right side of the screen
        
        p.y = (obstacle.y == 120) ? 400 : 190;
        coins.push_back(p);
    }
    else if (randomItem==2)
    {
        powerUpFly pf;
        pf.x = windowWidth; // Start from the right side of the screen
        
        pf.y = (obstacle.y == 120) ? 400 : 190;
        flyingPowerUp.push_back(pf);
    }
    else
    {
      //  printf("NOTHING IS ABOVE OR BWLOE OBSTACLE");
    }
}

void updateObstacles(int value) {
    updateStars();
    obstacleTimer += 0.02f;
    speedMultiplier+=0.0001;
    // Generate a new obstacle every 2 seconds
    if (obstacleTimer >= 2.0f) {
        generateObstacle();
        obstacleTimer = 0.0f;
    }
    collectibleRotationAngle += 0.5f;  // Adjust this value for slower or faster rotation
       if (collectibleRotationAngle >= 360.0f) {
           collectibleRotationAngle -= 360.0f;  // Keep the angle within 0-360 degrees
       }
    // Move obstacles to the left
    for (int i = 0; i < obstacles.size(); ++i) {
        obstacles[i].x -= (obstacleSpeed+speedMultiplier); // Move obstacle

        // Check if an obstacle is off the screen
        if (obstacles[i].x < -30.0f) {
            obstacles.erase(obstacles.begin() + i);
            --i;
            continue;
        }
        
        // Check collision with character
        if (checkCollision(obstacles[i].x,obstacles[i].y, obstacles[i].height)) {
            health--;
            // howa 5abat hena
           
            
            if (health <= 0) {
                gameEnded = true;
                // Stop updating the game
                return;
            }
            obstacles.erase(obstacles.begin() + i);
            --i;
        }
    }

    // Move collectibles to the left
    for (int i = 0; i < collecibles.size(); ++i) {
        collecibles[i].x -= (obstacleSpeed+speedMultiplier); // Move collectible to the left
        
        // Check if a collectible is off the screen
        if (collecibles[i].x < -30.0f) {
            collecibles.erase(collecibles.begin() + i);
            --i;
            continue;
        }
        if (checkCollisionCollectible(collecibles[i].x,collecibles[i].y, collecibles[i].height)) {
           score+=scoreAdditionFactor;
            collecibles.erase(collecibles.begin() + i);
            --i;
        }
    }
    for (int i = 0; i < coins.size(); ++i) {
        coins[i].x -= (obstacleSpeed+speedMultiplier); // Move collectible to the left
        
        // Check if a collectible is off the screen
        if (coins[i].x < -30.0f) {
            coins.erase(coins.begin() + i);
            --i;
            continue;
        }
        if (checkCollisionCoin(coins[i].x,coins[i].y)) {
            scoreAdditionFactor=2;
            coins.erase(coins.begin() + i);
            --i;
        }
    }
    for (int i = 0; i < flyingPowerUp.size(); ++i) {
        flyingPowerUp[i].x -= (obstacleSpeed+speedMultiplier); // Move collectible to the left
        
        // Check if a collectible is off the screen
        if (flyingPowerUp[i].x < -30.0f) {
            flyingPowerUp.erase(flyingPowerUp.begin() + i);
            --i;
            continue;
        }
        if (checkCollisionFlying(flyingPowerUp[i].x, flyingPowerUp[i].y)) {
           // printf("I TOOK A FLYING POWER UP");
            isFlying=true;
            flyingPowerUp.erase(flyingPowerUp.begin() + i);
            --i;
        }
    }
   
    
    // Call this function again after 16 ms (~60 FPS)
    glutTimerFunc(16, updateObstacles, 0);
}

//----------------------------- OBSTACLE GENERATION AND UPDATE ----------------------------


//----------------------------- KEYBOARD ACTIONS ----------------------------

void keyPress(unsigned char key, int x, int y) {
    if (key == ' ') { // Space key for jump
        if (!isJumping && characterY ==fixedcharacterY) {
            isJumping = true;
        }
    }
    if (key == GLUT_KEY_DOWN) { // Down arrow to duck
           isDucking = true;
       }
}

// Key release handler
void keyUp(unsigned char key, int x, int y) {
    if (key == ' ') { // Reset position on key release
        isJumping = false;
    }
    if (key == GLUT_KEY_DOWN) { // Reset duck state
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
    if (isJumping) {
        if (characterY < maxjumpHeight) {
            characterY += jumpSpeed;
        } else {
            isJumping = false;  // Stop jumping when max height is reached
        }
    } else if (characterY > fixedcharacterY) {
        characterY -= gravity;  // Gravity pulls the character back down
    } else {
        characterY = fixedcharacterY;  // Ensure the character stays at ground level after falling
    

    }
    if (isFlying)
    {
        characterY = windowHeight - 50.0f;

    }

    glutPostRedisplay(); // Request a redraw
    glutTimerFunc(16, updateCharacter, 0); // Call this function again after 16 ms
}
//----------------------------- CHARACTER ----------------------------


//----------------------------- BACKGROUND ANIMATION ----------------------------

void initStars(int numStars) {
    srand(static_cast<unsigned>(time(0))); // Seed for random number generation
    for (int i = 0; i < numStars; i++) {
        Star star;
        star.x = rand() % windowWidth; // Random x position
        // Set y position to be within the range of 400 to 500
        star.y = rand() % 101 + 450; // Random y position from 400 to 500 (inclusive)
        star.size = rand() % 3 + 2; // Random size between 2 and 5
        star.brightness = static_cast<float>(rand() % 100) / 100.0f; // Random brightness
        star.speed = static_cast<float>(rand() % 20 + 10) / 1000.0f; // Random speed for brightness change
        stars.push_back(star);
    }
}

//----------------------------- BACKGROUND ANIMATION ----------------------------





//--------------------------------DISPLAY -----------------------------------



void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (gameEnded) {
        
       if (health==0)
       {
           char endMessage[50];
           sprintf(endMessage, "YOUU LOST:((");
           
           glColor3f(0.0f, 0.0f, 0.0f); // Set text color to white

           drawText(endMessage, windowWidth / 2 - 100, windowHeight / 2);
           

       }
           else
           {
               char endMessage[50];
               sprintf(endMessage, "Game End! Score: %d", score);
               
               glColor3f(0.0f, 0.0f, 0.0f); // Set text color to white

               drawText(endMessage, windowWidth / 2 - 100, windowHeight / 2); // Center the message
           }
    }
    else {
       
           glClearColor(1.0, 1.0, 1.0, 1.0f); // Set the background color
    

        drawUpperFrame();  // Add the upper frame
        drawHealth();
        drawCharacter();

        for (const auto &star : stars) {
                drawDiamond(star.x, star.y, star.size, star.brightness);
            }
        for (const auto& obstacle : obstacles) {
            drawObstacle(obstacle.x, obstacle.y, obstacle.height);
        }
        for (const auto& collectible : collecibles) {
            drawStarCollectible(collectible.x, collectible.y, 30);
        }
        for (const auto& coin : coins) {
            drawCoin(coin.x,coin.y);
        }
        for (const auto& flying : flyingPowerUp) {
            drawFlyingPowerUp(flying.x,flying.y);
        }
        drawGround();

        // Time Tracking
        static float lastTime = 0.0f; // Variable to store the last frame time
        float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // Get current time in seconds
        elapsedTime += currentTime - lastTime; // Update elapsed time
        lastTime = currentTime; // Update last frame time

        // Score and Time Display
        char scoreText[50];
        char timeString[50];
        sprintf(timeString, "Time: %.2f seconds", elapsedTime);  // Format time as string
        sprintf(scoreText, "Score: %d", score);

        glColor3f(1.0f, 1.0f, 1.0f); // Set text color to white
        drawText(scoreText, windowWidth - 200.0f, windowHeight - 20.0f);
        drawText(timeString, windowWidth - 200.0f, windowHeight - 40.0f);

      // 5 minutes is the game time
        if (elapsedTime >= 300.0f) {
            gameEnded = true;
        }
    }

    glFlush();
}

void timer(int value) {
    timeforBackgroundColor += 0.01f; // Update the time elapsed
    glutPostRedisplay(); // Trigger a redraw
    glutTimerFunc(16, timer, 0); // Call timer again after 16 ms (~60 FPS)
}

//--------------------------------DISPLAY -----------------------------------


//----------------------- RESET GAME (WITH ORIGINAL HEALTH AND SCORE---------------------------

//void resetGame(int score)
//{
//    score=score;
//    characterX=fixedcharacterX;
//    characterY=fixedcharacterY;
//    glutTimerFunc(0, updateCharacter, 0);
//       glutTimerFunc(16, updateObstacles, 0);
//       glutTimerFunc(0, timer, 0);
//
//    
//}

//-------------------------------- MAIN METHOD -----------------------------------

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("UFO ALIEN GAME");

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
    initStars(50); // Initialize 100 stars
    glutDisplayFunc(display);
    glutTimerFunc(0, updateCharacter, 0);
    glutTimerFunc(16, updateObstacles, 0);
    glutTimerFunc(0, timer, 0);
     glutKeyboardFunc(keyPress);
     glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKeyPress); // Special key input
       glutSpecialUpFunc(specialKeyUp);   // Special key release input
    glutMainLoop();

    return 0;
}


