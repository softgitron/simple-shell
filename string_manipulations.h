void clean_line(char *line);
void convert_to_arguments(char *string, char *args[], char *program);
int handle_relative_path(char *path, char *cwd, char *results);
void get_absolute_path(char *path, char *cwd, char *results);
void clean_trailing_slash(char *path);
int check_for_char(char *string, char character);