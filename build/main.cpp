#include "iostream"

#ifdef MAIN
int main(int argc, char **argv) {
    std::cout << "main app";
    return 0;
}
#endif

#ifdef TESTKEY1
int main(int argc, char **argv) {
    std::cout << "ut app 1";
    return 0;
}
#endif

#ifdef TESTKEY2
int main(int argc, char **argv) {
    std::cout << "ut app 2";
    return 0;
}
#endif

#ifdef TESTKEY3
int main(int argc, char **argv) {
    std::cout << "ut app 3";
    return 0;
}
#endif

#ifdef TESTKEY4
int main(int argc, char **argv) {
    std::cout << "ut app 4";
    return 0;
}
#endif

#ifdef TESTKEY5
int main(int argc, char **argv) {
    std::cout << "ut app 5";
    return 0;
}
#endif