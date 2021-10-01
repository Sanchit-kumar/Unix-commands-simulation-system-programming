//SANCHIT KUMAR
//2020AIM1009
//M.TECH AI 
#include "csapp.h"
#include<sys/utsname.h>		//struct utsname
#include<readline/readline.h> 
#include<readline/history.h> 		//for tracking history of the most recently used command
#include<dirent.h>
#include<sys/utsname.h>
#include<pwd.h>
#include<grp.h>
#include<elf.h>
#define rep(n) for(i=0;i<n;i++)
#define MAXLEN 1000
#define MAXARGV 100
#define MAXFILES 100
// adjusting type w.r.t system for ELF file header
#if defined(__LP64__)
#define Elf_W(type) Elf64_ ## type
#else
#define Elf_W(type) Elf32_ ## type
#endif
int i;
void printShellname(char s[])	//printing the shellpath name
	{
	char str[500];
	char shellpath[500];
	getcwd(str,500);
	strcpy(shellpath,getenv("USER"));
	struct utsname buf1; 
	   if(uname(&buf1)!=0) 
	   { 
	      perror("some error occured..in calling uname"); 
	      exit(EXIT_FAILURE); 
	   } 
	strcat(shellpath,"@");
	strcat(shellpath,buf1.nodename);
	strcat(shellpath,":~");
	int count=0;
	for(i=1;i<strlen(str);i++)
		if(str[i]=='/')
		{
			count++;
			if(count==2)
				break;
		}
	strcat(shellpath,str+i);
	strcat(shellpath,"$ ");	
	strcpy(s,shellpath);
	}
void printhelp()		//Will execute on writing help
	{
	FILE *fp;		//file pointer declaration
	fp=fopen("help.txt","r+");  	//FILE OPENING help.txt
	if(fp==NULL)
		{
			printf("\nSorry...error occured in opening of help.txt");
			return;
		}
	while(!feof(fp))
		{
			char str[MAXLEN];
			fgets(str,MAXLEN,fp);	
			printf("%s",str);
		}
	fclose(fp);		//CLOSING FILE
	}
int readcommand(char *cmd)	//function will read command from the user and also update the history of the most recently used commands
	{	
	char *buffer;
	char str[MAXLEN]; 
	printShellname(str);
	buffer=readline(str); 
	if(strlen(buffer)!=0)
	{ 
		add_history(buffer); //adding to history 
		strcpy(cmd,buffer); 
		return 0; 
	}
	else
		return -1;
	}
void splitargv(int *argc,char *argv[],char *line)	//it will split the arguments
	{
	char *token;
	token=strtok(line," ");
	int i=0;
	argv[i++]=token;
	while(token)
		argv[i++]=token=strtok(NULL," ");
	
	*argc=i-1;
	}
///////  LS SECTION ///////////////
void permission(int i)			//It will print the permission
	{
	if(i&4)
		printf("r");
	else
		printf("-");
	if(i&2)
		printf("w");
	else
		printf("-");
	if(i&1)
		printf("x");
	else
		printf("-");
	}
void print_permission(mode_t m)
	{
		if (S_ISDIR(m))
			printf("\nd");
		else
			printf("\n-");
		permission((m &(S_IRWXU))>>6);
		permission((m &(S_IRWXG))>>3);
		permission((m &(S_IRWXO)));
		printf(" ");
	}
struct myls{
	struct stat s;
	char name[100];
};
void print_stat_node(struct myls mynode)
	{
		struct stat mystat=mynode.s;
		struct passwd *owner;
		struct group *grp;
		owner=getpwuid(mystat.st_uid);
		grp=getgrgid(mystat.st_gid);
					
		print_permission(mystat.st_mode);	//permisison
		printf("%ld ",mystat.st_nlink);		//no of hard links	
		printf("%s ",owner->pw_name);			//owner 			
		printf("%s ",grp->gr_name);			//group
		printf("%ld ",mystat.st_size);		//size of file in bytes
		struct stat info; 
		char buff[20]; 
		struct tm *timeformat;
		timeformat=localtime(&(mystat.st_mtime));
		strftime(buff, 20, "%b %d %H:%M",timeformat); 
		printf("\t%s ",buff);			//last modified data and time				
		printf("\t%s",mynode.name);
	}
