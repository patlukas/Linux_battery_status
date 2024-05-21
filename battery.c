#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MAX_PATH_LEN 256

long read_value_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Błąd: Nie można otworzyć pliku %s\n", filename);
        exit(1);
    }

    long value;
    fscanf(file, "%ld", &value);
    fclose(file);

    return value;
}

int main() {
    char energy_now_path[MAX_PATH_LEN];
    char energy_full_path[MAX_PATH_LEN];

    // Budowanie pełnych ścieżek do plików energy_now i energy_full
    snprintf(energy_now_path, sizeof(energy_now_path), "/sys/class/power_supply/BAT1/energy_now");
    snprintf(energy_full_path, sizeof(energy_full_path), "/sys/class/power_supply/BAT1/energy_full");

    float p_now, p_old = -1.0;
    int t1, t5, t10;

    while (1) {
        long energy_now = read_value_from_file(energy_now_path);
        long energy_full = read_value_from_file(energy_full_path);

        // Odczytywanie aktualnej godziny
        time_t now;
        struct tm *local_time;
        time(&now);
        local_time = localtime(&now);

        p_now = (float) energy_now / (float) energy_full * 100.0;
        if(p_old < 0.0) {
            p_old = p_now;
            int t = (local_time->tm_hour) * 3600 + (local_time->tm_min) * 60 + (local_time->tm_sec);
            t1 = t;
            t5 = t;
            t10 = t;
        }

        printf("\r                                                                          \r");

        int t = (local_time->tm_hour) * 3600 + (local_time->tm_min) * 60 + (local_time->tm_sec);

        if((int) p_now != (int) p_old) {
            printf("%02d:%02d:%02d\t", local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
            printf("%d%%\t", (int) p_now);
            p_old = p_now;
            printf("%d\t", (t - t1));
            t1 = t;
            if(((int) p_now) % 5 == 0) {
                printf("%d\t", (t - t5));
                t5 = t;
            }
            if(((int) p_now) % 10 == 0) {
                printf("%d\t", (t - t10));
                t10 = t;
            }
            printf("\n");
        }
        printf("%02d:%02d:%02d\t", local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
        printf("%.2f%%\t%d", p_now, (t - t1));
        fflush(stdout);
                
        sleep(1);
    }

    return 0;
}
