#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <unistd.h>
#endif
#ifdef __unix__
#include <getopt.h>
#endif
#include "hxd.h"

static void hxd_usage()
{
	printf("Usage: hxd [options] input_file\nOptions:\n");
	printf("\t-l   The number of bytes per line\n");
	printf("\t-g   The number of bytes per group\n");
	printf("\t-h   Show usage instructions\n");
	printf("\t-t   Find sections that match the given text\n");
	printf("\t-b   Find sections that match the given byte sequence\n");
	exit(EXIT_FAILURE);
}

int main(const int argc, char **argv)
{
	char *input_fp, *search_ptr;
	int opt, bytes_per_line = HXD_DEFAULT_BYTES_PER_LINE,
		bytes_per_group = HXD_DEFAULT_BYTES_PER_GROUP;
	enum hxd_session_type session_type = HXD_SESSION_TYPE_STD;

	if (argc < 2)
		hxd_usage();

	while((opt = getopt(argc, argv, "l:g:h:t:b:")) != -1) {
		switch (opt) {
			case 'l':
				bytes_per_line = atoi(optarg);
				break;
			case 'g':
				bytes_per_group = atoi(optarg);
				break;
			case 'h':
				hxd_usage();
				break;
			case 't':
				session_type = HXD_SESSION_TYPE_FIND_TEXT;
				search_ptr = optarg;
				break;
			case 'b':
				session_type = HXD_SESSION_TYPE_FIND_BYTES;
				search_ptr = optarg;
				break;
		}
	}

	input_fp = argv[argc -1];

	struct hxd_session session = hxd_create_session(session_type, input_fp, 
		bytes_per_line, bytes_per_group);
	switch (session_type) {
		case HXD_SESSION_TYPE_FIND_TEXT:
			hxd_process_text_search(&session, search_ptr);
			break;
		case HXD_SESSION_TYPE_FIND_BYTES:
			hxd_process_byte_search(&session, search_ptr);
			break;
		case HXD_SESSION_TYPE_STD:
		default:
			hxd_process_std(&session);
			break;
	}
	hxd_destroy_session(&session);
	return 0;
}