//ls command's comparator
int ls_l_cmp (const void * a, const void * b) {
	struct myls *x=(struct myls*)a;
	struct myls *y=(struct myls*)b;
   	return strcmp(x->name,y->name);
}
int ls_S_cmp (const void * a, const void * b) {
	struct myls *x=(struct myls*)a;
	struct myls *y=(struct myls*)b;
   	return (y->s.st_size)>=(x->s.st_size);
}
int ls_t_cmp (const void * a, const void * b) {
	struct myls *x=(struct myls*)a;
	struct myls *y=(struct myls*)b;
   	return (y->s.st_mtime)-(x->s.st_mtime);
}
int ls_array(struct myls *lsarr)		//putting data in array of structure myls to make it easy for sorting w.r.t requirement
	{
	struct dirent *de;
	int count=0;
	DIR *dr=opendir(".");
	if(dr==NULL)
		{
		printf("\n sorry.....can't open the dir (Error occured)");
		return -1;
		}
	while((de=readdir(dr))!=NULL)
		{
		struct myls mylsnode;
		struct stat mystat;
		Stat(de->d_name,&mystat);
		mylsnode.s=mystat;
		strcpy(mylsnode.name,de->d_name);
		lsarr[count++]=mylsnode;
		}
	closedir(dr);
	return count;
	}
void print_ls(struct myls *lsarr,int count,int ls_enable,int dir_enable)	
	{
	if(ls_enable==1)
	{
	rep(count)
		if(dir_enable==0 && (strcmp(lsarr[i].name,".")==0||strcmp(lsarr[i].name,"..")==0))
			continue;
		else
		print_stat_node(lsarr[i]);
	}
	else
	{
		int i=0;
		int k=0;
		char *s[MAXFILES];
		
		rep(count)
			if(dir_enable==0 && (strcmp(lsarr[i].name,".")==0||strcmp(lsarr[i].name,"..")==0))
				continue;
			else
				s[k++]=lsarr[i].name;
				
		
		
		count=k;
		int times=ceil(count/2);
		int odd=count%2;
		for(int i=0;i<times;i++)
			{
				printf("\n%25s%25s",s[i],s[i+times+odd]);
			
			}
		if(odd)
			printf("\n%25s",s[count-times-1]);
		
	}
	printf("\n");
	}
	
void ls_l(struct myls *lsarr,int count)
	{
	if(count==-1)
		return;
	qsort(lsarr, count, sizeof(struct myls),ls_l_cmp);
	}
void ls_a(struct myls *lsarr,int count)
	{
	if(count==-1)
		return;
	qsort(lsarr,count,sizeof(struct myls),ls_l_cmp);
	}
	
void ls_S(struct myls *lsarr,int count)
	{
	if(count==-1)
		return;
	qsort(lsarr,count,sizeof(struct myls),ls_S_cmp);
	}
	
void ls_t(struct myls *lsarr,int count)
	{
	if(count==-1)
		return;
	qsort(lsarr,count,sizeof(struct myls),ls_t_cmp);
	}
	
void myls(int argc,char *argv[])
	{
	char opt;
	int ls_enable,dir_enable;
	struct myls lsarr[MAXFILES];
	optind=1;		//reset optind for every execution
	int count=ls_array(lsarr);
	ls_enable=0; 		//to check is we need to list all during printing details or not
	dir_enable=0;			//to check if we need to display dir also
	int valid=1;
	while((opt=getopt(argc,argv,":latS"))!=-1)
	{
		//printf("\n%c",opt);
		switch(opt)
		{
		case 'l':
			ls_l(lsarr,count);
			ls_enable=1;
			break;
		case 'a': 
			ls_a(lsarr,count);
			dir_enable=1;    
			break;
		case 't':		//do not sort
			//printf("\t tt");
			ls_t(lsarr,count);	//sort by modification time (newest first)
			break;
		case 'S': 
			ls_S(lsarr,count);  //Sort by file size
			break;
		case '?':valid=0;
			printf("\nInvalid command..\n");
			break;
		}
		
	}
	if(valid)
		print_ls(lsarr,count,ls_enable,dir_enable);  	///printing respective selected values
	}

