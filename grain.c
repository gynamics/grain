/* 
 * A terminal character rain toy just like the famous *matrix*
 * make sure your terminal supports 256 color
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>

//length of "\e[38;5NmX" is no more than 13
#define STR_UNIT 16
#define min(x, y) ({                           \
    typeof(x) _x=(x);                          \
    typeof(y) _y=(y);                          \
    (_x < _y) ? _x : _y;                       \
})

struct rgb_t{short r:4;short g:4; short b:4;short a:4;};

void alloc_scene(char ****pscene, struct winsize *pwsz){
    *pscene = (char ***)malloc(sizeof(char **) * pwsz->ws_row);
    int i, j;
    for(i=0; i<pwsz->ws_row+1; i++){
        (*pscene)[i] = (char **)malloc(sizeof(char *) * pwsz->ws_col);
        if(!(*pscene)[i]){
            perror("memory allocation failure.\n"); abort();
        }
        for(j=0; j<pwsz->ws_col; j++){
            (*pscene)[i][j] = (char *)calloc(STR_UNIT, sizeof(char));
            if(!(*pscene)[i][j]){
                perror("memory allocation failure.\n"); abort();
            }
        }
    }
    return;
}

void free_scene(char ***scene, struct winsize *pwsz){
    int i, j;
    for(i=0; i<pwsz->ws_row+1; i++){
        for(j=0; j<pwsz->ws_col; j++){
            free(scene[i][j]);
        }
        free(scene[i]);
    }
    free(scene);
    return;
}

void copy_scene(char ***dst, char ***src, int ws_row, int ws_col){
    for(int i=0; i<ws_row+1; i++){
        for(int j=0; j<ws_col; j++){
            strcpy(dst[i][j], src[i][j]);
        }
    }
}

void init_scene(char ***scene, struct winsize *pwsz, struct rgb_t c){
    int j;
    for(j=0; j<pwsz->ws_col; j++){
        scene[0][j][14] = rand() % 8; // str[14]: color depth
        scene[0][j][15] = 1;  // str[15]: speed factor
        sprintf(scene[0][j], "\e[38;5;%dm" "%c",
            dye(c.r, c.g, c.b), // just green
            (char)(0x20 + rand() % 0x5f));// all visible ascii
    }
}

void dump_scene(char ***scene, struct winsize *pwsz, struct rgb_t c){
    int i, j;
    for(j=0; j<pwsz->ws_col; j++){
        if(scene[0][j][14] == 6){
            scene[0][j][14] = 2 + (rand()+j) % 4;
        } else if(rand() % (1+scene[0][j][14]) == 0) {
            scene[0][j][14] = (scene[0][j][14]+6)%7;
        }
        sprintf(scene[0][j], "\e[38;5;%dm" "%c",
                dye(c.r*scene[0][j][14]/6, c.g*scene[0][j][14]/6, c.b*scene[0][j][14]/6),
                (char)(0x20 + rand() % 0x5f));
    }
    for(i=pwsz->ws_row; i>0; i--){
        for(j=0; j<pwsz->ws_col; j++){ 
            if(scene[i-1][j][15] == 0){
                scene[i-1][j][15] = rand() % (2 + 2*(pwsz->ws_row-i)/pwsz->ws_row);
                strcpy(scene[i][j], scene[i-1][j]);
            } else {
                scene[i-1][j][15]--;
            }
        }
    }
    return;
}

void print_sence(char ***scene, struct winsize *pwsz){
    int i, j;
    printf("\e[1;1H"); // set cursor to (1,1)
    for(i=1; i<=pwsz->ws_row; i++){
        for(j=0; j<pwsz->ws_col; j++){
            printf("%s",scene[i][j]);
        }
    }
    fflush(stdout);
    return;
}

int pflag = 1; // set it to 0 for pause

void hquit(){ 
    // pop & show cursor, recover style, clear screen
    puts("\e[u" "\e[?25h" "\e[0m" "\e[2J");
    exit(0);
 }

void hpause(){ 
    pflag = !pflag; 
}

int main(int argc, char *argv[]){
    if(argc > 1){
        puts("please run this program without argument");
        puts("Usage:");
        puts("\tCtrl+z for pause");
        puts("\tCtrl+c for quit");
        return 0;
    }

    char ***scene;
    struct winsize wsz;
    struct rgb_t color = {0, 5, 0};
    // get terminal size(in characters)
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&wsz);
    alloc_scene(&scene, &wsz);
    srand(time(0));
    init_scene(scene, &wsz, color);
    // register INT handlers, optional
    signal(SIGINT, hquit);
    signal(SIGTSTP, hpause);
    
    char ***tscene;
    struct winsize twsz;
    int cnt = 0;
    int rainbowcnt = 0;
    // push & hide cursor
    puts("\e[s" "\e[25?l");
    while(1){ // add more interactive features here
        if(cnt++ == 5){
            cnt = 0;
            ioctl(fileno(stdout), TIOCGWINSZ, (char *)&twsz);
            if(twsz.ws_col != wsz.ws_col || twsz.ws_row != wsz.ws_row){
                alloc_scene(&tscene, &twsz);
                init_scene(tscene, &twsz, color);
                copy_scene(tscene, scene, min(wsz.ws_row, twsz.ws_row), min(wsz.ws_col, twsz.ws_col));
                free_scene(scene, &wsz);
                scene = tscene;
                wsz = twsz;
            }
        }
        if(pflag){
            dump_scene(scene, &wsz, color);
            print_sence(scene, &wsz);
        }else{
            fflush(stdout);
            while(!pflag);
        }
        usleep(40000); // fps = 1,000,000 / 40,000 = 25hz
        rainbowcnt++;
        if(rainbowcnt == 20){// rainbow!
            color.r = rand()%6;
            color.g = rand()%6;
            color.b = rand()%6;
            rainbowcnt = 0;
        }
    }
    return 0;
}
