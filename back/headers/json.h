#ifndef JSON_H
#define JSON_H

void load_json(char *file_name, json_t **p_json);
// json_t* get_lessons_today(json_t *json, time_t today);
void get_lessons_today(json_t *json, time_t today, json_t *lessons_today);

#endif