#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

void mov_left();
void mov_right();
void mov_drop();

int stage[21][12] = {{},{}};
int update_stage[21][12] = {{},{}};
int col_flag;
int debug_level;
int score, mino_count=0;
int mino_num[7] = {0,1,2,3,4,5,6};
int now_mino = -1;
int mino_color, mino_stat;
char key;

void mino_shuffle(int ary[]){
    int i,j,t;
    for(i=0;i<7;i++){
        j = rand()%7;
        t = ary[i];
        ary[i] = ary[j];
        ary[j] = t;
    }
}

int kbhit(void)
{

    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

void game_over_check(){
    if(stage[0][5] == 3 || stage[0][6] == 3){
        printf("You Lose\n");
        exit(1);
    }
}



void init(){
    if(debug_level>=1){printf("init\n");}
  int i,r;
  for(i=0;i<21;i++){
    for(r=0;r<12;r++){
      if(i==20 || r == 0 || r == 11){
	stage[i][r] = 2;
      }else{
	stage[i][r] = 0;
      }
    }
  }
}

void stage_update(){
        if(debug_level>=1){printf("stage_update\n");}
    int i,r;
    for(i=0;i<21;i++){
        for(r=0;r<12;r++){
            stage[i][r] = update_stage[i][r];
        }
    }
}

void pull_stage(){
        if(debug_level>=1){printf("pull_stage\n");}
    int i,r;
    for(i=0;i<21;i++){
        for(r=0;r<12;r++){
                update_stage[i][r] = stage[i][r];
            }
        }
}

void row_erase(int t){
    int i,r;
    for(r=1;r<11;r++){stage[t][r]=0;}

    for(i=t-1;i>=0;i--){
        for(r=1;r<11;r++){
            if(stage[i][r]==3){
                stage[i+1][r] = 3;
                stage[i][r] = 0;
            }
        }
    }
    score=score+50;
}

void row_check(){
    int i,r,count;
    for(i=0;i<21;i++){
        count=0;
        for(r=1;r<11;r++){
            if(stage[i][r]==3){
                count++;
            }
        }
        if(count==10){
            row_erase(i);
        }
    }
}


void gen_i_mino(){
  stage[0][4]=1;
  stage[0][5]=1;
  stage[0][6]=1;
  stage[0][7]=1;
    mino_color = 46;
    now_mino = 0;
    mino_stat = 0;
}
void gen_o_mino(){
  stage[0][4]=1;
  stage[0][5]=1;
  stage[1][4]=1;
  stage[1][5]=1;
    mino_color=43;
    now_mino = 1;
    mino_stat = 0;
}
void gen_s_mino(){
  stage[0][4]=1;
  stage[0][5]=1;
  stage[1][3]=1;
  stage[1][4]=1;
    mino_color=42;
    now_mino = 2;
    mino_stat = 0;
}
void gen_z_mino(){
  stage[0][4]=1;
  stage[0][5]=1;
  stage[1][5]=1;
  stage[1][6]=1;
    mino_color=41;
    now_mino = 3;
    mino_stat = 0;

}
void gen_j_mino(){
  stage[0][4]=1;
  stage[1][4]=1;
  stage[1][5]=1;
  stage[1][6]=1;
    mino_color=44;
    now_mino = 4;
    mino_stat = 0;

}
void gen_l_mino(){
  stage[0][7]=1;
  stage[1][5]=1;
  stage[1][6]=1;
  stage[1][7]=1;
    mino_color=47;
    now_mino = 5;
    mino_stat = 0;

}
void gen_t_mino(){
  stage[0][6]=1;
  stage[1][5]=1;
  stage[1][6]=1;
  stage[1][7]=1;
    mino_color=45;
    now_mino = 6;
    mino_stat = 0;

}

void create_mino(char mino){
    mino_count++;
        if(debug_level>=1){printf("create_mino\n");}
  switch(mino){
    case 0:  gen_i_mino(); break;
    case 1:  gen_o_mino(); break;
    case 2:  gen_s_mino(); break;
    case 3:  gen_z_mino(); break;
    case 4:  gen_j_mino(); break;
    case 5:  gen_l_mino(); break;
    case 6:  gen_t_mino(); break;
  }
}

void spin_i(){
    int i,r;
    pull_stage();
    for(i=0;i<21;i++){
        for(r=1;r<12;r++){
            if(mino_stat==0){
                if(update_stage[i][r]==1){
                    if(update_stage[i-1][r+1]==2 || update_stage[i-1][r+1]==3){col_flag=1;return;}
                    else{update_stage[i-1][r+1]=1;}
                    if(update_stage[i+1][r+1]==2 || update_stage[i+1][r+1]==3){col_flag=1;return;}
                    else{update_stage[i+1][r+1]=1;}
                    if(update_stage[i+2][r+1]==2 || update_stage[i+2][r+1]==3){col_flag=1;return;}
                    else{update_stage[i+2][r+1]=1;}
                    update_stage[i][r]=0;
                    update_stage[i][r+2]=0;
                    update_stage[i][r+3]=0;
                    mino_stat = 1;
                    return;
                }
            }
            if(mino_stat==1){
                if(update_stage[i][r]==1){
                    if(update_stage[i+1][r-1]==2 || update_stage[i+1][r-1]==3){col_flag=1;return;}
                    else{update_stage[i+1][r-1]=1;}
                    if(update_stage[i+1][r+1]==2 || update_stage[i+1][r+1]==3){col_flag=1;return;}
                    else{update_stage[i+1][r+1]=1;}
                    if(update_stage[i+1][r+2]==2 || update_stage[i+1][r+2]==3){col_flag=1;return;}
                    else{update_stage[i+1][r+2]=1;}
                    update_stage[i][r]=0;
                    update_stage[i+2][r]=0;
                    update_stage[i+3][r]=0;
                    mino_stat = 0;
                    return;
                }

            }
        }
    }
}

void spin_s(){
    int i,r;
    pull_stage();
    for(i=0;i<21;i++){
        for(r=1;r<12;r++){
            if(mino_stat==0){
                if(update_stage[i][r]==1){
                    if(update_stage[i-1][r-1]==2 || update_stage[i-1][r-1]==3){col_flag=1;return;}
                    else{update_stage[i-1][r-1]=1;}
                    if(update_stage[i][r-1]==2 || update_stage[i][r-1]==3){col_flag=1;return;}
                    else{update_stage[i][r-1]=1;}
                    update_stage[i+1][r-1]=0;
                    update_stage[i][r+1]=0;
                    mino_stat = 1;
                    return;
                }
            }
            if(mino_stat==1){
                if(update_stage[i][r]==1){
                    if(update_stage[i+2][r]==2 || update_stage[i+2][r]==3){col_flag=1;return;}
                    else{update_stage[i+2][r]=1;}
                    if(update_stage[i+1][r+2]==2 || update_stage[i+1][r+2]==3){col_flag=1;return;}
                    else{update_stage[i+1][r+2]=1;}
                    update_stage[i][r]=0;
                    update_stage[i+1][r]=0;
                    mino_stat = 0;
                    return;
                }
                
            }
        }
    }
}

void spin_z(){
    int i,r;
    pull_stage();
    for(i=0;i<21;i++){
        for(r=1;r<12;r++){
            if(mino_stat==0){
                if(update_stage[i][r]==1){
                    if(update_stage[i-1][r+1]==2 || update_stage[i-1][r+1]==3){col_flag=1;return;}
                    else{update_stage[i-1][r+1]=1;}
                    if(update_stage[i+1][r]==2 || update_stage[i+1][r]==3){col_flag=1;return;}
                    else{update_stage[i+1][r]=1;}
                    update_stage[i+1][r+1]=0;
                    update_stage[i+1][r+2]=0;
                    mino_stat = 1;
                    return;
                }
            }
            if(mino_stat==1){
                if(update_stage[i][r]==1){
                    if(update_stage[i+2][r]==2 || update_stage[i+2][r]==3){col_flag=1;return;}
                    else{update_stage[i+2][r]=1;}
                    if(update_stage[i+2][r+1]==2 || update_stage[i+2][r+1]==3){col_flag=1;return;}
                    else{update_stage[i+2][r+1]=1;}
                    update_stage[i][r]=0;
                    update_stage[i+2][r-1]=0;
                    mino_stat = 0;
                    return;
                }
                
            }
        }
    }
}

void spin_j(){
    int i,r;
    pull_stage();
    for(i=0;i<21;i++){
        for(r=1;r<12;r++){
            if(mino_stat==0){
                if(update_stage[i][r]==1){
                    if(update_stage[i][r+1]==2 || update_stage[i][r+1]==3){col_flag=1;return;}
                    else{update_stage[i][r+1]=1;}
                    if(update_stage[i][r+2]==2 || update_stage[i][r+2]==3){col_flag=1;return;}
                    else{update_stage[i][r+2]=1;}
                    if(update_stage[i+2][r+1]==2 || update_stage[i+2][r+1]==3){col_flag=1;return;}
                    else{update_stage[i+2][r+1]=1;}
                    update_stage[i][r]=0;
                    update_stage[i+1][r]=0;
                    update_stage[i+1][r+2]=0;
                    mino_stat = 1;
                    return;
                }
            }
            if(mino_stat==1){
                if(update_stage[i][r]==1){
                    if(update_stage[i+1][r-1]==2 || update_stage[i+1][r-1]==3){col_flag=1;return;}
                    else{update_stage[i+1][r-1]=1;}
                    if(update_stage[i+2][r+1]==2 || update_stage[i+2][r+1]==3){col_flag=1;return;}
                    else{update_stage[i+2][r+1]=1;}
                    if(update_stage[i+1][r+1]==2 || update_stage[i+1][r+1]==3){col_flag=1;return;}
                    else{update_stage[i+1][r+1]=1;}
                    update_stage[i][r]=0;
                    update_stage[i][r+1]=0;
                    update_stage[i+2][r]=0;
                    mino_stat = 2;
                    return;
                }
                
            }
            if(mino_stat==2){
                if(update_stage[i][r]==1){
                    if(update_stage[i-1][r+1]==2 || update_stage[i-1][r+1]==3){col_flag=1;return;}
                    else{update_stage[i-1][r+1]=1;}
                    if(update_stage[i+1][r]==2 || update_stage[i+1][r]==3){col_flag=1;return;}
                    else{update_stage[i+1][r]=1;}
                    if(update_stage[i+1][r+1]==2 || update_stage[i+1][r+1]==3){col_flag=1;return;}
                    else{update_stage[i+1][r+1]=1;}
                    update_stage[i][r]=0;
                    update_stage[i][r+2]=0;
                    update_stage[i+1][r+2]=0;
                    mino_stat = 3;
                    return;
                }
            }
            if(mino_stat==3){
                if(update_stage[i][r]==1){
                    if(update_stage[i][r-1]==2 || update_stage[i][r-1]==3){col_flag=1;return;}
                    else{update_stage[i][r-1]=1;}
                    if(update_stage[i+1][r-1]==2 || update_stage[i+1][r-1]==3){col_flag=1;return;}
                    else{update_stage[i+1][r-1]=1;}
                    if(update_stage[i+1][r+1]==2 || update_stage[i+1][r+1]==3){col_flag=1;return;}
                    else{update_stage[i+1][r+1]=1;}
                    update_stage[i][r]=0;
                    update_stage[i+2][r-1]=0;
                    update_stage[i+2][r]=0;
                    mino_stat = 0;
                    return;
                }
            }
        }
    }
}

void spin_l(){
    int i,r;
    pull_stage();
    for(i=0;i<21;i++){
        for(r=1;r<12;r++){
            if(mino_stat==0){
                if(update_stage[i][r]==1){
                    if(update_stage[i-1][r-1]==2 || update_stage[i-1][r-1]==3){col_flag=1;return;}
                    else{update_stage[i-1][r-1]=1;}
                    if(update_stage[i][r-1]==2 || update_stage[i][r-1]==3){col_flag=1;return;}
                    else{update_stage[i][r-1]=1;}
                    update_stage[i][r]=0;
                    update_stage[i+1][r-2]=0;
                    mino_stat = 1;
                    return;
                }
            }
            if(mino_stat==1){
                if(update_stage[i][r]==1){
                    if(update_stage[i+1][r+1]==2 || update_stage[i+1][r+1]==3){col_flag=1;return;}
                    else{update_stage[i+1][r+1]=1;}
                    if(update_stage[i+1][r-1]==2 || update_stage[i+1][r-1]==3){col_flag=1;return;}
                    else{update_stage[i+1][r-1]=1;}
                    if(update_stage[i+2][r-1]==2 || update_stage[i+2][r-1]==3){col_flag=1;return;}
                    else{update_stage[i+2][r-1]=1;}
                    update_stage[i][r]=0;
                    update_stage[i+2][r]=0;
                    update_stage[i+2][r+1]=0;
                    mino_stat = 2;
                    return;
                }
                
            }
            if(mino_stat==2){
                if(update_stage[i][r]==1){
                    if(update_stage[i-1][r]==2 || update_stage[i-1][r]==3){col_flag=1;return;}
                    else{update_stage[i-1][r]=1;}
                    if(update_stage[i-1][r+1]==2 || update_stage[i-1][r+1]==3){col_flag=1;return;}
                    else{update_stage[i-1][r+1]=1;}
                    if(update_stage[i+1][r+1]==2 || update_stage[i+1][r+1]==3){col_flag=1;return;}
                    else{update_stage[i+1][r+1]=1;}
                    update_stage[i][r]=0;
                    update_stage[i][r+2]=0;
                    update_stage[i+1][r]=0;
                    mino_stat = 3;
                    return;
                }
            }
            if(mino_stat==3){
                if(update_stage[i][r]==1){
                    if(update_stage[i+1][r]==2 || update_stage[i+1][r]==3){col_flag=1;return;}
                    else{update_stage[i+1][r]=1;}
                    if(update_stage[i][r+2]==2 || update_stage[i][r+2]==3){col_flag=1;return;}
                    else{update_stage[i][r+2]=1;}
                    if(update_stage[i+1][r+2]==2 || update_stage[i+1][r+2]==3){col_flag=1;return;}
                    else{update_stage[i+1][r+2]=1;}
                    update_stage[i][r]=0;
                    update_stage[i][r+1]=0;
                    update_stage[i+2][r+1]=0;
                    mino_stat = 0;
                    return;
                }
            }
        }
    }
}

void spin_t(){
    int i,r;
    pull_stage();
    for(i=0;i<21;i++){
        for(r=1;r<12;r++){
            if(mino_stat==0){
                if(update_stage[i][r]==1){
                    if(update_stage[i+2][r]==2 || update_stage[i+2][r]==3){col_flag=1;return;}
                    else{update_stage[i+2][r]=1;}
                    update_stage[i+1][r-1]=0;
                    mino_stat = 1;
                    return;
                }
            }
            if(mino_stat==1){
                if(update_stage[i][r]==1){
                    if(update_stage[i+1][r-1]==2 || update_stage[i+1][r-1]==3){col_flag=1;return;}
                    else{update_stage[i+1][r-1]=1;}
                    update_stage[i][r]=0;
                    mino_stat = 2;
                    return;
                }
                
            }
            if(mino_stat==2){
                if(update_stage[i][r]==1){
                    if(update_stage[i-1][r+1]==2 || update_stage[i-1][r+1]==3){col_flag=1;return;}
                    else{update_stage[i-1][r+1]=1;}
                    update_stage[i][r+2]=0;
                    mino_stat = 3;
                    return;
                }
            }
            if(mino_stat==3){
                if(update_stage[i][r]==1){
                    if(update_stage[i+1][r+1]==2 || update_stage[i+1][r+1]==3){col_flag=1;return;}
                    else{update_stage[i+1][r+1]=1;}
                    update_stage[i+2][r]=0;
                    mino_stat = 0;
                    return;
                }
            }
        }
    }
}

void spin_mino(){
    switch (now_mino) {
        case 0:  spin_i(); break;
        case 1:  break;
        case 2:  spin_s(); break;
        case 3:  spin_z(); break;
        case 4:  spin_j(); break;
        case 5:  spin_l(); break;
        case 6:  spin_t(); break;
        default:  break;
    }
}



void disp_stage(){
        if(debug_level>=1){printf("disp_stage\n");}
  int i,r;
    printf("\n\n");
  for(i=0;i<21;i++){
    for(r=0;r<12;r++){
        if(stage[i][r]==2){printf("\x1b[47m");printf("  ");printf("\x1b[49m");}
        if(stage[i][r]==1){printf("\x1b[%dm", mino_color);printf("  ");printf("\x1b[49m");}
      if(stage[i][r]==3){printf("\x1b[43m");printf("  ");printf("\x1b[49m");}
      if(stage[i][r]==0){printf("  ");}
    }
    if(i==4){printf("\x1b[32m");printf("  Player1 SCORE %d\n", score);printf("\x1b[39m");
    }else if(i==6){
        printf("\x1b[31m");printf("  Player2 SCORE ?????\n");printf("\x1b[39m");
    }else{
        printf("\n");
    }
  }

  printf("\n");
}

void land_mino(){
        if(debug_level>=1){printf("land_mino\n");}
  int i,r;
  for(i=0;i<21;i++){
    for(r=0;r<12;r++){
      if(stage[i][r]==1){
	stage[i][r] = 3;
      }
    }
  }
  row_check();
  game_over_check();
}

void gravity(){
        if(debug_level>=1){printf("gravity\n");}
    int i,r;
    clock_t start, end;
    start = clock();
    end = clock();
    while(end-start<1000000){
        col_flag=0;
      kbhit();
        key = getchar();
        switch(key){
            case 'k':
                mov_left();if(col_flag==0){stage_update();}disp_stage();
                break;
            case 'l':
                mov_right();if(col_flag==0){stage_update();}disp_stage();
                break;
            case 'z':
                mov_drop();if(col_flag==0){stage_update();}disp_stage();
                break;
            case 'c':
                spin_mino();if(col_flag==0){stage_update();}disp_stage();
                break;
        }

      end = clock();
    }
    pull_stage();
    for(i=19;i>=0;i--){
      for(r=0;r<12;r++){
        if(update_stage[i][r] == 1){
          if(update_stage[i+1][r] == 2 || update_stage[i+1][r] == 3){
              col_flag=1;
          }else{
            update_stage[i][r] = 0;
            update_stage[i+1][r] = 1;
          }
        }
      }
    }
    if(col_flag==1){
        land_mino();
        if(mino_count==7){mino_shuffle(mino_num);mino_count=0;}
        create_mino(mino_num[mino_count]);
    }else{
    stage_update();
    }
}

void mov_left(){
        if(debug_level>=1){printf("mov_left\n");}
  int i,r;
    pull_stage();
  for(i=0;i<21;i++){
    for(r=0;r<12;r++){
      if(update_stage[i][r]==1){
	if(update_stage[i][r-1]==0){
	  update_stage[i][r]=0;
	  update_stage[i][r-1]=1;
    }else{
        col_flag=1;
    }
      }
    }
  }
}

void mov_right(){
    if(debug_level>=1){printf("mov_right\n");}
  int i,r;
    pull_stage();
  for(i=0;i<21;i++){
    for(r=11;r>=0;r--){
      if(update_stage[i][r]==1){
	if(update_stage[i][r+1]==0){
	  update_stage[i][r]=0;
      update_stage[i][r+1]=1;
    }else{
        col_flag=1;
    }
      }
    }
  }
}

void mov_drop(){
    int i,r;
    while(col_flag==0){
    pull_stage();
    for(i=19;i>=0;i--){
        for(r=0;r<12;r++){
            if(update_stage[i][r] == 1){
                if(update_stage[i+1][r] == 2 || update_stage[i+1][r] == 3){
                    col_flag=1;
                }else{
                    update_stage[i][r] = 0;
                    update_stage[i+1][r] = 1;
                }
            }
        }
    }
        if(col_flag==0){stage_update();}
    }
        land_mino();
    if(mino_count==7){mino_shuffle(mino_num);mino_count=0;}
        create_mino(mino_num[mino_count]);
}


void tetris(){
  init();
    mino_shuffle(mino_num);
  create_mino(mino_num[mino_count]);

    while(1){
        col_flag = 0;
        disp_stage();
        gravity();
      
    }
}

int main(){
  debug_level=0;
  fcntl(0, F_SETFL, O_NONBLOCK);
    
    
  
  tetris();


  return 0;
}
