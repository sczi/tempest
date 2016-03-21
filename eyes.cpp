/*
   Based on code from tempest for eliza and tempest-AM

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
   */

// ./a.out 108000000 1280 1024 1688 1280000
// ./a.out 70630000 1366 768 1490 1280000
// compile with: g++ eyes.cpp -o eyes -lSDL
// z=C, x=D, c=E, s=C#, d=D# etc
// q,w,e... 2,3... is one octave higher

#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <string.h>
#include <SDL/SDL_events.h>
#include <vector>
#include <algorithm>

using namespace std;

double carrier;
int resx;
int resy;
int horizontalspan;
double pixelclock;
int freqs_n;
double freqs[100];
double tmp[100];

SDL_Surface *screen;

inline void pixelchen (const int x, const int y, const Uint32 pixel)
{
    Uint8 *bits=((Uint8*)screen->pixels)+y*screen->pitch+x*screen->format->BytesPerPixel;
    switch(screen->format->BytesPerPixel) {
        case 1:
            *((Uint8 *)(bits)) = (Uint8)pixel;
            break;
        case 2:
            *((Uint16 *)(bits)) = (Uint16)pixel;
            break;
        case 3:
            { /* Format/endian independent */
                Uint8 r, g, b;

                r = (pixel>>screen->format->Rshift)&0xFF;
                g = (pixel>>screen->format->Gshift)&0xFF;
                b = (pixel>>screen->format->Bshift)&0xFF;
                *((bits)+screen->format->Rshift/8) = r; 
                *((bits)+screen->format->Gshift/8) = g;
                *((bits)+screen->format->Bshift/8) = b;
            }
            break;
        case 4:
            *((Uint32 *)(bits)) = (Uint32)pixel;
            break;
    };
};

void mkrealsound()
{
    float freq;
    SDL_Rect myrect;
    myrect.x=myrect.y=0;
    myrect.w=resx;
    myrect.h=resy;

    int x, y, str, pw1, pw2, tmax, tmin, i;
    float t, A, m, tmp1, tmp2, tmp3, tmp4;

    /* Settings and pre-calculations */
    A = 255 / 4.0;			/* amplitude */
    m = 1.0;			/* amplitude */

    //double ftfp2 = freq / pixelclock * 2.0;
    float fcfp2 = carrier / pixelclock * 2.0;

    t = 0;
    for (y = 0; y < resy; y++) {

        for (i = 0; i < freqs_n; i++)
            tmp[i] = cos(M_PI * t * 2.0 * freqs[i] / pixelclock + M_PI);

        for (x = 0; x < resx; x++) {
            tmp2 = cos(M_PI * t * fcfp2);
            str = 3 * A * (m + tmp2);

            for (i = 0; i < freqs_n;i++)
                str *= (m + tmp[i]);

            /* min is (str - 512), max = 256 */
            tmax = min(256,str);
            tmin = max(0,str - 512);
            /* min is (str - pw1 - 256), max = 256 */
            tmax = min(256,str-pw1);
            tmin = max(0,(str - pw1 - 256));
            pw1 = min(255,str);
            pw2 = min(255,str-pw1);
            pixelchen(x, y, SDL_MapRGB(screen->format,pw1,str-pw1-pw2,pw2));
            t++;
        }
        t += horizontalspan - resx;
    }

    SDL_UpdateRect(screen,0,0,resx,resy);
}

double note (int nr)
{
    return pow(2.0,nr/12.0)*440.0*2;
};

void remove_freq (double f) {
    int i,j;

    for (i = 0; i < freqs_n; i++)
        if (fabs(freqs[i]-f) < 0.0005) {
            freqs_n--;
            for (j = i; j < freqs_n; j++)
                freqs[j] = freqs[j+1];
            return;
        }
}


