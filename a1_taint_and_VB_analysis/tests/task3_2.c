int main() {
    // entry
    int i, j, k, sink, source = 1234567;
    if (j > 1)
        // if.then
        i = source;
    else
        // if.else
        k = i;
    // if.end
    if (k > 0)
        // if.then2
        j = i;
    else
        // if.else3
        j = k;
    // if.end4
    sink = k + j; // expected sink {entry->if.then->if.end->if.then2->if.end4}
}