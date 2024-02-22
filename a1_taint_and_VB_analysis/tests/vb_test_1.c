int main() {
    int a, b, x, y;
    if (a > b) {
        x = b - a;
        y = a - b;
    } else {
        y = b - a;
        x = a - b;
        return 1;
    }
    return 0;
}