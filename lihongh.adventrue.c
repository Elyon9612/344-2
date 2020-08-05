#include<stdio.h>
#include<string.h>
#include<time.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>

//Setting the myMutex lock in the global
pthread_mutex_t myMutex;


//This struct is for storing each room's information
struct rooms{
	char name[20];
	int num;
	char connection[7][20];	
	char** connections;		//I didn't use this dynamic array at all in this assignment
	int type;			// type 1 for start and  2 for end and 0 for mid
};


//This function is for find the lastest directory
void locate_dir(char* dir_path){
	int time = -1;
	struct stat sb;

	DIR *dr = opendir(".");
	struct dirent *d;
	if(dr == NULL){
		printf("Could not open current directory");
		exit(1);
	}
	while((d=readdir(dr)) != NULL){
		stat(d->d_name,&sb);
		if(S_ISDIR(sb.st_mode) == 1 && strcmp(d->d_name,".") != 0){
			if((int)sb.st_mtime > time){
				time = sb.st_mtime;
				strcpy(dir_path,d->d_name);
			}
			//printf("Running and path is: %s \n",dir_path);
		}
	}
	closedir(dr);
}

//This function is for store every information of files into struct array
void storage(char* dir_path,struct rooms* r){
	int count = 0;
	char tmp[20];
	char txt[30];
	struct dirent *d;
	FILE *f = NULL;

	DIR *dr = opendir(dir_path);
	if(dr == NULL){
		printf("Could not open current directory");
		exit(1);
	}

	while((d=readdir(dr)) != NULL){
		if(strcmp(d->d_name,".") == 0 || strcmp(d->d_name,"..") == 0){
			continue;
		}
		int numbers = 0;
		sprintf(tmp,"%s/%s",dir_path,d->d_name);
		//printf("current is: %s\n",tmp);
		f = fopen(tmp,"r");
		if(f == NULL){
			printf("Can not read the file!\n");
			continue;
		}
		memset(txt,'\0',sizeof(txt));
		while(fgets(txt,25,f) != NULL){			//Read each line for each file
			char text[50];
			strcpy(text,txt);
			//printf("txt is:%s",txt);
			if(strcmp(txt,"ROOM TYPE: START_ROOM\n") == 0){
				r[count].type = 1;
			}else if(strcmp(txt,"ROOM TYPE: END_ROOM\n") == 0){
				r[count].type = 2;
			}else if(strcmp(txt,"ROOM TYPE: MID_ROOM\n") == 0){
				r[count].type = 0;
			}else if(txt[0] == 'C'){		//Check if the first char of a line is C of "Connection"
				char substring[20];
				int position = 15,length = 6,c=0;
				while(c<length){
					substring[c] = text[position+c-1];
					c++;
				}
				strcpy(r[count].connection[numbers],substring);
				numbers ++;
			}else{					//Check other condition which should be the "Room name"
				char substring[20];
				int position = 12,length = 5,c=0;
				while(c<length){
					substring[c] = txt[position+c-1];
					c++;
				}
				strcpy(r[count].name,substring);
			}
		}
		//printf("numbers is%d\n",numbers);
		fclose(f);
		r[count].num = numbers;
		count ++;
	}

}

//This function is for getting the start room
void check_start(struct rooms* r,char* start){
	int i;
	for(i=0;i<7;i++){
		if(r[i].type == 1){
			sprintf(start,r[i].name);
		}
	}
}

//This function is for getting the end room
void check_end(struct rooms* r,char* end){
	int i;
	for(i=0;i<7;i++){
		if(r[i].type == 2){
			sprintf(end,r[i].name);
		}
	}
}

//This function is for printing the basic game information and check the current room
int check_current(struct rooms* r,char* current){
	int i,j;
	for(i=0;i<7;i++){
		if(strcmp(r[i].name,current) == 0){
			printf("CURRENT LOCATION: %s\n",current);
			printf("POSSIBLE CONNECTIONS: ");
			for(j=0;j<r[i].num-1;j++){
				char tmp[6];
				memcpy(tmp,r[i].connection[j],5);
				printf("%.*s ",5,tmp);		//This is because sometimes the program will lost a char when printing
			}
			printf("%s",r[i].connection[r[i].num-1]);
			return i;
		}
	}
	return 0;
}

//This function is for checking if the input staifty the connection rooms of the current room
int check_correct(struct rooms* r,int n,char* input){
	printf("Input is:%s\n",input);
	int i,j;
	for(i=0;i<r[n].num;i++){
		if(strcmp(r[n].connection[i],input)== 0){
			return 0;
		}
	}
	return 1;
}

