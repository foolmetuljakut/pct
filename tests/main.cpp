#include <iostream>

#ifdef MAIN
int main(int argc, char **argv) {
    return 0;
}
#endif
#ifdef TEST1
int main(int argc, char **argv) {
    return 1;
}
#endif
#ifdef TEST2
int main(int argc, char **argv) {
    return 2;
}
#endif