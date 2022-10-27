#include<stdio.h>
#include<stdbool.h>
#include<math.h>

//getopt使用
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
extern char *ptarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;
int getopt(int argc,char *const *argv,const char *optstring);

#define R(piece) "\033[31m"#piece"\033[0m"
#define B(piece) "\033[34m"#piece"\033[0m"
#define ROW 9
#define COLUMN 9
#define STACKSIZE 500

FILE *fptr;

//棋盤
char* chess_board[ROW][COLUMN];

typedef struct record{
	int x1,y1;//原始位置
	int x2,y2;//後來位置
	char* p0;//原始位置上的棋子
	char* p1;//後來位置上的棋子
}Record;

//紀錄每一手資料（至多500手）
Record stack[STACKSIZE];
//最上,-1為空
int top=-1;
//暫存
Record * temp;
//是否為空
int Empty();
//拿出資料
void* pop();
//放入資料
void push(int x1,int y1,int x2,int y2,char* p0,char* p1);
//和局
void peace();

//遊戲是否結束,0為結束
bool gameOverSign =1;
//是否符合規則,0為符合,1為違反必須重新選擇移動棋子與位置
bool restart =0;
//是否符合遊戲規則 ,初始值為1(符合) 
bool isStandard=1; 
//初始棋盤
void InitGame();
//生成棋盤
void PrintChess();
//要移動的棋子位置
int xi,yi;
//要移動到的位置
int xj,yj;
//判斷現在輪到誰
int turn =-1;
//藍棋移動
void bluemove();
//紅棋移動
void redmove();
//判斷遊戲結束
void isGameOver();
//藍棋規則判斷
void blue_rule();
//紅棋規則判斷
void red_rule();
/*//載入棋譜
void Load;*/
//開新局為0,載入棋譜為1  
int NeworOld;
//getopt使用
int ch;
char Ss;





//主程式 
int main(int argc,char *argv[]){
	//getopt
	opterr=0;
	while((ch=getopt(argc,argv,"ns:l:"))!=-1){
		switch(ch){
			case 'n':
				//printf("-n被印出");
				NeworOld=0;
				break;

			case 's':
				//printf("-s被印出 %s",optarg);
				if((fptr=fopen(optarg,"w"))==NULL){
					printf("File can not be opened.\n");
					return 0;
				}
				NeworOld=0;
				break;
			case 'l':
				//printf("-l被印出 %s",optarg);
				if((fptr=fopen(optarg,"r"))==NULL){
					printf("File can not be opened.\n");
					return 0;
				}
				NeworOld =1;
				break;
		}
	}
	//開始下棋
	//新棋局
	if(NeworOld==0){
		//初始化棋盤
    	InitGame();
		//印製棋盤
    	PrintChess();
		//開始下棋
		turn =-1;
		while(gameOverSign){
			isStandard =1;
			turn *=-1;//雙方交替下棋
			switch(turn){
				case 1:
					bluemove();
					turn=(restart)?(turn*-1):turn;
					break;
				case -1:
					redmove();
					turn=(restart)?(turn*-1):turn;
					break;
			}
			isGameOver();
		}
		printf("請輸入s或S儲存此局");
		scanf("%c",&Ss);
		if(Ss=='s'||Ss=='S'){
			if((fptr=fopen("Review.txt","w"))==NULL){
				printf("已有此檔案名\n");
			}
			else{
				for(top==0;top<500;top++){
					fprintf(fptr,"%d %d %d %d",stack[top].x1,stack[top].y1,stack[top].x2,stack[top].y2);
				}
				fclose(fptr);
			}
		}
		else{
			return 0;
		}
	}
	else if(NeworOld==1){
		if((fptr=fopen("Review.txt","r"))==NULL){
			printf("File can not be opened.\n");
		}
		else{
			//從這裡寫沒電了Q
			InitGame();
			PrintChess();
			int data=top;//暫存棋盤筆數
			top=0;
			char order;//輸入字元
			while(top<=data){
				printf("請輸入移至上一手或下一手(b為退回上一手,f為移至下一手):\n");
				scanf("%c",&order);
				if(order=='b'){
					if(top==0){
						printf("此已為第一筆資料\n");
						break;
					}
					else{
						top--;
						fscanf(fptr,"%d %d %d %d",&(stack[top].x1),&(stack[top].y1),&(stack[top].x2),&(stack[top].y2));
						PrintChess();
					}

				}
				else if(order=='f'){
					if(top==data){
						printf("此已為最後一筆資料\n");
						break;
					}
					else{
						top++;
						fscanf(fptr,"%d %d %d %d",&(stack[top].x1),&(stack[top].y1),&(stack[top].x2),&(stack[top].y2));
						PrintChess();
					}

				}
			}

		}
		fclose(fptr);
	return 0;
	}
}


