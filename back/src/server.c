#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>

#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include <time.h>

#include "../libs/jansson.h"
#include "../headers/json.h"
#include "../headers/server.h"

int main(int argc, char *argv[]) {
    // Verifie qu'il y a bien un argument
    if (argc == 1) {
        printf("The name of the json file is needed, exiting...\n");
        return EXIT_FAILURE;
    }

    // Charge le json en memoire
    json_t *json = json_object();
    load_json(argv[1], &json);
    printf("- Json file loaded successfully\n");

    // Date d'ajourd'hui
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    t->tm_sec=0;
    t->tm_min=0;
    t->tm_hour=0;

    struct Context *p_context = malloc(sizeof(struct Context));
    p_context->json = json;
    p_context->time = t;
    p_context->file_name = argv[1];
    pthread_mutex_init(&(p_context->mutex), NULL);

    // Met en route le daemon du serveur
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL, &answer_to_connection, p_context, MHD_OPTION_END);

    if (NULL == daemon)
        return 1;

    // Ici afin que le serveur ne s'arrête pas dès le début
    getchar();
    MHD_stop_daemon(daemon);

    return 0;
}


enum MHD_Result answer_to_connection(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **req_cls) {
    char *response;
    struct MHD_Response *mhd_response;

    // Le contexte de la requete
    struct Context *context = (struct Context *) cls;
    // json_t *json = context->json;
    // struct tm *t = context->time;
    // char *file_name = context->file_name;

    // "/" => l'application
    if (strcmp(url, "/") == 0) {
        json_t *res = json_array();

        // Verrouille afin d'empecher l'acces concurrenciel
        pthread_mutex_lock(&context->mutex);
        get_lessons_today(context->json, mktime(context->time), res);
        pthread_mutex_unlock(&context->mutex);
        
        response = json_dumps(res, JSON_INDENT(3));
        printf("%s", response);
        return json_response_from_content(response, strlen(response), connection);
    }

    // Sinon reponse code 404
    else {
        response = "{\"code\": \"404\"}";
        mhd_response = MHD_create_response_from_buffer(strlen(response), (void *)response, MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(mhd_response, "Content-Type", "application/json");
        MHD_add_response_header(mhd_response, "Access-Control-Allow-Origin", "*");

        int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, mhd_response);
        MHD_destroy_response(mhd_response);

        return ret;
    }
}

int json_response_from_content(char *str, unsigned len, struct MHD_Connection *connection) {
    struct MHD_Response *mhd_response;
    int ret;

    mhd_response = MHD_create_response_from_buffer(len, (void *)str, MHD_RESPMEM_PERSISTENT);

    MHD_add_response_header(mhd_response, "Content-Type", "application/json");
    MHD_add_response_header(mhd_response, "Access-Control-Allow-Origin", "*");

    ret = MHD_queue_response(connection, MHD_HTTP_OK, mhd_response);
    MHD_destroy_response(mhd_response);

    return ret;
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