void keyboard () {
    mkrealsound();
    SDL_Event event;
    while(1) {
        if(SDL_PollEvent(&event)) {
            if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case 'q': freqs[freqs_n++] = note(0); break;
                    case '2': freqs[freqs_n++] = note(1); break;
                    case 'w': freqs[freqs_n++] = note(2); break;
                    case '3': freqs[freqs_n++] = note(3); break;
                    case 'e': freqs[freqs_n++] = note(4); break;
                    case 'r': freqs[freqs_n++] = note(5); break;
                    case '5': freqs[freqs_n++] = note(6); break;
                    case 't': freqs[freqs_n++] = note(7); break;
                    case '6': freqs[freqs_n++] = note(8); break;
                    case 'y': freqs[freqs_n++] = note(9); break;
                    case '7': freqs[freqs_n++] = note(10); break;
                    case 'u': freqs[freqs_n++] = note(11); break;
                    case 'i': freqs[freqs_n++] = note(12); break;
                    case '9': freqs[freqs_n++] = note(13); break;
                    case 'o': freqs[freqs_n++] = note(14); break;
                    case '0': freqs[freqs_n++] = note(15); break;
                    case 'p': freqs[freqs_n++] = note(16); break;
                    case '[': freqs[freqs_n++] = note(17); break;
                    case '=': freqs[freqs_n++] = note(18); break;
                    case ']': freqs[freqs_n++] = note(19); break;
                    case 'z': freqs[freqs_n++] = note(-12); break;
                    case 's': freqs[freqs_n++] = note(-11); break;
                    case 'x': freqs[freqs_n++] = note(-10); break;
                    case 'd': freqs[freqs_n++] = note(-9); break;
                    case 'c': freqs[freqs_n++] = note(-8); break;
                    case 'v': freqs[freqs_n++] = note(-7); break;
                    case 'g': freqs[freqs_n++] = note(-6); break;
                    case 'b': freqs[freqs_n++] = note(-5); break;
                    case 'h': freqs[freqs_n++] = note(-4); break;
                    case 'n': freqs[freqs_n++] = note(-3); break;
                    case 'j': freqs[freqs_n++] = note(-2); break;
                    case 'm': freqs[freqs_n++] = note(-1); break;
                    case ',': freqs[freqs_n++] = note(0); break;
                    case 'l': freqs[freqs_n++] = note(1); break;
                    case '.': freqs[freqs_n++] = note(2); break;
                    case ';': freqs[freqs_n++] = note(3); break;
                    case '/': freqs[freqs_n++] = note(4); break;
                    case SDLK_ESCAPE: exit(0);
                    default: break;
                }
                mkrealsound();
            } else if(event.type == SDL_KEYUP) {
                switch(event.key.keysym.sym) {
                    case 'q': remove_freq(note(0)); break;
                    case '2': remove_freq(note(1)); break;
                    case 'w': remove_freq(note(2)); break;
                    case '3': remove_freq(note(3)); break;
                    case 'e': remove_freq(note(4)); break;
                    case 'r': remove_freq(note(5)); break;
                    case '5': remove_freq(note(6)); break;
                    case 't': remove_freq(note(7)); break;
                    case '6': remove_freq(note(8)); break;
                    case 'y': remove_freq(note(9)); break;
                    case '7': remove_freq(note(10)); break;
                    case 'u': remove_freq(note(11)); break;
                    case 'i': remove_freq(note(12)); break;
                    case '9': remove_freq(note(13)); break;
                    case 'o': remove_freq(note(14)); break;
                    case '0': remove_freq(note(15)); break;
                    case 'p': remove_freq(note(16)); break;
                    case '[': remove_freq(note(17)); break;
                    case '=': remove_freq(note(18)); break;
                    case ']': remove_freq(note(19)); break;
                    case 'z': remove_freq(note(-12)); break;
                    case 's': remove_freq(note(-11)); break;
                    case 'x': remove_freq(note(-10)); break;
                    case 'd': remove_freq(note(-9)); break;
                    case 'c': remove_freq(note(-8)); break;
                    case 'v': remove_freq(note(-7)); break;
                    case 'g': remove_freq(note(-6)); break;
                    case 'b': remove_freq(note(-5)); break;
                    case 'h': remove_freq(note(-4)); break;
                    case 'n': remove_freq(note(-3)); break;
                    case 'j': remove_freq(note(-2)); break;
                    case 'm': remove_freq(note(-1)); break;
                    case ',': remove_freq(note(0)); break;
                    case 'l': remove_freq(note(1)); break;
                    case '.': remove_freq(note(2)); break;
                    case ';': remove_freq(note(3)); break;
                    case '/': remove_freq(note(4)); break;
                    default: break;
                }
                mkrealsound();
            }
        }
    }
};

void usage()
{
    printf("\nwrong parameters ! read readme file!\n\n");
    exit(1);
};

int main(int argc, char *argv[])
{
    atexit(SDL_Quit);

    if (argc!=6) usage();
    pixelclock=atof(argv[1]);
    resx=atol(argv[2]);
    resy=atol(argv[3]);
    horizontalspan=atol(argv[4]);
    carrier=atof(argv[5]);

    printf("\n"
            "Pixel Clock %.0f Hz\n"
            "X Resolution %d Pixels\n"
            "Y Resolution %d Pixels\n"
            "Horizontal Total %d Pixels\n"
            "AM Carrier Frequency %.0f Hz\n"
            "\n\n",
            pixelclock,resx,resy,horizontalspan,carrier);

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    };

    /* Have a preference for 8-bit, but accept any depth */
    screen = SDL_SetVideoMode(resx, resy, 8, SDL_SWSURFACE|SDL_ANYFORMAT|SDL_FULLSCREEN);
    if ( screen == NULL ) {
        fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
        exit(1);
    }
    printf("Have Set %d bits-per-pixel mode\n",
            screen->format->BitsPerPixel);

    keyboard();

    return 0;
};
