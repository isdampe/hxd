#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "hxd.h"

struct hxd_session hxd_create_session(const char *input_fp, 
	const int bytes_per_line, const int bytes_per_group)
{
	struct hxd_session session = {
		.input_fp = NULL,
		.bytes_per_line = bytes_per_line,
		.bytes_per_group = bytes_per_group,
		.file_size = 0x0
	};

	session.input_fp = malloc((strlen(input_fp) + 1) * sizeof(char));
	assert(session.input_fp != NULL);
	if (session.input_fp == NULL) {
		fprintf(stderr, "Could not allocate memory for session input file path.\n");
		exit(EXIT_FAILURE);
	}

	strcpy(session.input_fp, input_fp);

	return session;
}

void hxd_destroy_session(struct hxd_session *session)
{
	assert(session->input_fp != NULL);
	if (session->input_fp != NULL)
		free(session->input_fp);
}

void hxd_process(struct hxd_session *session)
{
	uint8_t input_buffer[session->bytes_per_line];
	FILE *fh = fopen(session->input_fp, "r");
	if (fh == NULL) {
		fprintf(stderr, "There was an error opening %s.\n", session->input_fp);
		return;
	}

	fseek(fh, 0x0, SEEK_END);
	session->file_size = ftell(fh);
	rewind(fh);

	int bytes_read = 0x0;
	int offset = 0x0;
	while (offset < session->file_size) {
		bytes_read = fread(input_buffer, 1, session->bytes_per_line, fh);
		render_line(bytes_read, input_buffer, offset, session);
		offset += bytes_read;
		fseek(fh, offset, SEEK_SET);
	}

	
	fclose(fh);
}

static void render_line(const int bytes_read, uint8_t *input_buffer, const int 
	offset, struct hxd_session *session)
{
	char b[9];
	sprintf(b, "0x%%0%ix  ", (int)(log(session->file_size) / log(16)) +2);
	b[8] = '\0';

	printf(HXD_COLOR_CYAN);
	printf(b, offset);
	for (int i=0; i < bytes_read; ++i) {
		render_byte(input_buffer[i]);
		if ((i + 1) % session->bytes_per_group == 0)
			printf(" ");
	}
	printf(" ");

	for (int i=0; i < bytes_read; ++i) {
		render_ascii(input_buffer[i]);
	}

	printf("\n");

}

static inline void render_byte(const uint8_t b)
{
	if (b == 0x0) {
		printf(HXD_COLOR_MAGENTA "%02x", b);
	} else if (b >= 9 && b <= 13) {
		printf(HXD_COLOR_BLUE "%02x", b);
	} else if (b >=33 && b <= 126) {
		printf(HXD_COLOR_YELLOW "%02x", b);
	} else {
		printf(HXD_COLOR_RED "%02x", b);
	}
}

static inline void render_ascii(const uint8_t b) 
{
	if (b == 0x0) {
		printf(HXD_COLOR_MAGENTA ".");
	} else if (b >= 9 && b <= 13) {
		printf(HXD_COLOR_BLUE ".");
	} else if (b >=33 && b <= 126) {
		printf(HXD_COLOR_YELLOW "%c", b);
	} else {
		printf(HXD_COLOR_RED ".");
	}
}
