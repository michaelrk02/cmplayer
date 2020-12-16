#ifndef _CMPLAYER_H
#define _CMPLAYER_H

#include <time.h>

#ifndef __cplusplus
typedef int bool;
#define true 1
#define false 0
#endif

typedef struct {
    char author[64];
    char title[128];
    time_t duration;
} song_t;

song_t *song_create(const char *author, const char *title, time_t duration);
void song_print(const song_t *song);
void song_destroy(song_t *song);

struct _playlist_item_t {
    song_t *song;
    time_t start;
    struct _playlist_item_t *next;
};

typedef struct _playlist_item_t playlist_item_t;

typedef struct {
    playlist_item_t *head;  /* to be dequeued */
    playlist_item_t *tail;  /* enqueued */
} playlist_t;

bool playlist_load(const char *filename, playlist_t *dst);
bool playlist_save(const char *filename, const playlist_t *src);
void playlist_print(const playlist_t *playlist);
bool playlist_enqueue(playlist_t *playlist, const char *author, const char *title, time_t duration);
void playlist_dequeue(playlist_t *playlist);
void playlist_clear(playlist_t *playlist);
void playlist_refresh(playlist_t *playlist);

void to_hms(time_t t, unsigned int *hours, unsigned int *mins, unsigned int *secs);

#endif
