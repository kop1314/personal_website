#include <stdio.h>
#include <unistd.h>
int main()
{
  int x;
  /*
  for (;;) {
    printf("\nHow many circles do you want to write? (to end the program click Ctrl+D at the same time!)\n");
    int res = scanf("%d", &x);
    if (res == EOF) break;
    if (res == 1) {
     // Draw x circles here
    } else {
      printf("Invalid input is ignored.\n");
      scanf("%*[^\n]");
    }
    
  }
  */
  while(1){
    printf("hello world\n");
    sleep(2);
  }
  printf("\n\n Bye! \n\n");
  return 0;
}
