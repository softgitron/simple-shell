int create_pipes(Process *process);
void connect_pipes_child(Process *process);
void pipe_to_std(int pipe, int destination);