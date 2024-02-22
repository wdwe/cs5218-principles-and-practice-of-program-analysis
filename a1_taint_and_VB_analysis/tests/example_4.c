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
}