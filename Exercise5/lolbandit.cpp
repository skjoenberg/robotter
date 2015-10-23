#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
using namespace std;

int main () {
    while (true) {
        int x, y;
        cin >> x >> y;
        double lol = atan((float)abs(x)/(float)abs(y));
        if (x > 0) {
            lol -= M_PI;
            if(y < 0) {
                lol = -lol;
            }
        }

        cout << lol * 180. / M_PI << endl;
    }
    return 0;
}