//This is for store each correct step into a templete file and print it when ending the game
void store_step(char* input){
	//Create a file and store into it
	FILE* f = NULL;
	char filename[] = "./History.txt";
	f = fopen(filename,"a+");
	if(f == NULL){
		printf("Can not read the file!\n");
		exit(1);
	}
	fprintf(f,input);
	fclose(f);
}

//This is for printing the step history and close the file
void print_history(){
	char txt[20];
	FILE* f = NULL;
	char filename[] = "./History.txt";
	f = fopen(filename,"r");
	if(f == NULL){
		printf("Can not read the file!\n");
		exit(1);
	}
	while(fgets(txt,20,f) != NULL){
		printf("%s",txt);
	}
	fclose(f);
}

//This function is for read the current time and store it into the file
void update_time(char* path){
	FILE* f = NULL;
	char filename[30];
	time_t t;
	t = time(NULL);
	struct tm* time;
	char buffer[100];
	pthread_mutex_lock(&myMutex);		//clock the file
	time = localtime(&t);
	if(time == NULL){
		perror("local time");
		exit(1);
	}
	strftime(buffer,sizeof(buffer),"%I:%M%p, %A, %B %d, %Y \n",time);
	//printf(" %s\n",buffer);
	//printf(" %s\n",path);
	sprintf(filename,"%s/currenTime.txt",path);
	f = fopen(filename,"w+");
	if(f == NULL){
		printf("Can not wirte the file!\n");
		exit(1);
	}
	fprintf(f,buffer);
	fclose(f);
	pthread_mutex_unlock(&myMutex);		//unclock the file
}

//This function is for read the time from the currentTime file
void read_time(char* path){
	FILE* f = NULL;
	char filename[30];
	char buffer[100];
	sprintf(filename,"%s/currenTime.txt",path);
	pthread_mutex_lock(&myMutex);		//clock the file
	f = fopen(filename,"r");
	if(f == NULL){
		printf("Can not read the file!\n");
		exit(1);
	}
	fgets(buffer,100,f);
	printf(" %s\n",buffer);
	fclose(f);
	pthread_mutex_unlock(&myMutex);		//unclock the file
}


//Run the program in this function and calling most of main functions above of it
void start_game(struct rooms* r,char* path){
	int num_step=0;
	pthread_t tid1;
	pthread_t tid2;
	char* input;
	char buffer[32];
	size_t inputsize = 32;
	input = buffer;

	char current[20],end[20];
	check_start(r,current);
	check_end(r,end);
	while(strcmp(current,end) != 0){			//Checking if the type of current room is "end" which will end the game
		int n = check_current(r,current);
		printf("WHERE TO? >");
		getline(&input,&inputsize,stdin);
		//printf("You typed: %s",input);
		printf("\n");
		if(strcmp(input,"time\n") == 0){		//If user wanna see the time then will call the mult-threads
			if(pthread_mutex_init(&myMutex,NULL)!=0){
				printf("mutex init has failed\n");
				exit(1);
			}
			if(pthread_create(&tid1,NULL,update_time,path) != 0){
				printf("Thread1 can't be created\n");
			}
			if(pthread_create(&tid2,NULL,read_time,path) != 0){
				printf("Thread2 can't be created\n");
			}
			pthread_join(tid1,NULL);
			pthread_join(tid2,NULL);
			
		}else if(check_correct(r,n,input) == 0){	//If the input is statify the connection rooms
			store_step(input);
			num_step ++;
			memcpy(current,input,5);
		}else{						//Bad input of user 
			printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		}
	}
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n",num_step);
	print_history();
	remove("./History.txt");				//Delete the templete file which is for storing the steps

}

int main(){
	int i,j;
	struct rooms* r;
	r = malloc(7 * sizeof(struct rooms*));
	/*for(i=0;i<7;i++){
		r[i].connections = malloc(7 * sizeof(char*));
		for(j=0;j<7;j++){
			r[i].connections[j] = malloc(30 * sizeof(r[i].connections[j]));
			//r[i].connections[j] = malloc(30 * sizeof(char));
		}
	}*/
	char dir_path[50];			//For storing the latest directory
	locate_dir(dir_path);

	storage(dir_path,r);				
	start_game(r,dir_path);

	/*for(i=0;i<7;i++){
		for(j=0;j<7;j++){
			free(r[i].connections[j]);
		}
		free(r[i].connections);
	}*/	
	free(r);
}















