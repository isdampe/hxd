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
	printf("\t-l  The number of bytes per line\n");
	printf("\t-g  The number of bytes per group\n");
	printf("\t-h  Show usage instructions\n");
	exit(EXIT_FAILURE);
}

int main(const int argc, char **argv)
{
	char *input_fp;
	int opt, bytes_per_line = HXD_DEFAULT_BYTES_PER_LINE,
		bytes_per_group = HXD_DEFAULT_BYTES_PER_GROUP;

	if (argc < 2)
		hxd_usage();

	while((opt = getopt(argc, argv, "l:g:h:")) != -1) {
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
		}
	}

	input_fp = argv[argc -1];

	struct hxd_session session = hxd_create_session(input_fp, bytes_per_line,
		bytes_per_group);
	hxd_process(&session);
	hxd_destroy_session(&session);
	return 0;
}