//////GREP SECTION ///////////
void grep_file_handler(DIR *dr,char *path,int i_enable,int c_enable,int n_enable,char *match)
{		//single function calling itself recursively for directories and looping for files
	if(dr==NULL)
		return;
	struct dirent *de;
	while((de=readdir(dr))!=NULL)
		{
		struct stat mystat;//
		if(strcmp(".",de->d_name)==0 || strcmp("..",de->d_name)==0)	//we don't want to open present and previous folder recursively
			continue;
		stat(de->d_name,&mystat);//
		if(de->d_type!=DT_DIR)	
		{
			char file_path[MAXLEN];
			if(strlen(path)==0)
				strcpy(file_path,de->d_name);
			else
				{
				strcpy(file_path,path);
				strcat(file_path,"/");
				strcat(file_path,de->d_name);
				}
			FILE *fp;		//file pointer declaration
			fp=fopen(file_path,"rb");  	//FILE OPENING de->d_name
			if(fp==NULL)
				{
					printf("\ngrep: %s Permission denied",file_path);
					continue;
				}
			//char copyline[MAXLEN];
			char line[MAXLEN];
			int line_no=0;
			int line_toprint=-1;
			int c_count=0;
			int len=strlen(match);
			int k=0;
			
			Elf_W(Ehdr) head; //declaring elfw header
			fread(&head,1,sizeof(head),fp);
			if(head.e_ident[1] =='E'&& head.e_ident[2]=='L'&& head.e_ident[3]=='F') //for checking ELF ie binary file header
				{
					//rewind(fp);
					if(!c_enable)
						{
						printf("\nBinary file %s matches",file_path);
						continue;
						}
						
					while(!feof(fp))		//customize for count because fgets line by line 
								//traversal of binary or object won't give correct result on matching
					{
					char c;
					fscanf(fp,"%c",&c);
					//printf("%c",c);
				
					if((!i_enable && (c==match[k])) || (i_enable && tolower(c)==tolower(match[k])))
						k++;
					else
						k=0;
					
					if(k==len)
						{
							c_count++;
							k=0;
						}
					}
				}
				else
				{
					int line_match=0;
					int i=0;
					rewind(fp);
					while(!feof(fp))		//customize for count because fgets line by line 
								//traversal of binary or object won't give correct result on matching
					{
					char c;
					fscanf(fp,"%c",&c);
					//printf("%c",c);
					line[i++%MAXLEN]=c;		//% just to save program to throw error in buffer overlow
				
					if((!i_enable && (c==match[k])) || (i_enable && tolower(c)==tolower(match[k])))
						k++;
					else
						k=0;
					if(k==len)
						{
							line_match=1;
							k=0;
						}
					if(c=='\n')
						{
						line_no++;
						if(line_match==1)	
							{
							c_count++;
							line[i-1]='\0';
							if(!c_enable && n_enable)
								printf("\n%s:%d:%s",file_path,line_no,line);
							else	
							if(!c_enable)
								printf("\n%s:%s",file_path,line);
							
							}
						strcpy(line,""); //making line empty
						i=0;
						line_match=0;
						}
					}
				}
				
			if(c_enable)
				printf("\n%s:%d",file_path,c_count);
			fclose(fp);		//closing file	
			
		}
		else
			{
			
			char temp_path[MAXLEN];
			strcpy(temp_path,path);
			if(strlen(path)!=0)
				strcat(path,"/");
			strcat(path,de->d_name);	
			DIR *temp=opendir(path);
			grep_file_handler(temp,path,i_enable,c_enable,n_enable,match);
			strcpy(path,temp_path);		// recovering the previous path
			closedir(temp);
			}
		}
}
void mygrep(int argc,char *argv[])
	{
	char opt;
	int i_enable,n_enable,c_enable,r_enable;
	struct myls lsarr[MAXFILES];
	//int count=ls_array(lsarr);
	
	i_enable=0; 		//to check is we need to list all during printing details or not
	n_enable=0;			//to check if we need to display dir also
	c_enable=0;
	r_enable=0;
	optind=1;		//reset optind for every execution
	while((opt=getopt(argc,argv,":incr"))!=-1)
	{
		switch(opt)
		{
		case 'i':
			i_enable=1;
			break;
		case 'n':
			n_enable=1;    
			break;
		case 'c':		//do not sort
			c_enable=1;
			break;
		case 'r':
			r_enable=1;
			break;
		case '?':
			printf("\nInvalid command..\n");
			break;
		}
	}
	if(r_enable==0)
		printf("\n No output as -r not selected\n");
	else
	if(argv[optind]!=NULL)
		{
		DIR *dr=opendir(".");
		if(dr==NULL)//file in cfile in c
			{
			printf("\n sorry.....can't open the dir (Error occured)");
			return;
			//return -1;
			}
		char path[MAXLEN]="";
		//,i_enable,c_enable,n_enable,argv[optind]);
		grep_file_handler(dr,path,i_enable,c_enable,n_enable,argv[optind]);
		closedir(dr);
		printf("\n");
		}
	}
