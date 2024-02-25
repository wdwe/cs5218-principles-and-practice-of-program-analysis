int main() {
    int i, j, sink, source, N;
    int a = 0;
    while (a < N) {
        if (a % 2 == 1)
            i = source;
        else
            j = i;
        a++;
    }
    sink = j;
    while (a < 2 * N) {
        if (a == 2 * N - 1)
            i = N + 1;
        else
            j = i;
    }
    // while.end10
    sink = j; // sink should be still tainted due to `may` analysis
}