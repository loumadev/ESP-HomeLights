int snakeX = 0;
int snakeY = 0;

int snakeXval = 1;
int snakeYval = 0;

int snakeSpeed = 300;

int snakeState = false;

int snakeSnake[3][2] = {
  {0,0},
  {0,1},
  {0,2}
};

long snakeTime;

bool snakeGetState() {
  return snakeState;
}

void snakeSetState(bool state) {
  snakeState = state;
  snakeTime = millis();
}

void snakeMove(int dir) {   //0:w, 1:a, 2:s, 3:d
  switch(dir) {
    case 0: snakeXval = 0; snakeYval = -1; break;
    case 1: snakeXval = -1; snakeYval = 0; break;
    case 2: snakeXval = 0; snakeYval = 1; break;
    case 3: snakeXval = 1; snakeYval = 0; break;
  }
}

void snakeHandler() {
  if(!snakeState) return;
  if(snakeTime > millis()) return;
  
  int a = s-1;

  for(int i=0;i<3;i++) {
    int b = snakeSnake[i][0] + snakeXval;
    int c = snakeSnake[i][1] + snakeYval;
    
    if(b<0) snakeSnake[i][0] = a;
    else if(b>s) snakeSnake[i][0] = 0;
    else snakeSnake[i][0] += snakeXval;
    
    if(c<0) snakeSnake[i][1] = a;
    else if(b>s) snakeSnake[i][1] = 0;
    else snakeSnake[i][1] += snakeYval;
  }
  
 snakeTime = millis() + snakeSpeed;

  int Buffer[s][s] = {
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0}
  };

  for(int i=0;i<3;i++) {
    Buffer[snakeSnake[i][0]][snakeSnake[i][1]] = 1;
  }
  
  draw(Buffer, 1);
}