//初始化棋盤
void InitGame(){
	int i,j; 
	chess_board[0][0]=chess_board[0][8]=R(香);
	chess_board[0][1]=chess_board[0][7]=R(桂);
	chess_board[0][2]=chess_board[0][6]=R(銀); 
	chess_board[0][3]=chess_board[0][5]=R(金);
	chess_board[0][4]=R(玉);
	chess_board[1][0]="  ";
	chess_board[1][1]=R(飛);
	for(j=2;j<7;j++){
		chess_board[1][j]="  ";
	}
	chess_board[1][7]=R(角);
	chess_board[1][8]="  ";
	
	for(j=0;j<COLUMN;j++){
		chess_board[2][j]=R(步);
	}
	for(i=3;i<ROW;i++){
		for(j=0;j<COLUMN;j++){
			chess_board[i][j]="  ";
		}
	}
	for(j=0;j<COLUMN;j++){
		chess_board[6][j]=B(步);
	}
		chess_board[7][0]="  ";
		chess_board[7][1]=B(角);
	for(j=2;j<7;j++){
		chess_board[7][j]="  ";
	}
	chess_board[7][7]=B(飛);
	chess_board[7][8]="  ";
	chess_board[8][0]=chess_board[8][8]=B(香);
	chess_board[8][1]=chess_board[8][7]=B(桂);
	chess_board[8][2]=chess_board[8][6]=B(銀); 
	chess_board[8][3]=chess_board[8][5]=B(金);
	chess_board[8][4]=B(玉);	
} 


//印製棋盤
void PrintChess(){
	printf("  ９   ８   ７   ６   ５   ４   ３   ２   １");
    printf("\n");
    int i;
    for(i=0;i<ROW;i++){
        printf("|");
        for(int j=0;j<COLUMN;j++){
            printf(" %s |",chess_board[i][j]);
        }
        printf("%d",i+1);
        printf("\n");
        if(i<ROW-1)
            printf("|----|----|----|----|----|----|----|----|----|\n");
    }	
}

//和局
void peace(){
	gameOverSign =0;
	printf("和局\n");
}

//判斷是否為空堆疊
int Empty(){
	if(top==-1){
		return 1;
	}
	else{
		return 0;
	}
}
//拿出資料
void* pop(){
	void* data;
	if(Empty()==0){
		data=&(stack[top]);
		top --;
		return data;
	}
}

//放入資料
void push(int x1,int y1,int x2,int y2,char* p0,char* p1){
	top++;
	stack[top].x1=xi;
	stack[top].x2=xj;
	stack[top].y1=yi;
	stack[top].y2=yj;
	stack[top].p0=chess_board[xi][yi];
	stack[top].p1=chess_board[xj][yj];
	if(top==STACKSIZE-1){
		peace();
	}
}
//藍棋移動
void bluemove(){
	int i;
	if(restart){
		printf("違反遊戲規則，請重新輸入\n");
		restart=0;
	}
	printf("請輸入要移動的藍棋之xi(0為悔棋):");
	scanf("%d",&xi);
	if(xi==0){
		if(Empty()==1){
			printf("已為初始狀態，不可悔棋\n");
			turn *=-1;
		}
		else{
			printf("是否真的悔棋？(0:真的,其他:否)\n");
			scanf("%d",&i);
			if(i==0){
				temp=pop();
				chess_board[temp->x1][temp->y1]=temp->p0;
				chess_board[temp->x1][temp->y1]=temp->p0;
				PrintChess();
			}
			else{
				turn *=-1;
			}
		}
	}
	else{
		printf("請輸入要移動的藍棋之yi:");
		scanf("%d",&yi);
		printf("請輸入要放置的位置(xj,yj):");
		scanf("%d %d",&xj,&yj);
		xi=xi-1;
		yi=9-yi;
		xj=xj-1;
		yj=9-yj;
		blue_rule();
		PrintChess();
	}
}

