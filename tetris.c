#include "tetris.h"

static struct sigaction act, oact;
int B, count, score_number=0;
typedef struct node *Node;
struct node{
        char name[NAMELEN+1];
        int score;
        Node link;
};
Node Head = NULL;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

        createRankList();

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
                case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

        

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++){
		for(i=0;i<WIDTH;i++){
			field[j][i]=0;
                }
        }

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
        nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
        move(9,WIDTH+10);
        DrawBox(9,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(16,WIDTH+10);
	printw("SCORE");
	DrawBox(17,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(18,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	
	for( int i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( int j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

        for( int i = 0; i < 4; i++ ){
                move(10+i,WIDTH+13);
                for( int j = 0; j < 4; j++ ){
                        if( block[nextBlock[2]][0][i][j] == 1 ){
                                attron(A_REVERSE);
                                printw(" ");
                                attroff(A_REVERSE);
                        }
                        else printw(" ");
                }
        }
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
                //알람 1로 설정
                alarm(1);
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
        for(int i=0;i<4;i++){
                for(int j=0;j<4;j++){
                        if(block[currentBlock][blockRotate][i][j]==1){
                                //예상되는 블록의 위치가 범위를 벗어나거나 다른 블록이 있는 위치와 겹칠 경우 
                                if(i+blockY>=HEIGHT || j+blockX<0 || j+blockX>=WIDTH || f[i+blockY][j+blockX]==1) return 0;
                        }

                }
        }
        return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code

	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 

        int prevBlockRotate=blockRotate, prevBlockY=blockY, prevBlockX=blockX;
        //바뀌기 전 블록 정보 저장
        switch(command){
                case KEY_UP:
                        prevBlockRotate=(prevBlockRotate-1)%4;
                case KEY_DOWN:
                        prevBlockY--;
                case KEY_RIGHT:
                        prevBlockX--;
                case KEY_LEFT:
                        prevBlockX++;
                default:
                        break;
        }

        //바뀌기 전 블록 필드에서 지움
        for(int i=0;i<4;i++){
                for(int j=0;j<4;j++){
                        if(block[currentBlock][prevBlockRotate][i][j]==1) f[i+prevBlockY][j+prevBlockX]=0;
                }
        }
        //필드 갱신
        DrawField();
        
        //현재 블록 필드에 그림
        DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
        //커서를 필드 밖으로 이동
        move(HEIGHT, WIDTH+1);
}

void BlockDown(int sig){
	// user code

	//강의자료 p26-27의 플로우차트를 참고한다.

        if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)==1){
                blockY++;
                DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
        }
        else{
                //블록을 더 이상 쌓을 수 없으면 게임 오버
                if(blockY==-1) gameOver=1;
                else{
                        //블록을 필드에 합친다
                        score+=AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
                        //완전히 채워진 라인을 지우고 점수 업데이트
                        score+=DeleteLine(field);
                        //다음에 나올 블록 생성
                        nextBlock[0]=nextBlock[1];
                        nextBlock[1]=nextBlock[2];
                        nextBlock[2]=rand()%7;
                        //블록 변수 초기화
                        blockRotate=0; blockY=-1; blockX=WIDTH/2-2;
                        //다음 블록 그림
                        DrawNextBlock(nextBlock);
                        //점수 출력
                        PrintScore(score);
                        //새 블록 그림
                        DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
                }
                DrawField();
        }
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code

        int touched=0;

	//Block이 추가된 영역의 필드값을 바꾼다.
        for(int i=0;i<4;i++){
                for(int j=0;j<4;j++){
                        if(block[currentBlock][blockRotate][i][j]==1){
                                f[i+blockY][j+blockX]=1;
                                //블록이 필드와 닿아 있을 때
                                if(i+blockY==HEIGHT-1) touched++;
                        }
                }
        }
        //필드 갱신
        DrawField();
        return touched*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.

        int deleteLineNum=0, flag;

        for(int i=0;i<HEIGHT;i++){
                //줄이 다 채워져 있는 것으로 초기화
                flag=1;
                for(int j=0;j<WIDTH;j++){
                        //채워지지 않은 부분이 있으면 flag 체크를 해제
                        if(f[i][j]==0){
                                flag=0;
                                break;
                        }
                }
                
                //한 줄이 채워져 있을 경우
                if(flag){
                        //지워진 라인의 수 1 증가
                        deleteLineNum++;
                        //꽉 찬 구간 지우고 위의 필드값 한칸씩 내림
                        for(int j=0;j<WIDTH;j++) f[i][j]=0;
                        for(int y=i-1;y>=0;y--){
                                for(int x=0;x<WIDTH;x++){
                                        f[y+1][x]=f[y][x];
                                }
                        }
                        //필드 갱신
                        DrawField();
                } 
        }
        //점수 계산해서 리턴
        return deleteLineNum*deleteLineNum*100;
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	//블록의 그림자를 그림

        int i, j, s=0, empty=1, line=HEIGHT-1;
        //블록의 가장 밑부분의 y좌표 s에 저장
        for(i=3; i>=0; i--){
                for(j=0; j<4; j++){
                        if(block[blockID][blockRotate][i][j]){
                                s=i;
                                break;
                        }
                }
                if(s) break;     
        }
        //블록이 더이상 내려갈 수 없는 마지막 y좌표를 line에 저장
        for(j=0; j<4; j++){
                if(block[blockID][blockRotate][s][j]==0) continue;
                for(i=s; i<HEIGHT ; i++){
                        if(field[i][j]){
                                if(i<line) line=i;
                                if(empty) empty=0;
                                break;
                        }
                }
        }
        //field에 맞춰 그림자 그림
        if(empty) DrawBlock(HEIGHT-1-s, x, blockID, blockRotate, '/'); 
        else DrawBlock(line-1-s, x, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
        //블록과 그림자를 그림
        DrawBlock(y, x, blockID, blockRotate, ' ');
        //DrawShadow(y, x, blockID, blockRotate);
}

void createRankList(){
	
        FILE *fp;
        int i;

        fp = fopen("rank.txt","r");

        //score_number의 수만큼 노드 생성, 정보 저장
        if (fscanf(fp, "%d", &score_number) != EOF) {
                if(score_number==0) Head=NULL;
                else{ 
                        Node pNew=(Node)malloc(sizeof(struct node));
                        fscanf(fp, "%s %d", pNew->name, &(pNew->score));
                        Head=pNew;
                        Node ptr=Head;
                        for(i=1;i<score_number;i++){
                                Node pNew = (Node)malloc(sizeof(struct node));
                                fscanf(fp, "%s %d", pNew->name, &(pNew->score));
                                ptr->link=pNew;
                                ptr=ptr->link;
                        }
                        ptr->link=NULL;
                }
        }
        else{
                Head = NULL;
        }       

        fclose(fp);

}

void rank(){

        int X=1, Y=score_number, ch;
        clear();

        printw("1. list ranks from X to Y\n");
        printw("2. list ranks by a specific name\n");
        printw("3. delete a specific rank\n");

        ch=wgetch(stdscr);

        if(ch == '1') {
                echo();
                printw("X: "); scanw("%d", &X);
                printw("Y: "); scanw("%d", &Y);
                if(Y>score_number) Y=score_number;
                noecho();
                printw("        name         |   score\n");
                printw("-------------------------------------\n");

                if(X>Y) printw("search failure: no rank in the list\n");
                else{
                        count=0;
                        count++;
                        Node ptr=Head;
                        while(ptr){
                                if(count>=X && count <=Y) {
                                        printw(" %-19s | %-9d\n", ptr->name, ptr->score);
                                }
                                else if(count>Y) break;
                                ptr=ptr->link;
                                count++;
                        }
                }
        }

        else if (ch == '2') {
                char str[NAMELEN+1];
                int check=0;

                echo();
                printw("input the name: "); scanw("%s", str);
                noecho();

                printw("        name         |   score\n");
                printw("-------------------------------------\n");

                Node ptr=Head;
                while(ptr){
                        //입력으로 들어온 이름과 같은 이름이 있을 경우 출력
                        if(strcmp(str,ptr->name)==0){
                                if(!check) check=1;
                                printw(" %-19s | %-9d\n", ptr->name, ptr->score);
                        }
                        ptr=ptr->link;
                }

                //같은 이름이 없을 경우 에러 메세지 출력
                if(check==0) printw("search failure: no name in the list\n");
        }

        else if (ch == '3') {
                int num;

                echo();
                printw("input the rank: "); scanw("%d", &num);
                noecho();
                printw("\n");

                //랭킹이 범위 안에 없으면 에러 메세지 출력
                if(num<1 || num>score_number){
                        printw("search failure: the rank not in the list\n");
                }
                else{
                        count=0;
                        count++;
                        Node ptr=Head, pPrev=Head;
                        //랭킹을 찾아 list에서 삭제
                        while(ptr){
                                if(count==num){
                                        if(count==1) Head=ptr->link;
                                        else pPrev->link=ptr->link;
                                        free(ptr);
                                        printw("result: the rank deleted\n");
                                        break;
                                }
                                count++;
                                pPrev=ptr;
                                ptr=ptr->link;
                        }
                        //rank.txt 갱신
                        writeRankFile();
                }
        }
        getch();

}

void writeRankFile(){
	int sn, score_newnum=0;

        FILE *fp;

        sn=score_number;
        Node ptr=Head;
        while(ptr){
                score_newnum++;
                ptr=ptr->link;
        }
        
        //기존 파일의 랭킹 수와 같으면 종료
        if (sn == score_newnum) return;
        else{
                fp = fopen("rank.txt", "w");
                fprintf(fp, "%d\n", score_newnum);
                ptr=Head;
                while(ptr){
                        fprintf(fp, "%s %d\n", ptr->name, ptr->score);
                        ptr=ptr->link;
                }
                fclose(fp);
        }

        score_number=score_newnum;

        //노드 할당 해제
        
        while(ptr){
                ptr=Head;
                Head=Head->link;
                free(ptr);
        }
        
}

void newRank(int score){
	
        char str[NAMELEN+1];
        int last=1;
        clear();

        Node pNew = (Node)malloc(sizeof(struct node));
        pNew->score=score;

        printw("your name: ");
        echo();
        scanw("%s", pNew->name);
        noecho();

        if(Head==NULL){
                pNew->link=Head;
                Head=pNew;
        }
        else{
                Node ptr=Head;
                //첫 번째 노드보다 점수가 높을 경우
                if(score > ptr->score){
                        pNew->link=ptr;
                        Head=pNew;
                }
                else{
                        while(ptr->link){
                        //ptr의 다음 노드보다 점수가 높을 경우
                                if(score > ptr->link->score){
                                        pNew->link=ptr->link;
                                        ptr->link=pNew;
                                        last=0; break;
                                }
                                ptr=ptr->link;
                        }
                        //가장 점수가 낮을 경우
                        if(last){
                                ptr->link=pNew;
                                pNew->link=NULL;
                        }
                }
        }
        writeRankFile();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
