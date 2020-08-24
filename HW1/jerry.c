#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
  FILE * fp;
  char buff[256];
  int mode = 0;
  char ptmp[64] = "id -u ";
  char ptmp2[64];
  char str[128];

  printf("*****welcome to jerry mode*****\n");
  printf("please enter mode (1-blockOpen, 2-blockKill) : ");
  scanf("%d",&mode);
  if(mode==1)
  {
    int exit2 = 0;
    printf("Do you want to exit blockRead mode? (1-No,2-Yes) ");
    scanf("%d",&exit2);
    if(exit2==1)
    {
      printf("Enter username : ");
      scanf("%s",ptmp2);
      strcat(ptmp,ptmp2);
      fp = popen(ptmp, "r");
      fgets(buff,64,fp);
      printf("uid is %s",buff);
      printf("Enter filename : ");
      scanf("%s",str);
      strcat(buff,str);
      strcat(buff,"\n");
    }
    else{
      strcpy(buff,"!");
    }

    FILE * fp = fopen("/proc/mousehole","w");
    fputs(buff,fp);

    printf("Job is completed!\n");
    fclose(fp);
  }

  else if(mode==2)
  {
    int exit = 0;
    printf("Do you want to exit blockKill mode? (1-No,2-Yes) ");
    scanf("%d",&exit);
    if(exit==1)
    {
      printf("Enter username : ");
      scanf("%s",ptmp2);
      strcat(ptmp,ptmp2);
      fp = popen(ptmp, "r");
      fgets(buff,64,fp);
      printf("uid is %s",buff);
      strcpy(ptmp," ");
      strcat(ptmp,buff);
      strcat(ptmp,"\n");
    }
    else{
      strcpy(ptmp," e");
    }
      FILE * fp = fopen("/proc/mousehole","w");
      fputs(ptmp,fp);
      printf("Job is completed!\n");
      fclose(fp);
  }

  else{
    printf("Wrong input!");
  }

  return 0;
}
