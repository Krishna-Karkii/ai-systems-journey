#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {

// We make there systems call here
// Instruction one: Opening File
int fd = open("/tmp/file", O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
assert(fd > -1);

// Instruction two: Writing in file
int rc = write(fd, "hello world\n", 13);
assert(rc == 13);

// Instruction three: Closing file
close(fd);
return 0;

}