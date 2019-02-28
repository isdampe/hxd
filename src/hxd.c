#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "hxd.h"

struct hxd_session hxd_create_session(enum hxd_session_type session_type,
	const char *input_fp, const int bytes_per_line, const int bytes_per_group)
{
	struct hxd_session session = {
		.session_type = session_type,
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

void hxd_process_std(struct hxd_session *session)
{
	process_output(session, 0x0, -1, HXD_NO_HIGHLIGHT, HXD_NO_HIGHLIGHT);
}

static void process_output(struct hxd_session *session, const int offset_start, 
	int max_length, const int highlight_start, const int highlight_end)
{
	uint8_t input_buffer[session->bytes_per_line];
	FILE *fh = fopen(session->input_fp, "r");
	assert(fh != NULL);
	if (fh == NULL) {
		fprintf(stderr, "There was an error opening %s.\n", session->input_fp);
		exit(EXIT_FAILURE);
	}

	fseek(fh, 0x0, SEEK_END);
	session->file_size = ftell(fh);
	rewind(fh);

	if (offset_start > session->file_size) {
		fprintf(stderr, "The file size is not big enough to begin dumping from that offset.\n");
		fclose(fh);
		exit(EXIT_FAILURE);
	}

	if (max_length < 0)
		max_length = session->file_size;

	int bytes_read = 0x0;
	int offset = offset_start;
	fseek(fh, offset, SEEK_SET);

	while (offset < session->file_size && bytes_read < max_length) {
		bytes_read = fread(input_buffer, 1, session->bytes_per_line, fh);
		render_line(bytes_read, input_buffer, offset, session, highlight_start, highlight_end);
		offset += bytes_read;
		fseek(fh, offset, SEEK_SET);
	}

	printf(HXD_COLOR_RESET);
	
	fclose(fh);
}

void hxd_process_text_search(struct hxd_session *session, const char *subject)
{
	uint8_t input_buffer[2];
	FILE *fh = fopen(session->input_fp, "r");
	if (fh == NULL) {
		fprintf(stderr, "There was an error opening %s.\n", session->input_fp);
		exit(EXIT_FAILURE);
	}

	fseek(fh, 0x0, SEEK_END);
	session->file_size = ftell(fh);
	rewind(fh);

	int offset = 0x0, bytes_read = 0x0;
	int search_idx = 0x0, search_end = strlen(subject);

	while (offset < session->file_size) {
		bytes_read = fread(&input_buffer, 1, 1, fh);
		offset += bytes_read;

		if (input_buffer[0] == subject[search_idx]) {
			search_idx++;
			if (search_idx >= search_end) {
				process_output(session, offset - (2 * search_end), (2 * search_end), search_idx,
					(search_idx + search_end -1));
				search_idx = 0;
			}
		} else {
			search_idx = 0;
		}

		fseek(fh, offset, SEEK_SET);
	}

	printf(HXD_COLOR_RESET);
	
	fclose(fh);
}

void hxd_process_byte_search(struct hxd_session *session, const char *subject)
{
	if (subject == NULL) {
		fprintf(stderr, "No string of bytes was provided.\n");
		exit(EXIT_FAILURE);
	}

	size_t slength = strlen(subject);
	if ((slength % 2) != 0) {
		fprintf(stderr, "You must provide an even number of characters in the byte sequence.\n");
		exit(EXIT_FAILURE);
	}


	size_t dlength = slength / 2;

	uint8_t data[dlength];
	memset(data, 0, dlength);

	size_t index = 0;
	while (index < slength) {
		char c = subject[index];
		int value = 0;
		if (c >= '0' && c <= '9')
			value = (c - '0');
		else if (c >= 'A' && c <= 'F')
			value = (10 + (c - 'A'));
		else if (c >= 'a' && c <= 'f')
			value = (10 + (c - 'a'));
		else {
			fprintf(stderr, "An invalid byte string was provided.\n");
			exit(EXIT_FAILURE);
		}

		data[(index / 2)] += value << (((index + 1) % 2) * 4);

		index++;
	}

	uint8_t input_buffer[2];
	FILE *fh = fopen(session->input_fp, "r");
	if (fh == NULL) {
		fprintf(stderr, "There was an error opening %s.\n", session->input_fp);
		exit(EXIT_FAILURE);
	}

	fseek(fh, 0x0, SEEK_END);
	session->file_size = ftell(fh);
	rewind(fh);

	int offset = 0x0, bytes_read = 0x0;
	int search_idx = 0x0, search_end = dlength;

	while (offset < session->file_size) {
		bytes_read = fread(&input_buffer, 1, 1, fh);
		offset += bytes_read;

		if (input_buffer[0] == data[search_idx]) {
			search_idx++;
			if (search_idx >= search_end) {
				process_output(session, offset - (2 * search_end), (2 * search_end), search_idx,
						(search_idx + search_end -1));
				search_idx = 0;
			}
		} else {
			search_idx = 0;
		}

		fseek(fh, offset, SEEK_SET);
	}

	printf(HXD_COLOR_RESET);

	fclose(fh);


}

static void render_line(const int bytes_read, uint8_t *input_buffer, const int 
		offset, struct hxd_session *session, const int highlight_start, const int highlight_end)
{
	char b[9];
	sprintf(b, "0x%%0%ix  ", (int)(log(session->file_size) / log(16)) +2);
	b[8] = '\0';

	printf(HXD_COLOR_CYAN);
	printf(b, offset);
	for (int i=0; i < session->bytes_per_line; ++i) {
		if (i < bytes_read)
			render_byte(input_buffer[i], (i >= highlight_start && i <= highlight_end ? true : false));
		else
			printf("  ");

		if ((i + 1) % session->bytes_per_group == 0)
			printf(" ");
	}
	printf(" ");

	for (int i=0; i < session->bytes_per_line; ++i) {
		if (i < bytes_read)
			render_ascii(input_buffer[i], (i >= highlight_start && i <= highlight_end ? true : false));
		else
			printf(" ");
	}

	printf("\n");

}

static inline void render_byte(const uint8_t b, bool highlight)
{
	if (highlight) {
		printf(HXD_BG_COLOR_GREEN);
		printf(HXD_COLOR_BLACK "%02x", b);
	} else if (b == 0x0) {
		printf(HXD_COLOR_MAGENTA "%02x", b);
	} else if (b >= 9 && b <= 13) {
		printf(HXD_COLOR_BLUE "%02x", b);
	} else if (b >=33 && b <= 126) {
		printf(HXD_COLOR_YELLOW "%02x", b);
	} else {
		printf(HXD_COLOR_RED "%02x", b);
	}
	printf(HXD_BG_COLOR_RESET);
}

static inline void render_ascii(const uint8_t b, bool highlight) 
{
	if (highlight) { 
		printf(HXD_BG_COLOR_GREEN);
		if (b >=33 && b <= 126)
			printf(HXD_COLOR_BLACK "%c", b);
		else
			printf(HXD_COLOR_BLACK ".");
		return;
	}
	
	printf(HXD_BG_COLOR_RESET);
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
