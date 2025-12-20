#ifndef JSON_H
#define JSON_H

// Charge les lessons
void load_json(char *file_name, json_t **p_json);

// Donne les lecons du jour
void get_lessons_today(json_t *json, time_t today, json_t *lessons_today);

// Met a jour le json
void update_lessons(json_t *json, time_t today);

// Ajoute une lecon
void add_lesson(json_t *json, time_t today, int id, char *name, char *subject, char *number);

// Donne les lecons pour le jour n (a utiliser pour remplacer get_lessons_today)
void get_lessons(json_t *json, time_t day, json_t *lessons);

#endif