void cp_b(char *source,char *dest)	//take backup of the file
	{
	char new_dest[MAXLEN];
	int dest_file_exist=0;
	if(strcmp(source,dest)==0)
		{
			printf("\nSource and destination can't be same");
			return ;
		}
	FILE *fs,*fd,*temp;
	
	int fdd;
	if((fdd=open(dest,O_RDWR,0))>=0)	//checking file
		{
			close(fdd);
			dest_file_exist=1;
			
			fs=fopen(dest,"r");
			if(fs==NULL)
			{
				printf("\nSorry..premission to access file denied!");
				return;
			} 
			strcpy(new_dest,dest);
			strcat(new_dest,"~"); 
			fd=fopen(new_dest,"w");  
			if(fd==NULL)
				{
					printf("\nSorry..permission is denied!");
					return;
				}
			while(!feof(fs))
				{
					char c;
					fscanf(fs,"%c",&c);
					fprintf(fd,"%c",c);
					
				}
			fclose(fs); //close destination file
			fclose(fd);
		} 
	fs=fopen(source,"r");
	fd=fopen(dest,"w");
	if(fd==NULL || fs==NULL)
		{
			printf("\nSorry..permission is denied!");
			return;
		}

	while(!feof(fs))
		{
			char c;
			fscanf(fs,"%c",&c);
			fprintf(fd,"%c",c);
		
		}	
	fclose(fs);
	fclose(fd);
	}
int cp_i(char *source,char *dest) //copy the source file to destinatio if not present. If file already exist in destination, then prompt to y/n for overwrite + return 1 if coppied successfully
	{
	if(strcmp(source,dest)==0)
		{
			printf("\nSource and destination can't be same");
			return 0;
		}
	FILE *fs,*fd,*temp;
	fs=fopen(source,"r");
	if(fs==NULL)
		{
			printf("\nSorry..premission to access file denied!");
			return 0;
		} 
	int fdd;
	if((fdd=open(dest,O_WRONLY,0))>=0)	//checking file
		{
			close(fdd);
			char c='n';
			printf("\nFile %s already exist, want to override (y/n):",dest);
			scanf("%c",&c);
			if(c!='y' && c!='Y')
				return 0;
		}
	fd=fopen(dest,"w");
	if(fd==NULL)
		{
			printf("\nSorry..premission to access file denied!");
			return 0;
		}
	while(!feof(fs))
		{
			char ch;
			fscanf(fs,"%c",&ch);
			fprintf(fd,"%c",ch);	//copyting data to destination
		}
	printf("\n File updated");
	fclose(fs);
	fclose(fd);
	return 1;
	}