//紅棋移動
void redmove(){
	int i;
	if(restart){
		printf("違反遊戲規則，請重新輸入\n");
		restart=0;
	}
	printf("請輸入要移動的紅棋之xi(0為悔棋):");
	scanf("%d",&xi);
	if(xi==0){
		if(Empty()==1){
			printf("已為初始狀態，不可悔棋\n");
			turn *=-1;
		}
		else{
			printf("是否真的悔棋？(0:真的,其他:否)\n");
			scanf("%d",&i);
			if(i==0){
				temp=pop();
				chess_board[temp->x1][temp->y1]=temp->p0;
				chess_board[temp->x1][temp->y1]=temp->p0;
				PrintChess();
			}
			else{
				turn *=-1;
			}
		}
	}
	else{
		printf("請輸入要移動的紅棋之yi:");
		scanf("%d",&yi);
		printf("請輸入要放置的位置(xj,yj):");
		scanf("%d %d",&xj,&yj);
		xi=xi-1;
		yi=9-yi;
		xj=xj-1;
		yj=9-yj;
		red_rule();
		PrintChess();
	}
}


//判斷遊戲結束
void isGameOver(){
	bool sign_r =0;
	bool sign_b =0;
	for(int i=0;i<9;i++){
		for(int j=0;j<9;j++){
			if(chess_board[i][j]==B(玉)){
				sign_b=1;
			}
			else if(chess_board[i][j]==R(玉)){
				sign_r=1;
			}
		}
	}
	if((sign_b==0)||(sign_r==0)){
			gameOverSign =0;
			if((sign_b==0)&&(sign_r==1)){
				printf("紅棋玩家獲勝\n");
			}
			if((sign_b==1)&&(sign_r==0)){
				printf("藍棋玩家獲勝\n");
			}
		}
}

