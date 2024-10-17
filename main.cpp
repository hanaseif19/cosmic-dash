
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>  // Use <GL/freeglut.h> on Windows/Linux
#include <cmath>
#include <cstdio>  // For sprintf
#include <iostream>
#include <vector>

// window size settings
int windowWidth = 700;
int windowHeight = 700;
//score
int score = 0;
// health
int health = 5;
// for obstacles movement
float speedMultiplier=0;
float speedMultiplier2=0;
float timeforBackgroundColor=0.0f;
//float ufoY = 120;
//float ufoXChange = -0.8f;
//float UFOoriginalX;
//float UFOoriginalY;
float elapsedTime = 0.0f; // Global variable to track elapsed time
float ufoX1 = windowWidth + 50.0f;  // Start first UFO outside the right edge

float ufoX2 = windowWidth + 500.0f;
float ufoY2 = 300;


float characterX = 100.0f;
float characterY = 250.0f;
float fixedcharacterX = 100.0f;
float fixedcharacterY = 250.0f;

bool isJumping = false;
float maxjumpHeight = 400.0f;
float gravity=3.0f;
float jumpSpeed=50.0f;
bool isDucking = false;
float duckingDistance=0;
bool gameEnded = false; // Variable to check if the game has ended

struct Obstacle {
    float x;       // Horizontal position
    float y;       // Vertical position (ground level or upper level)
    float height;  // Height of the obstacle (always 50 in this case)
};
struct Collectible {
    float x;       // Horizontal position
    float y;       // Vertical position (ground level or upper level)
    float height;  // Height of the obstacle (always 50 in this case)
};
std::vector<Collectible> collecibles;

std::vector<Obstacle> obstacles;
float obstacleTimer = 0.0f;
float obstacleSpeed = 4.0f;
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
    float collectibleRight=collectibleX+20;
    float collectibleTop=collectibleY+20;
    float collectibleBottom=collectibleY-20;
    float collectibleLeft=collectibleX-20;
    bool isColliding = !(characterRight < collectibleLeft ||
                         characterLeft > collectibleRight ||
                         CharacterTop < collectibleBottom ||
                         characterBottom > collectibleTop);
    return isColliding;

}
// generate obstacles randomly either at a level of the character's head or on the ground
void generateObstacle() {
    Obstacle obstacle;
    obstacle.x = windowWidth;
    obstacle.height = 50;
    Collectible collectible;
    collectible.x = windowWidth; // Start from the right side of the screen
    collectible.height = 20; // Set a fixed height for collectibles

    if (rand() % 2 == 0) {
        obstacle.y = 120;        // Ground obstacle
        collectible.y = 250; // Above the ground obstacle
    } else {
        obstacle.y = 270;      // Upper obstacle
        collectible.y = 120; // Below the upper obstacle
    }

    
    collecibles.push_back(collectible);
   
    obstacles.push_back(obstacle);
}





