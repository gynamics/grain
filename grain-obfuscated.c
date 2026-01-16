/**
 * grain-obfuscated.c: terminal character rain like a colorful `matrix'
 * ! make sure your terminal supports 256 color
 *
 * CopyRevolted by gynamics
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
typedef char *s;
typedef struct winsize wsz;
typedef struct { short r : 4; short g : 4; short b : 4; short a : 4; } rgb_t;
#define dt(i, l)                                      for (i = 0; i < l; i++)
#define for2(l1, l2, rv1, cv, rv2    ) dt(i, l1) { rv1; dt(j, l2) { cv; } rv2; }
#define forsz(sz, rv1, cv, rv2)    for2(wr1(sz), wc(sz), rv1, cv, rv2)
#define getws( ws )               ioctl(fileno(stdout), TIOCGWINSZ, (s) & ws)
#define ca(t, l)                     (t *)calloc(l, sizeof(t))
#define wr(ws)                         (ws).ws_row
#define wr1(                             ws) wr(ws) + 1
#define wc(                               ws) (ws).ws_col
#define s0j            sc[0][             j]
#define s1j          sc[i - 1][j           ]
#define sij          sc[i    ][j           ]
#define s14           s0j[  11+           3]
#define s15             s1j[             15]
#define rd(n)                           (rand() % (n))
#define rdch( )                        (char)(0x20 + rd(0x5f))
#define dye(r,g,b)                  ((r) * 36 + (g) * 6 + (b) + 16)
#define rgbx(c, op)               dye((c.r)op, (c.g)op, (c.b)op)
#define inits0j(op)   sprintf(sc[0][j],"\e[38;5;%dm%c", rgbx(c, op), rdch())
#define min(x, y)                                              ((x < y) ? x : y)
int fps=25;int i,j,paus=1; s **sc,**tc; wsz sz,tz; rgb_t c={0,5,0,0};void alsc(s
***psc,wsz sz,rgb_t c){s **sc=ca(s *,wr1(sz));forsz(sz,sc[i]=ca(s,wc(sz)),sij=ca
(char,16),);for2(wr(sz),wc(sz),,sij[0]=' ',);dt(j,wc(sz)){s14=rd(8);s0j[15]=1;/*
*/inits0j();}*psc=sc;}int ms(int n){struct timespec sp={n/1000,(n%1000)*1e6};/**
**/return nanosleep(&sp,0);}void hq(int _){exit(puts("\e[u\e[?25h\e[0m")==EOF);}
void hp(int _){paus=!paus;}void cp(){for2(min(wr(sz),wr(tz)),min(wc(sz),wc(tz)),
,strcpy(tc[i][j],sij),);}void rp(){forsz(sz,,free(sij),free(sc[i]));sc=tc;sz=tz;
}void wt(){for(;!paus;)ms(10);}int pr(){dt(j,wc(sz))(((s0j[14]==6)?(s14=2+(rand(
)+j)%4):(rd(1+s0j[14]))?:(s14=(s14+6)%7)),inits0j(*s14/6));for(i=wr(sz);i>0;i--)
dt(j,wc(sz))(s15?s15--:((s15=rd(2+2*(wr(sz)-i))/wr(sz)),strcpy(sij,s1j),0));/**/
printf("\e[1;1H");forsz(sz,,printf("%s",sc[i][j]),);fflush(stdout);return 1;}int
main(int argc,s argv[]){int cnt=0,rbc=0;srand(time(0));alsc(&sc,(getws(sz),sz),c
);signal(2,hq);signal(20,hp);puts("\e[s\e[25?l");for(;;){(cnt++<5)?1:((cnt=0),//
(getws(tz),((wc(tz)-wc(sz))|(wr(tz)-wr(sz)))?:(alsc(&tc,tz,c),cp(),rp(),0)));/**
*/(paus?pr():(fflush(stdout),wt(),0));ms(1e3/fps);(++rbc<20)?1:((c=(rgb_t){rd(6)
,rd(6),rd(6),c.a}),(rbc=0));}return 0;}/** The sqaure width is aligned to 80. */
