int main() {
    int x, y, z;
    if (y > 0) {
        x = 10;
    } else {
        x = 5;
    }
    // x = [5, 10]
    if (y < x)
        z = y; // z = y = [-inf, 9]
    else
        z = x - 2; // z = [3, 8]
    return 0;
}