int cp_u(char *source,char *dest)		//copy only when source is newer than destination (by checking last modification time)
	{
	int already_exist=0;
	if(strcmp(source,dest)==0)
		{
			printf("\nSource and destination can't be same");
			return 0;
		}
	FILE *fs,*fd,*temp;
	
	fs=fopen(source,"r");
	
	if(fs==NULL)
		{
			printf("\nSorry..premission to access file denied!");
			return 0;
		} 
	int fdd;
	if((fdd=open(dest,O_WRONLY,0))>=0)	//checking file
		{
			close(fdd);
			already_exist=1;
		}
	if(already_exist)
		{
		struct stat s1;
		struct stat s2;
		stat(source,&s1);
		stat(dest,&s2);
		if(s1.st_mtime<s2.st_mtime)  //checking modified time
			return 1;
		}
	fd=fopen(dest,"w");
	if(fd==NULL)
		{
			printf("\nSorry..premission to access file denied!");
			return 0;
		}
	while(!feof(fs))
		{
			char ch;
			fscanf(fs,"%c",&ch);
			fprintf(fd,"%c",ch);
		}
	printf("\n File updated");
	fclose(fs);
	fclose(fd);
	return 0;
	}
void cp_f(char *source,char *dest)	//update the destinatio file if access allow, otherwise delete it and copy the data of source file
	{
	int already_exist=0;
	if(strcmp(source,dest)==0)
		{
			printf("\nSource and destination can't be same");
			return;
		}
	FILE *fs,*fd,*temp;
	fs=fopen(source,"r");
	if(fs==NULL)
		{
			printf("\nSorry..premission to access file denied!");
			return;
		} 
	int fdd;
	if((fdd=open(dest,O_WRONLY,0))>=0)	//checking file
		{
			close(fdd);
			already_exist=1;
		}
	if(!already_exist) 
		remove(dest);	//delete file only if permission not allow
	fd=fopen(dest,"w");
	if(fd==NULL)
		{
			printf("\nSorry..premission to access file denied!");
			return;
		}
	while(!feof(fs))
		{
			char ch;
			fscanf(fs,"%c",&ch);
			fprintf(fd,"%c",ch);
		}
	printf("\n File updated");
	fclose(fs);
	fclose(fd);
	}
void mycp(int argc,char *argv[])
	{
	char opt;
	int b_enable=0;
	int i_enable=0;
	int u_enable=0;
	int f_enable=0;
	optind=1;		//reset optind for every execution
	while((opt=getopt(argc,argv,":biuf"))!=-1)
	{
		//printf("\n%c",opt);
		switch(opt)
		{
		case 'b':
			b_enable=1;
			break;
		case 'i':
			i_enable=1;    
			break;
		case 'u':		//do not sort
			u_enable=1;
			break;
		case 'f':f_enable=1;
			break;
		case '?':
			printf("\nInvalid command..\n");
			break;
		}
	}
	if(b_enable)
	{
		if(argv[optind]==NULL || argv[optind+1]==NULL)
			{
				printf("\n Sorry...source or destination file not provided correctly\n");
				return;
			}		
		cp_b(argv[optind],argv[optind+1]);
		
	}
	if(i_enable)
	{
		if(argv[optind]==NULL || argv[optind+1]==NULL)
			{
				printf("\n Sorry...source or destination file not provided correctly\n");
				return;
			}		
		cp_i(argv[optind],argv[optind+1]);
		printf("\n");
	}
	if(u_enable)
	{
		if(argv[optind]==NULL || argv[optind+1]==NULL)
			{
				printf("\n Sorry...source or destination file not provided correctly\n");
				return;
			}		
		cp_u(argv[optind],argv[optind+1]);
		printf("\n");
	}
	if(f_enable)
	{
		if(argv[optind]==NULL || argv[optind+1]==NULL)
			{
				printf("\n Sorry...source or destination file not provided correctly\n");
				return;
			}		
		cp_f(argv[optind],argv[optind+1]);
		printf("\n");
	}
	
	}
void mv_b(char *source,char *dest)	//take backup of the file, take backup of destination and make sourse as destination file
	{	
		if(strcmp(source,dest)==0)
		{
			printf("\nSource and destination can't be same");
			return;
		}
		
		char backup[300];
		strcpy(backup,dest);
		strcat(backup,"~");
		rename(dest,backup);
		rename(source,dest);	
	}
