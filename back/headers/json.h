#ifndef JSON_H
#define JSON_H

// Charge les lecons
void load_json(char *file_name, json_t **p_json);

// Enregistre les lecons
void save_json(char *file_name, json_t *json);

// Met a jour le json
void update_lessons(json_t *json, time_t today);

// Ajoute une lecon
void add_lesson(json_t *json, time_t today, json_t *values);

// Donne les lecons pour le jour n
void get_lessons(json_t *json, time_t day, json_t *lessons);

#endif