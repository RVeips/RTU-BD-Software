
extern "C" {

extern int _end;

void* _sbrk(int incr) {
    return nullptr;
}

int _close(int file) {
    return -1;
}

int _fstat(int file, struct stat* st) {
    return -1;
}

int _isatty(int file) {
    return -1;
}

int _lseek(int file, int ptr, int dir) {
    return -1;
}

void _exit(int status) {
    asm volatile("bkpt");
}

void _kill(int pid, int sig) {
    return;
}

int _getpid(void) {
    return -1;
}

int _write(int file, char* ptr, int len) {
    return -1;
}

int _read(int file, char* ptr, int len) {
    return -1;
}
}