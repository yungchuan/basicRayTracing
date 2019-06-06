#include <stdio.h>
#include <string.h>
#include <time.h>
#include "algebra3.h"
#include "output.h"
#include "shader.h"

int main()
{
    time_t timer_s, timer_f;
    Shader shader;
    char filename[30] = "example.txt";
    if(!shader.readFile(filename)){
        do{
            printf("Cannot find the file, please input the name of information file\n>>");
            scanf("%s",filename);
        }while(!shader.readFile(filename));
    }
    vec3 up_direction(0,1,0);
    timer_s = time(0);
    shader.setScreen(1, up_direction);
    filename[strlen(filename)-4] = '\0';
    strcat(filename, ".ppm\0");
    shader.outputPPMformat(filename);
    timer_f = time(0);
    printf("\n\nTotal time: %d seconds.\n\n", timer_f - timer_s);

    return 0;
}
