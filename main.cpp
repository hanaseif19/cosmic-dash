
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
// for the animated background stars (the greyscale ones)
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

//    if (isDucking) {
        
        glColor3f(0.3f, 0.8f, 0.3f);
        drawCircle(characterX,characterY-duckingDistance, 30.0f, 50);
        glColor3f(1.0f, 0.4f, 0.5f);
   
            drawTriangle(characterX-15, characterY+25-duckingDistance, characterX+15,characterY+25-duckingDistance, characterX, characterY+60-duckingDistance);
    
        glColor3f(1.0f, 1.0f, 1.0f);
 
            drawCircle(characterX-10, characterY+10-duckingDistance, 10.0f, 30);
           drawCircle(characterX+15, characterY+10-duckingDistance, 10.0f, 30);
    
        glColor3f(0.0f, 0.0f, 0.0f);
        drawCircle(characterX-8.0, characterY+12-duckingDistance, 4.0f, 20);
        drawCircle(characterX+17, characterY+12-duckingDistance, 4.0f, 20);
    
        glColor3f(0.3f, 0.8f, 0.3f);
    
            glBegin(GL_LINES);
            glVertex2f(characterX-20, characterY-10-duckingDistance); glVertex2f(characterX-50, characterY-150);
            glVertex2f(characterX-10, characterY-10-duckingDistance); glVertex2f(characterX-20, characterY-150);
            glVertex2f(characterX, characterY-10-duckingDistance); glVertex2f(characterX+10, characterY-150);
            glVertex2f(characterX+10, characterY-10-duckingDistance); glVertex2f(characterX+40, characterY-150);
            glEnd();
            glColor3f(1.0f, 0.0f, 1.0f);
            glPointSize(5.0f);
            glBegin(GL_POINTS);
            glVertex2f(characterX-50, characterY-148);
            glVertex2f(characterX-20,  characterY-148);
            glVertex2f(characterX+10,  characterY-148);
            glVertex2f(characterX+40,  characterY-148);
            glEnd();
           glEnd();
      

  
}

void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawHealth() {
   
        float spacing = 35.0f; // Spacing between rockets
        float startX = 30.0f;  // Starting x position

        for (int i = 0; i < health; i++) {
            drawRocket(startX + i * spacing, windowHeight-120); // Draw rockets horizontally
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
    glColor3f(0.0f, 0.0f, 0.0f); // Set star color to yellow

    glBegin(GL_LINE_LOOP); // Start drawing a line loop
    for (int i = 0; i < points * 2; i++) {
        float angle = i * M_PI / points; // Calculate angle
        float r = (i % 2 == 0) ? outerRadius : innerRadius;

        glVertex2f(centerX + cos(angle) * r, centerY + sin(angle) * r);
    }
    glEnd();
}
void drawUpperFrame() {
    glColor3f(0.5f, 0.5f, 0.5f);  // Gray color

    // 1. Top-left rectangle
    glBegin(GL_QUADS);
    glVertex2f(0.0f, windowHeight - 50.0f);
    glVertex2f(150.0f, windowHeight - 50.0f);
    glVertex2f(150.0f, windowHeight);
    glVertex2f(0.0f, windowHeight);
    glEnd();
    glColor3f(0.4f, 0.7f, 1.0f);  // Gray color
    // 2. Top-right rectangle
    glBegin(GL_QUADS);
    glVertex2f(windowWidth - 150.0f, windowHeight - 50.0f);
    glVertex2f(windowWidth, windowHeight - 50.0f);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(windowWidth - 150.0f, windowHeight);
    glEnd();

    glColor3f(0.8f, 1.0f, 0.2f);  // Gray color
    glBegin(GL_QUADS);
    glVertex2f(150.0f, windowHeight - 50.0f);
    glVertex2f(windowWidth - 150.0f, windowHeight - 50.0f);
    glVertex2f(windowWidth - 150.0f, windowHeight);
    glVertex2f(150.0f, windowHeight);
    glEnd();
    drawStar(350, 675, 25,10, 5);

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
    Collectible collectible;
    collectible.x = windowWidth; // Start from the right side of the screen
    collectible.height = 20; // Set a fixed height for collectibles

    if (rand() % 2 == 0) {
        obstacle.y = 120;
        collectible.y = 350;
    } else {
        obstacle.y = 270;
        collectible.y = 140;
    }

    
    collecibles.push_back(collectible);
   
    obstacles.push_back(obstacle);
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
           score++;
            collecibles.erase(collecibles.begin() + i);
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

        glColor3f(0.0f, 0.0f, 0.0f); // Set text color to white
        drawText(scoreText, windowWidth - 200.0f, windowHeight - 70.0f);
        drawText(timeString, windowWidth - 200.0f, windowHeight - 90.0f);

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

