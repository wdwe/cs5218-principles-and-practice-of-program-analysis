int main() {
    int a, b, c, d, e;
    while (a > 1) {
        a = a; // self assignment does not remove very busy expression
        if (c == 0) {
            d = a % b;
        } else {
            c = a % b;
        }
        d = a * e;
    }
    return 0;
}