#ifndef HXD_H
#include <stdint.h>
#define HXD_H

#define HXD_DEFAULT_BYTES_PER_LINE 0x16
#define HXD_DEFAULT_BYTES_PER_GROUP 0x8

#define HXD_COLOR_RED     "\x1b[31m"
#define HXD_COLOR_GREEN   "\x1b[32m"
#define HXD_COLOR_YELLOW  "\x1b[33m"
#define HXD_COLOR_BLUE    "\x1b[34m"
#define HXD_COLOR_MAGENTA "\x1b[35m"
#define HXD_COLOR_CYAN    "\x1b[36m"
#define HXD_COLOR_RESET   "\x1b[0m"

struct hxd_session {
	char *input_fp;
	int bytes_per_line;
	int bytes_per_group;
	int file_size;
};

struct hxd_session hxd_create_session(const char *input_fp, 
	const int bytes_per_line, const int bytes_per_group);
void hxd_destroy_session(struct hxd_session *session);
void hxd_process(struct hxd_session *session);
static void render_line(const int bytes_read, uint8_t *input_buffer, const int 
	offset, struct hxd_session *session);
static inline void render_byte(const uint8_t b);
static inline void render_ascii(const uint8_t b);

#endif
