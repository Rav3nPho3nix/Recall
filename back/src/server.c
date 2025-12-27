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

// Fonction du serveur
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

    // Contexte de l'application
    struct Context *context = malloc(sizeof(struct Context));
    context->json = json;
    context->time = t;
    context->file_path = argv[1];
    pthread_mutex_init(&(context->mutex), NULL);

    // Met en route le daemon du serveur
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL, &answer_to_connection, context, MHD_OPTION_END);

    if (NULL == daemon)
        return 1;

    // Ici afin que le serveur ne s'arrête pas
    getchar();
    MHD_stop_daemon(daemon);

    json_decref(json);
    return 0;
}

// Fonction de callback de microhttpd qui gere les requetes HTTP recues
enum MHD_Result answer_to_connection(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **req_cls) {
    char *response;
    struct MHD_Response *mhd_response;

    // Le contexte de la requete
    struct Context *context = (struct Context *) cls;

    const char *root_prefix = "/";

    const char *day_prefix = "/day/";
    const size_t day_prefix_length = strlen(day_prefix);
    
    const char *add_prefix = "/add";


    // "/" => l'application
    if (strcmp(url, root_prefix) == 0) {
        json_t *res = json_array();

        // Verrouille afin d'empecher l'acces concurrenciel
        pthread_mutex_lock(&context->mutex);
        get_lessons(context->json, mktime(context->time), res);
        pthread_mutex_unlock(&context->mutex);
        
        response = json_dumps(res, JSON_INDENT(3));
        enum MHD_Result ret = json_response_from_content(response, strlen(response), connection);
    
        free(response);
        json_decref(res);
        return ret;
    }

    // "/day/{number}" => les lecons dans {number} jours
    else if (strncmp(url, day_prefix, day_prefix_length) == 0) {
        // Chiffre après "/day/"
        const char *day_offset_str = url + day_prefix_length;
        int day_offset = atoi(day_offset_str);
        
        json_t *res = json_array();

        // Verrouille afin d'empecher l'acces concurrenciel
        pthread_mutex_lock(&context->mutex);

        // Copie la date actuelle
        struct tm t = *(context->time);
        t.tm_sec=0;
        t.tm_min=0;
        t.tm_hour=0;
        t.tm_mday += day_offset;

        get_lessons(context->json, mktime(&t), res);
        pthread_mutex_unlock(&context->mutex);
        
        response = json_dumps(res, JSON_INDENT(3));
        enum MHD_Result ret = json_response_from_content(response, strlen(response), connection);

        free(response);
        json_decref(res);
        return ret;
    }

    // Ajout d'une lecon
    else if (strcmp(url, add_prefix) == 0) {

        // Recuperation des informations via l'url
        const char *name = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "name");
        const char *subject = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "subject");
        const int number = atoi(MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "number"));
        
        // Creation d'un json afin de donner les informations en parametres
        json_t *root = json_object();
        json_object_set(root, "name", json_string(name));
        json_object_set(root, "subject", json_string(subject));
        json_object_set(root, "number", json_integer(number));

        // Copie la date actuelle
        struct tm t = *(context->time);
        t.tm_sec=0;
        t.tm_min=0;
        t.tm_hour=0;

        add_lesson(context->json, mktime(&t), root);
        printf("%s\n", json_dumps(context->json, JSON_INDENT(3)));
        save_json(context->file_path, context->json);

        response = "{\"code\": \"200\", \"add\": \"true\"}";
        mhd_response = MHD_create_response_from_buffer(strlen(response), (void *)response, MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(mhd_response, "Content-Type", "application/json");
        MHD_add_response_header(mhd_response, "Access-Control-Allow-Origin", "*");

        int ret = MHD_queue_response(connection, MHD_HTTP_OK, mhd_response);
        MHD_destroy_response(mhd_response);

        json_decref(root);
        return ret;
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

// Envoi une reponse HTTP au format JSON
int json_response_from_content(char *str, unsigned len, struct MHD_Connection *connection) {
    struct MHD_Response *mhd_response;
    int ret;

    mhd_response = MHD_create_response_from_buffer(len, (void *)str, MHD_RESPMEM_MUST_COPY);

    MHD_add_response_header(mhd_response, "Content-Type", "application/json");
    MHD_add_response_header(mhd_response, "Access-Control-Allow-Origin", "*");

    ret = MHD_queue_response(connection, MHD_HTTP_OK, mhd_response);
    MHD_destroy_response(mhd_response);

    return ret;
}

// Met a jour le contexte avec le nouveau contenu
void update_context(struct Context *context, json_t *json) {
    if (json != NULL) {
        context->json = json;
    }
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