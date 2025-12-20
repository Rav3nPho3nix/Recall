#ifndef SERVER_H
#define SERVER_H

#define PORT 8888

struct Context {
    // Le fichier json
    json_t *json;
    // La date d'aujourd'hui
    struct tm *time;
    // Nom du fichier json
    char *file_name;
    // Protection afin d'interdire l'acces en parralle au contexte
    pthread_mutex_t mutex;
};

enum MHD_Result answer_to_connection(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **req_cls);
int json_response_from_content(char *str, unsigned len, struct MHD_Connection *connection);

#endif