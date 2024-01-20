#include <TFT.h>
#include <SPI.h>

#define cs   10
#define dc   9
#define rst  8
#define leftDownBtn 3
#define leftUpBtn 2
#define rightDownBtn 5
#define rightUpBtn 6

// screen
TFT screen = TFT(cs, dc, rst);
int width, height;
  // .setTextSize(int);
  // .text(string);

// ball
int defaultSpeed = 4;
int oldBallX, oldBallY;
int ballX, ballY;
int ballSize = 4;
int ballXDirection = defaultSpeed;
int ballYDirection = defaultSpeed;

// paddles
int paddleHeight = 40;
int paddleWidth = 4;
int paddleJump = 5;
int leftY, rightY;
int oldLeftY, oldRightY;
int leftPaddleX;
int rightPaddleX;

// util
bool isChanged = false;

// setup
void setup()
{
  screen.begin();
  screen.background(0, 0, 0);
  width = screen.width();
  height = screen.height();

  leftY = rightY = oldRightY = oldLeftY = height / 2 - 20;
  leftPaddleX = 0;
  rightPaddleX = width - 4;
  
  ballX = oldBallX = width / 2 - 12; // da e slightly off-center
  ballY = oldBallY = height / 2 - 12;

  drawBackground();
  //draw(ballX, ballY, ballSize, ballSize);
}

// main
void loop()
{ 
  if(!(digitalRead(leftUpBtn) == LOW && digitalRead(leftDownBtn) == LOW &&
     digitalRead(rightUpBtn) == LOW && digitalRead(rightDownBtn) == LOW))
     {
      coverOld(leftPaddleX, oldLeftY, paddleWidth, paddleHeight);
      coverOld(rightPaddleX, oldRightY, paddleWidth, paddleHeight);
  
      updatePaddle(leftY, oldLeftY, leftDownBtn, leftUpBtn);
      updatePaddle(rightY, oldRightY, rightUpBtn, rightDownBtn); // switch the buttons later
  
      draw(leftPaddleX, leftY, paddleWidth, paddleHeight);
      draw(rightPaddleX, rightY, paddleWidth, paddleHeight);
     }

  coverOld(oldBallX, oldBallY, ballSize, ballSize);
  draw(ballX, ballY, ballSize, ballSize);
  moveBall();
  
  
  delay(33);
}

 // -------------------------------------------------------------------
void endScreen()
{
  screen.background(0, 0, 0);
  screen.stroke(255, 255, 255);
  screen.setTextSize(2);
  screen.text("Game over", width / 6, height / 3);
  screen.setTextSize(1);
  screen.text("Restarting in 3", width / 5, height / 3 + 25);
  delay(3000);
  screen.noStroke();
  screen.background(0, 0, 0);
  drawBackground();
}

void drawBackground()
{
  screen.stroke(100, 100, 100);
  screen.fill(0, 0, 0);
  screen.circle(width / 2, height / 2, 37); 
  screen.noStroke();
  
  screen.fill(100, 100, 100);
  screen.rect(width / 2, 0, 1, height);

  screen.fill(100, 100, 100);
  screen.circle(width / 2, height / 2, 3);  

  // paddles and ball
  draw(leftPaddleX, leftY, paddleWidth, paddleHeight);
  draw(rightPaddleX, rightY, paddleWidth, paddleHeight);
}

void updatePaddle(int& currentY, int& oldY, const int& downBtn, const int& upBtn)
{
  oldY = currentY;
  if(digitalRead(downBtn) == HIGH && digitalRead(upBtn) == HIGH) {
    return;
  }
  
  if(digitalRead(downBtn) == HIGH)
  {
    // reached the bottom
    if(currentY + paddleHeight > height) {
      currentY = height - paddleHeight;
    }
    else{
      currentY += paddleJump;
    }
  }

  if(digitalRead(upBtn) == HIGH)
  {
    if(currentY - paddleJump < 0) {
      currentY = 0;
    }
    else {
      currentY -= paddleJump;
    }
  }
}

void coverOld(int oldX, int oldY, int width, int height)
{
  screen.fill(0, 0, 0);
  screen.rect(oldX, oldY, width, height);
}

void draw(int currentX, int currentY, int width, int height)
{
  // 150, 10, 100 purple (og color)
  screen.fill(100, 100, 100);
  screen.rect(currentX, currentY, width, height);
}

void moveBall()
{
  if(ballX == 0 || ballX + ballSize == width)
  {
    delay(500);
    draw(ballX, ballY, ballSize, ballSize);
    endScreen();
    reset();
    return;
  } 

  // ball movement
  oldBallX = ballX;
  oldBallY = ballY;
  ballX += ballXDirection;
  ballY += ballYDirection;

  validateBallPosition();
  
  if(collideWithLeft() || collideWithRight())
  {    
    ballXDirection *= -1;
    ballYDirection--;
    return;
  }

  if(ballX == 0 || ballX >= width - ballSize) // can be == ig
  {
    bounceFromSide();
  }

  if(ballY == 0 || ballY >= height - ballSize)
  {
    bounceFromCeilingOrFloor();
  }
}


void reset()
{
  ballX = width / 2;
  ballY = height / 2;
  ballXDirection = defaultSpeed;
  ballYDirection = defaultSpeed;
}

bool collideWithLeft()
{
  return ballX == leftPaddleX + ballSize && leftY <= ballY && ballY <= leftY + paddleHeight;
}

bool collideWithRight()
{
  return ballX + ballSize == rightPaddleX && rightY <= ballY + ballSize && ballY + ballSize <= rightY + paddleHeight;
}

void bounceFromSide()
{
  ballXDirection *= -1;
}

void bounceFromCeilingOrFloor()
{
  ballYDirection *= -1;
}

void validateBallPosition()
{
  if(ballX < 0)
  {
    ballX = 0;
  }

  if(ballY < 0)
  {
    ballY = 0;
  }

  if(ballX > width)
  {
    ballX = width;
  }

  if(ballY > height)
  {
    ballY = height;
  }
}