void mv_u(char *source,char *dest)	////copy only when source is newer than destination (by checking last modification time)
	{
		if(strcmp(source,dest)==0)
		{
			printf("\nSource and destination can't be same");
			return;
		}
		struct stat d,src;
		int status=stat(dest,&d); //checking if dest. file exist
		if(status==0)	//checking file
		{
			stat(source,&src);
			if(src.st_mtime<d.st_mtime)  //checking modified time
				return;	
		}
		rename(source,dest);	
			
	}
void mv_f(char *source,char *dest)	//move forcefully
	{
		if(strcmp(source,dest)==0)
		{
			printf("\nSource and destination can't be same");
			return;
		}
		remove(dest);		//deleting any destination file if any	(by default rename also delete original file)
		rename(source,dest);	//renaming the file
	}
void mv_i(char *source,char *dest)	//move directly if not file at destination present, otherwise prompt (y/n)
	{
		if(strcmp(source,dest)==0)
		{
			printf("\nSource and destination can't be same");
			return;
		}
		struct stat mystat;
		int status=stat(dest,&mystat); //checking if dest. file exist
		if(status==0)	//checking file
		{
			char ch='n';
			printf("\n File already exist, want to overwrite(y/n):");
			scanf("%c",&ch);
			if(ch=='y' || ch=='Y')
				mv_f(source,dest);
			return;
		}
		rename(source,dest);	
	}
void mymv(int argc,char *argv[])
	{
	char opt;
	int b_enable=0;
	int i_enable=0;
	int u_enable=0;
	int f_enable=0;
	optind=1;		//reset optind for every execution
	while((opt=getopt(argc,argv,":biuf"))!=-1)
	{
		//printf("\n%c",opt);
		switch(opt)
		{
		case 'b':
			b_enable=1;
			break;
		case 'i':
			i_enable=1;    
			break;
		case 'u':		//do not sort
			u_enable=1;
			break;
		case 'f':f_enable=1;
			break;
		case '?':
			printf("\nInvalid command..\n");
			break;
		}
	}
	if(b_enable)
	{
		if(argv[optind]==NULL || argv[optind+1]==NULL)
			{
				printf("\n Sorry...source or destination file not provided correctly\n");
				return;
			}		
		mv_b(argv[optind],argv[optind+1]);
		
	}
	if(i_enable)
	{
		if(argv[optind]==NULL || argv[optind+1]==NULL)
			{
				printf("\n Sorry...source or destination file not provided correctly\n");
				return;
			}		
		mv_i(argv[optind],argv[optind+1]);
		printf("\n");
	}
	if(u_enable)
	{
		if(argv[optind]==NULL || argv[optind+1]==NULL)
			{
				printf("\n Sorry...source or destination file not provided correctly\n");
				return;
			}		
		mv_u(argv[optind],argv[optind+1]);
		printf("\n");
	}
	if(f_enable)
	{
		if(argv[optind]==NULL || argv[optind+1]==NULL)
			{
				printf("\n Sorry...source or destination file not provided correctly\n");
				return;
			}		
		mv_f(argv[optind],argv[optind+1]);
		printf("\n");
	}
	
	}
void ps_time(char *pid)		//it will return the time in consumed by process by taking pid
	{
			//FILE OPENING FOR CLOCKS
				FILE *fp;
				char path[300];
				snprintf(path,sizeof(path),"/proc/%s/stat",pid);
				fp=fopen(path,"r");  	//FILE OPENING de->d_name
				if(fp==NULL)
					{
						printf("\tAccess denied");
						return;
					}
				long hertz=sysconf(_SC_CLK_TCK);	
				unsigned long utime,stime,time;
				fscanf(fp,"%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu",&utime,&stime);
				time =(unsigned long)((double)(utime + stime)/hertz);
				char cpu_usage[100];
				sprintf(cpu_usage, "%02lu:%02lu:%02lu",(time/3600)%3600,(time/60)%60,time%60);
				fclose(fp);
				printf("\t%s",cpu_usage);
	
			//FILE CLOSING
			//////////////////////////
	
	}
