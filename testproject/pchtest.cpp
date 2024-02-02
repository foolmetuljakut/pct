#include "pch.hpp"

int main(int argc, char **argv) {

    std::vector<int> intarray;
    for(int i = 0; i < 3; i++) {
        intarray.push_back(i);
        std::cout << intarray[i] << std::endl;
    }
    std::vector<float> farray;
    for(int i = 0; i < 3; i++) {
        farray.push_back(i);
        std::cout << farray[i] << std::endl;
    }

    return 0;
}

/*[Fr Feb 02][devsr][~/pmt/testproject][main|b0dc00a]$ g++ pch.hpp -fdiagnostics-color=always -Werror -g 
[Fr Feb 02][devsr][~/pmt/testproject][main|b0dc00a]$ g++ -o pchtest pchtest.cpp -H -fdiagnostics-color=always -Werror -g 
! pch.hpp.gch
 pchtest.cpp
[Fr Feb 02][devsr][~/pmt/testproject][main|b0dc00a]$ g++ -o pchtest pchtest.cpp -H -fdiagnostics-color=always -Werror -g -Winvalid-pch
! pch.hpp.gch
 pchtest.cpp
[Fr Feb 02][devsr][~/pmt/testproject][main|b0dc00a]$ ./pchtest 
0
1
2
0
1
2
[Fr Feb 02][devsr][~/pmt/testproject][main|b0dc00a]$ 
*/