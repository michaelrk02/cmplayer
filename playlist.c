/* File ini sudah otomatis ter-include dalam main.c */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cmplayer.h"

#define MIN(_A, _B) (((_A) < (_B)) ? (_A) : (_B))

/* asumsikan semua arsitektur adalah menggunakan little-endian byte addressing */
#define FOURCC(_A, _B, _C, _D)      \
    ((_A) & 0xFF) |                 \
    (((_B) & 0xFF) << 8) |          \
    (((_C) & 0xFF) << 16) |         \
    (((_D) & 0xFF) << 24)

#define CMPLAYER_MAGIC  FOURCC('C', 'M', 'P', 'L')

bool playlist_load(const char *filename, playlist_t *dst) {
    FILE *fp;
    bool success = false;

    if ((fp = fopen(filename, "rb")) != NULL) {
        unsigned int magic;

        fread(&magic, sizeof(unsigned int), 1, fp);
        if (magic == CMPLAYER_MAGIC) {
            song_t song;
            unsigned int count = 0, loaded = 0;

            playlist_clear(dst);
            while (fread(&song, sizeof(song_t), 1, fp) == 1) {
                if (playlist_enqueue(dst, song.author, song.title, song.duration)) {
                    loaded++;
                }
                count++;
            }
            printf("Successfully loaded %d out of %d items from playlist %s\n", loaded, count, filename);
            success = true;
        } else {
            printf("E: %s is not a valid playlist file\n", filename);
        }
        fclose(fp);
    } else {
        printf("E: Unable to open file: %s\n", filename);
    }

    return success;
}

bool playlist_save(const char *filename, const playlist_t *src) {
    FILE *fp;
    bool success = false;

    if ((fp = fopen(filename, "wb")) != NULL) {
        unsigned int magic = CMPLAYER_MAGIC;
        playlist_item_t *iterator;
        unsigned int count = 0, written = 0;

        fwrite(&magic, sizeof(unsigned int), 1, fp);
        for (iterator = src->head; iterator != NULL; iterator = iterator->next) {
            if (fwrite(iterator->song, sizeof(song_t), 1, fp) == 1) {
                written++;
                fflush(fp);
            }
            count++;
        }
        printf("Successfully written %d out of %d items to %s\n", written, count, filename);
        success = true;
        fflush(fp);
        fclose(fp);
    } else {
        printf("E: Unable to write to %s\n", filename);
    }

    return success;
}

void playlist_print(const playlist_t *playlist) {
    playlist_item_t *iterator;
    unsigned int i = 1;

    puts("=== PLAYLIST ===");
    for (iterator = playlist->head; iterator != NULL; iterator = iterator->next) {
        printf(" [%d] ", i);

        if (iterator == playlist->head) {
            printf("[");
        }
        song_print(iterator->song);
        if (iterator == playlist->head) {
            printf("]");
        }

        puts("");
        i++;
    }
}

bool playlist_enqueue(playlist_t *playlist, const char *author, const char *title, time_t duration) {
    playlist_item_t *item;
    song_t *song;
    bool success = false;

    if ((song = song_create(author, title, duration)) != NULL) {
        if ((item = (playlist_item_t *)malloc(sizeof(playlist_item_t))) != NULL) {
            item->song = song;
            item->start = time(NULL);
            item->next = NULL;
            if (playlist->tail == NULL) {
                playlist->head = item;
                playlist->tail = item;
            } else {
                playlist->tail->next = item;
                playlist->tail = item;
            }
            printf("Enqueued: ");
            song_print(song);
            puts("");
            success = true;
        } else {
            puts("E: Unable to enqueue a song because of insufficient memory available");
        }
    } else {
        puts("E: Song creation failed");
    }

    return success;
}

void playlist_dequeue(playlist_t *playlist) {
    if (playlist->head != NULL) {
        playlist_item_t *head = playlist->head;

        playlist->head = head->next;
        if (playlist->head == NULL) {
            playlist->tail = NULL;
        } else {
            time_t now = time(NULL);
            time_t stop = head->start + head->song->duration;
            playlist->head->start = MIN(now, stop);
        }

        printf("Dequeued: ");
        song_print(head->song);
        puts("");

        song_destroy(head->song);
        free(head);
    }
}

void playlist_clear(playlist_t *playlist) {
    while (playlist->tail != NULL) {
        playlist_dequeue(playlist);
    }
}

void playlist_refresh(playlist_t *playlist) {
    while ((playlist->head != NULL) && (time(NULL) > playlist->head->start + playlist->head->song->duration)) {
        playlist_dequeue(playlist);
    }
}

