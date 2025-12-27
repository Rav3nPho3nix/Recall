#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>

#include "../libs/jansson.h"
#include "../headers/json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include <time.h>

#define PORT 8888

int main(int argc, char *argv[]) {
    // Verifie qu'il y a bien un argument
    if (argc == 1) {
        printf("The name of the json file is needed, exiting...\n");
        return EXIT_FAILURE;
    }

    // Charge le json en memoire
    json_t *json;
    load_json(argv[1], &json);
    printf("- Json file loaded successfully\n");

    // Recupere la date du jour
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    t->tm_sec=0;
    t->tm_min=0;
    t->tm_hour=0;


    json_t *res = json_array();
    // get_lessons_today(json, mktime(t), res);
    get_lessons(json, mktime(t), res);
    printf("Aujourd'hui \n: %s\n", json_dumps(res, JSON_INDENT(3)));

    return 0;
}

// void test_creation_json(char **p_p_json) {
//     json_t *root, *days, *nextId, *lessons, *lesson;
//     int nextId_int = 1;

//     // TEMPORAIRE
//     int nb_lessons = 1000;

//     root = json_object();

//     // Creer la liste contenant les j+x a faire
//     days = json_array();
//     json_array_append(days, json_false());

//     // Creer le nextid
//     nextId = json_integer(nextId_int);
    
//     // Creer la liste des lecons
//     lessons = json_arraynext": [

//     json_object_set_new(root, "days", days);
//     json_object_set_new(root, "nextId", nextId);

//     for (int i=0; i<nb_lessons; i++) {
//         lesson = json_object();
        
//         // Id
//         json_object_set_new(lesson, "id", json_integer(nextId_int++));
//         // Nom
//         json_object_set_new(lesson, "name", json_string("Toto"));

//         json_array_append(lessons, lesson);
//     }

//     json_object_set_new(root, "lessons", lessons);
//     json_object_set(root, "nextId", json_integer(nextId_int));

//     *p_p_json = json_dumps(root, JSON_INDENT(4));
// }