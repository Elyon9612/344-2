//Honghao Li
//933654887

#include<stdio.h>
#include<string.h>
#include<time.h>
#include<fcntl.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

//Get the Global PID and connection relationship
//For using in every functions

int PID = 0;
int links[7][7];				//This for build the connection for each files and if can access it will be 1 otherwise be 0
char filesname[7][9];				//For store each room name into this 2-D character array
char totalname[10][9]={"RoomA",			//Creating a 10 Room names
	"RoomB",
	"RoomC",
	"RoomD",
	"RoomE",
	"RoomF",
	"RoomG",
	"RoomH",
	"RoomI",
	"RoomJ",};


//This function is for creating seven files randomly from given ten files
void initial_file(){
	int i,j;
	int total=10;
	for(i=0;i<7;i++){
		int num = rand()%total;		//Get random number
		//printf("num is: %d\n",num);
		strcpy(filesname[i],totalname[num]);
		//printf("name i is: %s\n",&filesname[i]);
		for(j=num;j<9;j++){
			strcpy(totalname[num],totalname[num+1]);
			num++;
		}
		total--;
	}
}
//This function is for checking if Room has full of connection rooms
bool IsRoomFull(int* Room,int i,int n){
	if(Room[i] >= n){
		return true;
	}
	return false;
}

//This function is for creating directory named with PID and files into the directory
void initial_directory(char* dir){
	ssize_t nwritten;
	PID=getpid();				//This is for getting the random PID from the system
	sprintf(dir,"lihongh.room.%d",PID);
	int create_dir = mkdir(dir,0777);	//Creating the directory

	int i=0;
	for(i=0;i<7;i++){
		char tmp[40];
		sprintf(tmp,"lihongh.room.%d/%s",PID,filesname[i]);
		//printf("tmp is: %s\n",tmp);
		int file_descriptor=0;
		file_descriptor = open(tmp,O_RDWR | O_CREAT | O_TRUNC, 0600);			//Some of code get from the lecture ppt
		if (file_descriptor == -1){
			printf("Create file failed on \"%s\"\n",tmp);
			perror("In initial_directory()");
			exit(1);
		}
		//printf("i is: %d\n",i);
		close(file_descriptor);
	}
}

//This function is for adding the connection between room a and room b
void Build_Link(int* Room,int a,int b){
	links[a][b] = 1;
	links[b][a] = 1;
	Room[a]++;
	Room[b]++;
}

//This is for getting random number for type and its connection rooms
void AddRandomConnection(int* Room){
	int i=0,num=0,j=0;
	for(i;i<7;i++){
		num = 3 + rand()%4;
		//printf("num is: %d\n",num);
		while(IsRoomFull(Room,i,num)==false){
			int r = rand()%7;
			if( r != i && links[i][r] != 1){
				//printf("Build %d with: %d\n",i,r);
				Build_Link(Room,r,i);
			}
		}
		//printf("The Room %d is done!\n",i);
	}
}

//This function is for writing name, connection and type into each room files
void AddContent(int* Room){
	int i=0,j=0;
	FILE *f = NULL;
	for(i=0;i<7;i++){
		int n=1;
		char tmp[40];
		sprintf(tmp,"lihongh.room.%d/%s",PID,filesname[i]);
		f = fopen(tmp,"w+");
		fprintf(f,"ROOM NAME: %s\n",filesname[i]);
		for(j=0;j<7;j++){
			//printf("Room~~~~~~~~~~~ is: %d\n",links[i][j]);
			//printf("i is %d and j is %d\n",i,j);
			if(links[i][j] == 1){
				fprintf(f,"CONNECTION %d: %s\n",n,filesname[j]);
				n++;
			}
		}
		if(i == 0){
			fprintf(f,"ROOM TYPE: START_ROOM\n");
		}else if(i == 2){
			fprintf(f,"ROOM TYPE: END_ROOM\n");
		}else{
			fprintf(f,"ROOM TYPE: MID_ROOM\n");
		}
		fclose(f);
	}
}

int main(){
	int Room[7];
	char dir[30];		//This for storing the directory's name each time
	srand(time(NULL));

	memset(Room,0,7*sizeof(Room[0]));
	memset(dir,'\0',30*sizeof(dir[30]));

	initial_file();
	initial_directory(dir);	
	
	AddRandomConnection(Room);

	AddContent(Room);
}



























