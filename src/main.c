#include <stdio.h>
#include "hxd.h"

int main(const int argc, const char **argv)
{
	struct hxd_session session = hxd_create_session("./bin/hxd", 32, 4);
	hxd_process(&session);
	hxd_destroy_session(&session);
	return 0;
}