void ps_tty(char *pid)	//it will return TTY field for particular process
	{
			char path[300];
			sprintf(path, "/proc/%s/fd/0",pid);
			//FILE OPENING FOR tty
				int fd = open(path, O_RDONLY);
				char *tty = ttyname(fd);
				close(fd);
			//FILE CLOSING
			if(tty==NULL)
				tty="?";
			else
				{
					tty=strchr(tty,'/');
					tty=strchr(tty+1,'/');
					tty+=1; //skipping /
				}
			printf("\t%s",tty);
	}
void ps_cmd(char *pid)		//it will return CMD field for process
	{
	// FILE PROCESSING  ///
			char line[300],path[300];
			snprintf(path,sizeof(path),"/proc/%s/status",pid);
			FILE *fp;
			//FILE OPENING FOR CMD
				fp=fopen(path,"r");
				if(fp==NULL)
					{
						printf("\t access denied");
						return;
					} 
				fgets(line,200,fp);
				char *cmd=line+6;  //skipping Name:
				cmd[strlen(cmd)-1]='\0'; //removing \n form end
				strcpy(pid,cmd);
				//printf("%s",cmd);
				fclose(fp);
			//FILE CLOSING
	}
void ps_uid(char *pid)		//take pid and replace pid data with uid
	{
	// FILE PROCESSING  ///
			char line[300],path[300];
			snprintf(path,sizeof(path),"/proc/%s/status",pid);
			FILE *fp;
			//FILE OPENING FOR CMD
				fp=fopen(path,"r");
				if(fp==NULL)
					{
						printf("\t access denied");
						return;
					} 
				while(!feof(fp))
					{
						fgets(line,200,fp);
						if(strstr(line,"Uid")!=NULL)
							break;	
					}	
				char *cmd=line+5;  //skipping Uid: and \t
				cmd[strlen(cmd)-1]='\0'; //removing \n form end
				char *temp=strchr(cmd,'\t');
				*temp='\0';	//only initial required
				
				strcpy(pid,cmd);
				//printf("%s",cmd);
				fclose(fp);
			//FILE CLOSING
	}
void ps_uname(char *uid)		//take uid and replace uid data with uname
	{
			char line[500],pattern[100];
			FILE *fp;
			//FILE OPENING FOR CMD
			snprintf(pattern,sizeof(pattern),":x:%s:",uid);		//data written in uname:x:uid:gid format in the file 
				fp=fopen("/etc/passwd","r");
				if(fp==NULL)
					{
						printf("\t access denied for /etc/passwd");
						return;
					} 
				while(!feof(fp))
					{
						fgets(line,500,fp);
						if(strstr(line,pattern)!=NULL)
							break;	
					}	
				char *cmd=line;
				char *temp=strchr(cmd,':');	//only username required
				*temp='\0';
				strcpy(uid,cmd);
				fclose(fp);
			//FILE CLOSING
	}