void updateObstacles(int value) {
    obstacleTimer += 0.02f;
    
    // Generate a new obstacle every 2 seconds
    if (obstacleTimer >= 2.0f) {
        generateObstacle();
        obstacleTimer = 0.0f;
    }
    
    // Move obstacles to the left
    for (int i = 0; i < obstacles.size(); ++i) {
        obstacles[i].x -= obstacleSpeed; // Move obstacle

        // Check if an obstacle is off the screen
        if (obstacles[i].x < -30.0f) {
            obstacles.erase(obstacles.begin() + i);
            --i;
            continue;
        }
        
        // Check collision with character
        if (checkCollision(obstacles[i].x,obstacles[i].y, obstacles[i].height)) {
            health--;
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
        collecibles[i].x -= obstacleSpeed; // Move collectible to the left
        
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

void drawHealth() {
   
        float spacing = 35.0f; // Spacing between rockets
        float startX = 30.0f;  // Starting x position

        for (int i = 0; i < health; i++) {
            drawRocket(startX + i * spacing, windowHeight-120); // Draw rockets horizontally
        }
}
void updateCharacter(int value) {
//    if (isJumping) {
//        if (characterY < maxjumpHeight) {
//            characterY += jumpSpeed;}
////        } else {
////            isJumping = false;
////        }
//    } else {
//        if (characterY > 250.0f) {
//            characterY -= gravity;
//        }
//        if (isDucking) {
//               characterY = 200.0f;  // Lower the character to simulate ducking
//           } else {
//               characterY = 250.0f;  // Restore to normal height when not ducking
//           }
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


// Function to draw the star collectible
void drawStarCollectible(float x, float y, float size) {
    // Draw Star (Polygon)
    glBegin(GL_POLYGON); // Start drawing the star shape
    glColor3f(0.0f, 1.0f, 1.0f); // Star color: cyan
    
//    for (int i = 0; i < 6; i++) {
//        float angle = i * 2.0f * M_PI / 6; // Calculate angle for each vertex
//        float vertexX = x + size * cos(angle); // X coordinate
//        float vertexY = y + size * sin(angle); // Y coordinate
//        glVertex2f(vertexX, vertexY); // Define the vertex
//    }
     
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

    // Print the coordinates of the collectible
   
}


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

void drawUFO(float x, float y) {
    glColor3f(0.7f, 0.7f, 0.0f);
    drawEllipse(x, y, 50.0f, 15.0f, 50);

    glColor3f(0.0f, 0.7f, 0.0f);
    drawCircle(x, y + 10.0f, 20.0f, 30);

    glColor3f(1.0f, 0.0f, 0.0f);
    drawCircle(x - 15.0f, y - 2.5f, 2.5f, 20);
    drawCircle(x, y - 2.5f, 2.5f, 20);
    drawCircle(x + 15.0f, y - 2.5f, 2.5f, 20);
}

//void updateUFO(int value) {
//  
//    ufoX1 += (speedMultiplier+ufoXChange);
//    ufoX2 += (speedMultiplier2+ufoXChange);
//
////    std::cout << "The value is: " << ufoX1 << std::endl;
//    speedMultiplier-=0.001;
//    speedMultiplier2-=0.0001;
//    std::cout << "The value is: " << speedMultiplier << std::endl;
//
//   
//    if (ufoX1 <= -50.0f) {
//        ufoX1 = windowWidth + 50.0f;  // Reset position to the right side
//    }
//
//    
//     if (ufoX2 <= -50.0f) {
//         ufoX2 = windowWidth + 100.0f;  // Reset position to the right side
//     }
//
//
//    glutTimerFunc(16, updateUFO, 0);  // Schedule next update (~60 FPS)
//    glutPostRedisplay();  // Redraw the screen
//}
void drawObstacle(float x, float y, float height) {
//    glColor3f(0.8f, 0.0f, 0.0f);  // Red color for obstacle
//    glBegin(GL_QUADS);
//        glVertex2f(x, y);                 // Bottom-left
//        glVertex2f(x + 30.0f, y);         // Bottom-right
//        glVertex2f(x + 30.0f, y + height); // Top-right
//        glVertex2f(x, y + height);        // Top-left
//    glEnd();
   
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

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (gameEnded) {
        
       if (health==0)
       {
           char endMessage[50];
           sprintf(endMessage, "YOUU LOST");
           
           glColor3f(1.0f, 1.0f, 1.0f); // Set text color to white
           drawText(endMessage, windowWidth / 2 - 100, windowHeight / 2);
           

       }
           else
           {
               char endMessage[50];
               sprintf(endMessage, "Game End! Score: %d", score);
               
               glColor3f(1.0f, 1.0f, 1.0f); // Set text color to white
               drawText(endMessage, windowWidth / 2 - 100, windowHeight / 2); // Center the message
           }
    }
    else {
        // Background Color Setup
        float baseR = 0.2f; // Darker red
        float baseG = 0.2f; // Darker green
        float baseB = 0.4f; // Darker blue (to give a night sky feel)

        float pastelFactor = 0.5f; // Controls how pastel the colors are
        float r = pastelFactor * ((sin(timeforBackgroundColor) + 1.0f) / 2.0f) + (1.0f - pastelFactor) * baseR; // Blend with darker base
        float g = pastelFactor * ((cos(timeforBackgroundColor) + 1.0f) / 2.0f) + (1.0f - pastelFactor) * baseG; // Blend with darker base
        float b = pastelFactor * ((sin(timeforBackgroundColor + 3.14f) + 1.0f) / 2.0f) + (1.0f - pastelFactor) * baseB; // Blend with darker base

        // Draw Background
        glBegin(GL_QUADS);
        glColor3f(r, g, b); // Set color based on time
        glVertex2f(0, 0); // Bottom left
        glVertex2f(windowWidth, 0);  // Bottom right
        glVertex2f(windowWidth, windowHeight);   // Top right
        glVertex2f(0, windowHeight);  // Top left
        glEnd();

        drawUpperFrame();  // Add the upper frame
        drawHealth();
        drawCharacter();

        for (const auto& obstacle : obstacles) {
            drawObstacle(obstacle.x, obstacle.y, obstacle.height);
        }
        for (const auto& collectible : collecibles) {
            drawStarCollectible(collectible.x, collectible.y, 40);
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
        drawText(scoreText, windowWidth - 200.0f, windowHeight - 70.0f);
        drawText(timeString, windowWidth - 200.0f, windowHeight - 90.0f);

        // Check if 5 minutes have passed
        if (elapsedTime >= 300.0f) {
            gameEnded = true; // Set game end flag to true
        }
    }

    glFlush();
}

void timer(int value) {
    timeforBackgroundColor += 0.01f; // Update the time elapsed
    glutPostRedisplay(); // Trigger a redraw
    glutTimerFunc(16, timer, 0); // Call timer again after 16 ms (~60 FPS)
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
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("UFOs Moving Towards Character - OpenGL");

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);

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

