int main() {
    int i, j, k, source, sink;
    if (i > 0)
        j = source;
    else {
        k = j;
    }
    sink = j; // sink tainted
    if (i > 1)
        sink = k;
    else
        sink = k;
    // sink untainted
}