void ps_A()		//display all process in the process
{	
	DIR *dr=opendir("/proc");
	char cmd[300];
	struct dirent *de;
	printf("\nPID\tTTY\tTIME\t\tCMD");
	while((de=readdir(dr))!=NULL)
		{
			int flag=atoi(de->d_name);
			if(flag==0)
				continue;
			printf("\n%s",de->d_name);
			
			ps_tty(de->d_name);
			ps_time(de->d_name);
			
			strcpy(cmd,de->d_name);
			ps_cmd(cmd);
			printf("\t%s",cmd);
		}
}
void ps_p(int pid)
{	
	DIR *dr=opendir("/proc");
	char cmd[300];
	struct dirent *de;
	printf("\nPID\tTTY\tTIME\t\tCMD");
	while((de=readdir(dr))!=NULL)
		{
			int flag=atoi(de->d_name);
			if(flag==0)
				continue;
			if(pid!=flag)
				continue;
			
			printf("\n%s",de->d_name);
			
			ps_tty(de->d_name);
			ps_time(de->d_name);
			
			strcpy(cmd,de->d_name);
			ps_cmd(cmd);
			printf("\t%s",cmd);
		}
}
void ps_C(char *command)
{	
	DIR *dr=opendir("/proc");
	char cmd[300];
	struct dirent *de;
	printf("\nPID\tTTY\tTIME\t\tCMD");
	while((de=readdir(dr))!=NULL)
		{
			if(atoi(de->d_name)==0)
				continue;
			strcpy(cmd,de->d_name);
			ps_cmd(cmd);
			if(strcmp(command,cmd)!=0)
				continue;
			
			printf("\n%s",de->d_name);
			
			ps_tty(de->d_name);
			ps_time(de->d_name);
			printf("\t%s",cmd);
			
		}
}
void ps_username(char *uname)			//display process of any particular user
{	
	DIR *dr=opendir("/proc");
	char pid[300],cmd[300];
	struct dirent *de;
	printf("\nPID\tTTY\tTIME\t\tCMD");
	while((de=readdir(dr))!=NULL)
		{
			if(atoi(de->d_name)==0)
				continue;
				
			strcpy(pid,de->d_name);
			ps_uid(pid);	//pid contain uid not
			ps_uname(pid);	//pid contain username now
			
			if(strcmp(pid,uname)!=0)
				continue;
			
			printf("\n%s",de->d_name);
			
			ps_tty(de->d_name);
			ps_time(de->d_name);
			strcpy(cmd,de->d_name);
			ps_cmd(cmd);
			printf("\t%s",cmd);
		}
}
void myps(int argc,char *argv[])
	{
	char opt;
	int p_enable=0;
	int A_enable=0;
	int u_enable=0;
	int c_enable=0;
	optind=1;		//reset optind for every execution
	while((opt=getopt(argc,argv,":peAuUC"))!=-1)
	{
		//printf("\n%c",opt);
		switch(opt)
		{
		case 'p':		//for searching process w.r.t pid
			p_enable=1;
			break;
		case 'e':
		case 'A':		//display all process present in the system
			A_enable=1;
			break;
		case 'u':
		case 'U':		//for searching process  w.r.t usrname
			u_enable=1;
			break;
		case 'C':		//for searching process w.r.t Command CMD
			c_enable=1;
			break;
		case '?':
			printf("\nInvalid command..\n");
			break;
		}
	}
	if(p_enable)
	{
		if(argv[optind]==NULL)
			{
				printf("\nPid is missing\n");
				return;
			}		
		ps_p(atoi(argv[optind]));
		printf("\n");
	}
	if(A_enable)
	{
				
		ps_A(argv[optind],argv[optind+1]);
		printf("\n");
	}
	
	if(u_enable)
	{
		if(argv[optind]==NULL)
			{
				printf("\nUsername is missing\n");
				return;
			}		
		ps_username(argv[optind]);
		printf("\n");
	}
	if(c_enable)
	{
		if(argv[optind]==NULL)
			{
				printf("\nCommand is missing\n");
				return;
			}		
		ps_C(argv[optind]);
		printf("\n");
	}
	
	}
int main()
	{	
	while(1)
	{
		int argc;
		char *argv[20];
		char line[MAXLEN];
		int valid=0;
		if(readcommand(line)==-1)
			continue;
		if(strcmp(line,"exit")==0)
			break;
		if(strcmp(line,"help")==0)
			{
				valid=1;
				printhelp();
			}
		splitargv(&argc,argv,line); //it will split the line to respective argv values
		if(strcmp(argv[0],"ls")==0)
			{	
				valid=1;
				myls(argc,argv);
			}
		if(strcmp(argv[0],"grep")==0)
			{
				valid=1;
				mygrep(argc,argv);
			}
		if(strcmp(argv[0],"cp")==0)
			{
				valid=1;
				mycp(argc,argv);
			}
		if(strcmp(argv[0],"mv")==0)
			{
				valid=1;
				mymv(argc,argv);
			}
		if(strcmp(argv[0],"ps")==0)
			{
				valid=1;
				myps(argc,argv);
			}
		if(valid==0)
			printf("\nInvalid command..\n");
		fflush(stdout);
	}	
	printf("\nExiting...\n");
	return 0;
	}