//藍棋規則判斷
void blue_rule(){
	if(chess_board[xi][yi]==R(步)||chess_board[xi][yi]==R(香)||chess_board[xi][yi]==R(桂)||chess_board[xi][yi]==R(銀)||chess_board[xi][yi]==R(金)||chess_board[xi][yi]==R(玉)||chess_board[xi][yi]==R(角)||chess_board[xi][yi]==R(飛)||chess_board[xi][yi]=="  "){
		restart=1;
	}
	else{
		//B(步)
		if(chess_board[xi][yi]==B(步)){
			if(xj==xi-1&&yj==yi){
				if(chess_board[xj][yj]==B(步)||chess_board[xj][yj]==B(香)||chess_board[xj][yj]==B(桂)||chess_board[xj][yj]==B(銀)||chess_board[xj][yj]==B(金)||chess_board[xj][yj]==B(玉)||chess_board[xj][yj]==B(角)||chess_board[xj][yj]==B(飛)){
					restart=1;
				}
				else{
					chess_board[xi][yi]="  ";
					chess_board[xj][yj]=B(步);
					push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
				}
			}
			else{
				restart=1;
			}	
		}
		//B(香)
		if(chess_board[xi][yi]==B(香)){
			if(yi!=yj){
				restart=1;
			}
			else{
				for(int i=xi+1;i<xj;i++){
					if(chess_board[i][yi]!="  "){
						isStandard =0;
					}
				} 
				for(int i=xi-1;i>xj;i--){
					if(chess_board[i][yi]!="  "){
						isStandard =0;
					}
				} 
				if(chess_board[xj][yi]==B(步)||chess_board[xj][yi]==B(香)||chess_board[xj][yi]==B(桂)||chess_board[xj][yi]==B(銀)||chess_board[xj][yi]==B(金)||chess_board[xj][yi]==B(玉)||chess_board[xj][yi]==B(角)||chess_board[xj][yi]==B(飛)){
					isStandard =0;
				}  
				if(isStandard ==1){
					chess_board[xi][yi]="  ";
					chess_board[xj][yj]=B(香);
					push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
				}
				else{
					restart =1;
				}
			}
		}
		//B(桂)
		if(chess_board[xi][yi]==R(桂)){
			if((xj==xi-2&&yj==yi-1)||(xj==xi-2&&yj==yi+1)){
				if(chess_board[xj][yi]==B(步)||chess_board[xj][yi]==B(香)||chess_board[xj][yi]==B(桂)||chess_board[xj][yi]==B(銀)||chess_board[xj][yi]==B(金)||chess_board[xj][yi]==B(玉)||chess_board[xj][yi]==B(角)||chess_board[xj][yi]==B(飛)){
					restart=1;
				}
				else{
					chess_board[xi][yi]="  ";
					chess_board[xj][yj]=B(桂);
					push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
				}
			}
			else{
				restart=1;
			}
		}
	
		//B(銀)
		if(chess_board[xi][yi]==B(銀)){
			if((xj==xi-1&&yj==yi)||(xj==xi-1&&yj==yi-1)||(xj==xi-1&&yj==yi+1)||(xj==xi+1&&yj==yi-1)||(xj==xi+1&&yj==yi+1)){
				if(chess_board[xj][yj]==B(步)||chess_board[xj][yj]==B(香)||chess_board[xj][yj]==B(桂)||chess_board[xj][yj]==B(銀)||chess_board[xj][yj]==B(金)||chess_board[xj][yj]==B(玉)||chess_board[xj][yj]==B(角)||chess_board[xj][yj]==B(飛)){
					restart=1;
				}
				else{
					chess_board[xi][yi]="  ";
					chess_board[xj][yj]=B(銀);
					push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
				}
			}
			else{
				restart=1;
			}
		}
		//B(金)
		if(chess_board[xi][yi]==B(金)){
			if((xj==xi-1&&yj==yi)||(xj==xi-1&&yj==yi-1)||(xj==xi-1&&yj==yi+1)||(xj==xi&&yj==yi-1)||(xj==xi&&yj==yi+1)||(xj==xi+1&&yj==yi)){
				if(chess_board[xj][yj]==B(步)||chess_board[xj][yj]==B(香)||chess_board[xj][yj]==B(桂)||chess_board[xj][yj]==B(銀)||chess_board[xj][yj]==B(金)||chess_board[xj][yj]==B(玉)||chess_board[xj][yj]==B(角)||chess_board[xj][yj]==B(飛)){
					restart=1;
				}
				else{
					chess_board[xi][yi]="  ";
					chess_board[xj][yj]=B(金);
					push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
				}
			}
			else{
				restart=1;
			}
		}
		//B(玉)	
		if(chess_board[xi][yi]==B(玉)){
			if((xj==xi+1&&yj==yi)||(xj==xi+1&&yj==yi-1)||(xj==xi+1&&yj==yi+1)||(xj==xi-1&&yj==yi-1)||(xj==xi-1&&yj==yi+1)||(xj==xi&&yj==yi-1)||(xj==xi&&yj==yi+1)||(xj==xi-1&&yj==yi)){
				if(chess_board[xj][yj]==B(步)||chess_board[xj][yj]==B(香)||chess_board[xj][yj]==B(桂)||chess_board[xj][yj]==B(銀)||chess_board[xj][yj]==B(金)||chess_board[xj][yj]==B(角)||chess_board[xj][yj]==B(飛)){				
					restart=1;
				}
				else{
					chess_board[xi][yi]="  ";
					chess_board[xj][yj]=B(玉);
					push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
				}
			}
			else{
				restart=1;
			}
		}
		//B(角)
		if(chess_board[xi][yi]==B(角)){
			if(abs(yj-yi)==0){
				restart =1;
			}
			else{
				if(abs(xj-xi)/abs(yj-yi)==1){
					int i,j;
					if(yi>yi){
						for(i=xi-1,j=yi+1;i>xj;i--,j++){
							if(chess_board[i][j]!="  "){
								isStandard =0;
							}
						}
						for(i=xi+1,j=yi+1;i<xj;i++,j++){
							if(chess_board[i][j]!="  "){
								isStandard =0;
							}
						}
					}
					else if(yj<yi){
						for(i=xi-1,j=yi+1;i>xj;i--,j--){
							if(chess_board[i][j]!="  "){
								isStandard =0;
							}
						}
						for(i=xi+1,j=yi+1;i<xj;i++,j--){
							if(chess_board[i][j]!="  "){
								isStandard =0;
							}
						}
					}
					if(chess_board[xj][yj]==B(步)||chess_board[xj][yj]==B(香)||chess_board[xj][yj]==B(桂)||chess_board[xj][yj]==B(銀)||chess_board[xj][yj]==B(金)||chess_board[xj][yj]==B(玉)||chess_board[xj][yj]==B(飛)){
						isStandard =0;
					}
					if(isStandard==1){
						chess_board[xi][yi]="  ";
						chess_board[xj][yj]=B(角);
						push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
					}
					else{
						restart=1;
					}
				}
				else{
					restart=1;
				}
			}
		}
		//B(飛)
		if(chess_board[xi][yi]==B(飛)){
			//列座標不變
			if(yi==yj){
				for(int i=xi+1;i<xj;i++){
					if(chess_board[i][yi]!="  "){
						isStandard =0;
					}
				} 
				for(int i=xi-1;i>xj;i--){
					if(chess_board[i][yi]!="  "){
						isStandard =0;
					}
				} 
				if(chess_board[xj][yi]==B(步)||chess_board[xj][yi]==B(香)||chess_board[xj][yi]==B(桂)||chess_board[xj][yi]==B(銀)||chess_board[xj][yi]==B(金)||chess_board[xj][yi]==B(玉)||chess_board[xj][yi]==B(角)){
					isStandard =0;
				}  
			}
			//行座標不變
			else if(xi==xj){
				for(int i=yi+1;i<yj;i++){
					if(chess_board[xi][i]!="  "){
						isStandard =0;
					}
				} 
				for(int i=yi-1;i>yj;i--){
					if(chess_board[xi][i]!="  "){
						isStandard =0;
					}
				} 
				if(chess_board[xi][yj]==B(步)||chess_board[xi][yj]==B(香)||chess_board[xi][yj]==B(桂)||chess_board[xi][yj]==B(銀)||chess_board[xi][yj]==B(金)||chess_board[xi][yj]==B(玉)||chess_board[xi][yj]==B(角)){
					isStandard =0;
				} 
			}
			if((yi==yj||xi==xj)&&isStandard==1){
				chess_board[xi][yi]="  ";
				chess_board[xj][yj]=B(飛);
				push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
			}
			else{
				restart =1;
			}
		}
	}
}
//紅棋規則判斷
void red_rule(){
	if(chess_board[xi][yi]==B(步)||chess_board[xi][yi]==B(香)||chess_board[xi][yi]==B(桂)||chess_board[xi][yi]==B(銀)||chess_board[xi][yi]==B(金)||chess_board[xi][yi]==B(玉)||chess_board[xi][yi]==B(角)||chess_board[xi][yi]==B(飛)||chess_board[xi][yi]=="  "){
		restart=1;
	}
	else{
		//R(步)
		if(chess_board[xi][yi]==R(步)){
			if(xj==xi+1&&yj==yi){
				if(chess_board[xj][yj]==R(步)||chess_board[xj][yj]==R(香)||chess_board[xj][yj]==R(桂)||chess_board[xj][yj]==R(銀)||chess_board[xj][yj]==R(金)||chess_board[xj][yj]==R(玉)||chess_board[xj][yj]==R(角)||chess_board[xj][yj]==R(飛)){
					restart=1;
				}
				else{
					chess_board[xi][yi]="  ";
					chess_board[xj][yj]=R(步);
					push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
				}
			}
			else{
				restart=1;
			}	
		}
		//B(香)
		if(chess_board[xi][yi]==R(香)){
			if(yi!=yj){
				restart=1;
			}
			else{
				for(int i=xi+1;i<xj;i++){
					if(chess_board[i][yi]!="  "){
						isStandard =0;
					}
				} 
				for(int i=xi-1;i>xj;i--){
					if(chess_board[i][yi]!="  "){
						isStandard =0;
					}
				} 
				if(chess_board[xj][yi]==R(步)||chess_board[xj][yi]==R(香)||chess_board[xj][yi]==R(桂)||chess_board[xj][yi]==R(銀)||chess_board[xj][yi]==R(金)||chess_board[xj][yi]==R(玉)||chess_board[xj][yi]==R(角)||chess_board[xj][yi]==R(飛)){
					isStandard =0;
				}  
				if(isStandard ==1){
					chess_board[xi][yi]="  ";
					chess_board[xj][yj]=R(香);
					push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
				}
				else{
					restart =1;
				}
			}
		}
		//R(桂)
		if(chess_board[xi][yi]==R(桂)){
			if((xj==xi+2&&yj==yi-1)||(xj==xi+2&&yj==yi+1)){
				if(chess_board[xj][yi]==R(步)||chess_board[xj][yi]==R(香)||chess_board[xj][yi]==R(桂)||chess_board[xj][yi]==R(銀)||chess_board[xj][yi]==R(金)||chess_board[xj][yi]==R(玉)||chess_board[xj][yi]==R(角)||chess_board[xj][yi]==R(飛)){
					restart=1;
				}
				else{
					chess_board[xi][yi]="  ";
					chess_board[xj][yj]=R(桂);
					push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
				}
			}
			else{
				restart=1;
			}
		}
	
		//R(銀)
		if(chess_board[xi][yi]==R(銀)){
			if((xj==xi+1&&yj==yi)||(xj==xi+1&&yj==yi-1)||(xj==xi+1&&yj==yi+1)||(xj==xi-1&&yj==yi-1)||(xj==xi-1&&yj==yi+1)){
				if(chess_board[xj][yj]==R(步)||chess_board[xj][yj]==R(香)||chess_board[xj][yj]==R(桂)||chess_board[xj][yj]==R(銀)||chess_board[xj][yj]==R(金)||chess_board[xj][yj]==R(玉)||chess_board[xj][yj]==R(角)||chess_board[xj][yj]==R(飛)){
					restart=1;
				}
				else{
					chess_board[xi][yi]="  ";
					chess_board[xj][yj]=R(銀);
					push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
				}
			}
			else{
				restart=1;
			}
		}
		//R(金)
		if(chess_board[xi][yi]==R(金)){
			if((xj==xi+1&&yj==yi)||(xj==xi+1&&yj==yi-1)||(xj==xi+1&&yj==yi+1)||(xj==xi&&yj==yi-1)||(xj==xi&&yj==yi+1)||(xj==xi-1&&yj==yi)){
				if(chess_board[xj][yj]==R(步)||chess_board[xj][yj]==R(香)||chess_board[xj][yj]==R(桂)||chess_board[xj][yj]==R(銀)||chess_board[xj][yj]==R(金)||chess_board[xj][yj]==R(玉)||chess_board[xj][yj]==R(角)||chess_board[xj][yj]==R(飛)){
					restart=1;
				}
				else{
					chess_board[xi][yi]="  ";
					chess_board[xj][yj]=R(金);
					push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
				}
			}
			else{
				restart=1;
			}
		}
		//R(玉)	
		if(chess_board[xi][yi]==R(玉)){
			if((xj==xi+1&&yj==yi)||(xj==xi+1&&yj==yi-1)||(xj==xi+1&&yj==yi+1)||(xj==xi-1&&yj==yi-1)||(xj==xi-1&&yj==yi+1)||(xj==xi&&yj==yi-1)||(xj==xi&&yj==yi+1)||(xj==xi-1&&yj==yi)){
				if(chess_board[xj][yj]==R(步)||chess_board[xj][yj]==R(香)||chess_board[xj][yj]==R(桂)||chess_board[xj][yj]==R(銀)||chess_board[xj][yj]==R(金)||chess_board[xj][yj]==R(角)||chess_board[xj][yj]==R(飛)){				
					restart=1;
				}
				else{
					chess_board[xi][yi]="  ";
					chess_board[xj][yj]=R(玉);
					push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
				}
			}
			else{
				restart=1;
			}
		}
		//R(角)
		if(chess_board[xi][yi]==R(角)){
			if(abs(yj-yi)==0){
				restart =1;
			}
			else{
				if(abs(xj-xi)/abs(yj-yi)==1){
					int i,j;
					if(yi>yi){
						for(i=xi-1,j=yi+1;i>xj;i--,j++){
							if(chess_board[i][j]!="  "){
								isStandard =0;
							}
						}
						for(i=xi+1,j=yi+1;i<xj;i++,j++){
							if(chess_board[i][j]!="  "){
								isStandard =0;
							}
						}
					}
					else if(yj<yi){
						for(i=xi-1,j=yi+1;i>xj;i--,j--){
							if(chess_board[i][j]!="  "){
								isStandard =0;
							}
						}
						for(i=xi+1,j=yi+1;i<xj;i++,j--){
							if(chess_board[i][j]!="  "){
								isStandard =0;
							}
						}
					}
					if(chess_board[xj][yj]==R(步)||chess_board[xj][yj]==R(香)||chess_board[xj][yj]==R(桂)||chess_board[xj][yj]==R(銀)||chess_board[xj][yj]==R(金)||chess_board[xj][yj]==R(玉)||chess_board[xj][yj]==R(飛)){
						isStandard =0;
					}
					if(isStandard==1){
						chess_board[xi][yi]="  ";
						chess_board[xj][yj]=R(角);
						push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
					}
					else{
						restart=1;
					}
				}
				else{
					restart=1;
				}
			}
		}
		//R(飛)
		if(chess_board[xi][yi]==R(飛)){
			//列座標不變
			if(yi==yj){
				for(int i=xi+1;i<xj;i++){
					if(chess_board[i][yi]!="  "){
						isStandard =0;
					}
				} 
				for(int i=xi-1;i>xj;i--){
					if(chess_board[i][yi]!="  "){
						isStandard =0;
					}
				} 
				if(chess_board[xj][yi]==R(步)||chess_board[xj][yi]==R(香)||chess_board[xj][yi]==R(桂)||chess_board[xj][yi]==R(銀)||chess_board[xj][yi]==R(金)||chess_board[xj][yi]==R(玉)||chess_board[xj][yi]==R(角)){
					isStandard =0;
				}  
			}
			//行座標不變
			else if(xi==xj){
				for(int i=yi+1;i<yj;i++){
					if(chess_board[xi][i]!="  "){
						isStandard =0;
					}
				} 
				for(int i=yi-1;i>yj;i--){
					if(chess_board[xi][i]!="  "){
						isStandard =0;
					}
				} 
				if(chess_board[xi][yj]==R(步)||chess_board[xi][yj]==R(香)||chess_board[xi][yj]==R(桂)||chess_board[xi][yj]==R(銀)||chess_board[xi][yj]==R(金)||chess_board[xi][yj]==R(玉)||chess_board[xi][yj]==R(角)){
					isStandard =0;
				} 
			}
			if((yi==yj||xi==xj)&&isStandard==1){
				chess_board[xi][yi]="  ";
				chess_board[xj][yj]=R(飛);
				push(xi,yi,xj,yj,chess_board[xi][yi],chess_board[xj][yj]);
			}
			else{
				restart =1;
			}
		}
	}
}
