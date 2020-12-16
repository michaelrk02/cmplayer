/* File ini sudah otomatis ter-include dalam main.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmplayer.h"

song_t *song_create(const char *author, const char *title, time_t duration) {
    song_t *song;

    song = (song_t *)malloc(sizeof(song_t));
    if (song != NULL) {
        strcpy(song->author, author);
        strcpy(song->title, title);
        song->duration = duration;
    } else {
        puts("E: Unable to create a song due to insufficient memory");
    }

    return song;
}

void song_print(const song_t *song) {
    char duration[16];
    unsigned int hours, mins, secs;

    to_hms(song->duration, &hours, &mins, &secs);
    sprintf(duration, "%02u:%02u:%02u", hours, mins, secs);

    printf("%s - %s (%s)", song->author, song->title, duration);
}

void song_destroy(song_t *song) {
    free(song);
}

