#include <stdio.h>

#include "cmplayer.h"

playlist_t g_playlist;

typedef void (*menu_callback_t)(void);

void view_playlist(void);
void load_playlist(void);
void save_playlist(void);
void clear_playlist(void);
void enqueue_song(void);
void now_playing(void);
void skip_song(void);

int main() {
    puts("Welcome to Console Media Player (CMPlayer)!");
    while (true) {
        int menu_choice;
        menu_callback_t menu_callback[] = {
            view_playlist,
            load_playlist,
            save_playlist,
            clear_playlist,
            enqueue_song,
            now_playing,
            skip_song
        };

        playlist_refresh(&g_playlist);

        puts("=== MENU ===");
        puts(" [1] View Playlist");
        puts(" [2] Load Playlist");
        puts(" [3] Save Playlist");
        puts(" [4] Clear Playlist");
        puts(" [5] Enqueue Song");
        puts(" [6] Now Playing");
        puts(" [7] Skip Song");
        puts(" [other] Exit");
        printf("Choice: ");
        scanf("%d%*c", &menu_choice);

        playlist_refresh(&g_playlist);

        if ((1 <= menu_choice) && (menu_choice <= sizeof(menu_callback)/sizeof(menu_callback_t))) {
            menu_callback[menu_choice - 1]();
        } else {
            break;
        }
    }

    return 0;
}

void view_playlist(void) {
    if (g_playlist.head != NULL) {
        playlist_print(&g_playlist);
    } else {
        puts("Playlist is empty");
    }
}

void load_playlist(void) {
    char filename[512];

    printf("Enter file name: ");
    scanf("%511[^\n]%*c", filename);

    if (playlist_load(filename, &g_playlist)) {
        puts("Playlist successfully loaded");
    }
}

void save_playlist(void) {
    char filename[512];

    printf("Enter file name: ");
    scanf("%511[^\n]%*c", filename);

    if (playlist_save(filename, &g_playlist)) {
        puts("Playlist successfully saved");
    }
}

void clear_playlist(void) {
    playlist_clear(&g_playlist);
}

void enqueue_song(void) {
    char author[64], title[128];
    unsigned int hours, mins, secs;

    printf("Enter song author: ");
    scanf("%63[^\n]%*c", author);

    printf("Enter song title: ");
    scanf("%127[^\n]%*c", title);

    printf("Enter song duration (HH:MM:SS): ");
    if (scanf("%d:%d:%d%*c", &hours, &mins, &secs) == 3) {
        if (((0 <= hours) && (hours <= 99)) &&
            ((0 <= mins) && (mins <= 59)) &&
            ((0 <= secs) && (secs <= 59))) {
            time_t duration = hours * 3600 + mins * 60 + secs;
            if (!playlist_enqueue(&g_playlist, author, title, duration)) {
                puts("E: Unable to enqueue song");
            }
        } else {
            puts("E: Maximum song duration is 99:59:59");
        }
    } else {
        puts("E: Invalid format");
    }
}

void now_playing(void) {
    if (g_playlist.head != NULL) {
        time_t t;
        unsigned int hours, mins, secs;

        puts("=== NOW PLAYING ===");
        song_print(g_playlist.head->song);
        puts("");

        t = time(NULL) - g_playlist.head->start;
        to_hms(t, &hours, &mins, &secs);
        printf("Position: %02u:%02u:%02u\n", hours, mins, secs);

        t = g_playlist.head->start + g_playlist.head->song->duration - time(NULL);
        to_hms(t, &hours, &mins, &secs);
        printf("> %02u:%02u:%02u left\n", hours, mins, secs);
    } else {
        puts("Playlist is empty");
    }
}

void skip_song(void) {
    if (g_playlist.head != NULL) {
        printf("Skipped: ");
        song_print(g_playlist.head->song);
        puts("");

        playlist_dequeue(&g_playlist);
    } else {
        puts("Playlist is empty");
    }
}

void to_hms(time_t t, unsigned int *hours, unsigned int *mins, unsigned int *secs) {
    *hours = (unsigned int)(t / 3600);
    *mins = (unsigned int)((t / 60) % 60);
    *secs = (unsigned int)(t % 60);
}

#include "song.c"
#include "playlist.c"

