

#ifndef CONFIG_EFTP_READLINE
#define read_line read
#define read_line_init()  do{}while(0)
#define read_line_exit()  do{}while(0)
#else

/*
 * Set read_line_prompt_string to the whole string displayed 
 * on the command line.
 */
extern char* read_line_prompt_string;

/*
 * Call this at the start of the program.
 */
void read_line_init (void);

/*
 * Call read_line_exit() to reset the terminal to its normal state.
 * This is done automatically at the end of the program, but if you want/need
 * to temporarily disable the read_line terminal modes, use read_line_exit()
 * (and read_line_init() to start the read_line terminal mode again).
 */
void read_line_exit (void);

/* 
 * A drop-in replacement for the read function,
 * with history and command line editing.
 */
ssize_t read_line (int fd, void *buf, size_t count);

#endif

