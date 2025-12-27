#include "../libs/jansson.h"
#include <time.h>
#include <stdbool.h>

// Charge les lecons
void load_json(char *file_name, json_t **p_json) {
    json_error_t error;
    *p_json = json_load_file(file_name, 0, &error);

    if (!*p_json) {
        printf("Couldn't load the json file, exiting...\n");
        exit(1);
    }
}

// Enregistre les lecons
void save_json(char *file_name, json_t *json) {
    json_dump_file(json, file_name, JSON_INDENT(3));
}

// Recupere les lessons du jour passe en parametres
void get_lessons(json_t *json, time_t day, json_t *lessons_of_the_day) {
    // liste json contenant les jours j+x de revisions
    json_t *days_json = json_object_get(json, "days");

    // liste d'entiers stockant les j+x jours de revisions
    int *days = malloc(sizeof(int) * json_array_size((days_json)));
    size_t _;
    int i = 0;
    json_t *day_;
    // Stocke dans le tableau d'entiers tout les entiers
    json_array_foreach(days_json, _, day_) {
        days[i] = json_integer_value(day_);
        i++;
    }

    // liste json contenant les lecons
    json_t *lessons = json_object_get(json, "lessons");
    // index de chaque lecon
    size_t index;
    // chaque lecon
    json_t *lesson;

    // Boucle sur toutes les lecons
    json_array_foreach(lessons, index, lesson) {
        // liste json contenant les booleens
        json_t *next = json_object_get(lesson, "next");

        int index = 0;
        bool found = false;
        
        // Chaque booleen de la liste
        json_t *boolean;
        
        // Boucle sur le tableau de booleens tant que 'true' n'est pas trouve
        do {
            boolean = json_array_get(next, index);

            if (json_is_true(boolean)) {
                found = true;
            }
            else {
                index++;
            }
        }
        while (!found && index < json_array_size(next));

        // Si 'true' est trouve, on verifie si le jour correspond
        if (found) {
            // Extraction de la date de creation de la lecon en json
            json_t *date_json = json_object_get(lesson, "date");
            // Cette meme date en chaine de caractere
            const char *date_str = json_string_value(date_json);
            
            // Date a verifier avec la date actuelle
            time_t date;
            
            // Extraction des infos de la chaine de caracteres
            struct tm tmp = {0};
            sscanf(date_str, "%d-%d-%d", &tmp.tm_year, &tmp.tm_mon, &tmp.tm_mday);
            tmp.tm_year -= 1900;
            tmp.tm_mon -= 1;
            
            // Calcul du temps de creation + le nb de jours trouves
            tmp.tm_mday += days[index];
            date = mktime(&tmp);

            // Si la date correspond au jour
            if (date == day) {
                json_array_append(lessons_of_the_day, lesson);
            }
        }
    }

    free(days);
}

// Ajoute une lecon
void add_lesson(json_t *json, time_t today, json_t *values) {
    json_t *lessons = json_object_get(json, "lessons");
    json_t *next_id = json_object_get(json, "nextId");

    // Incremente l'id de la prochaine lecon de 1
    json_object_set(json, "nextId", json_integer(json_integer_value(next_id) + 1));

    // Ajoute la lecon
    json_array_append(lessons, values);
}