/* file: minunit.h */
#define mu_stringify(s) #s
#ifdef _MSC_VER
#define mu_fileline(line) __FILE__ "(" mu_stringify( line ) ")"
#define mu_assert(test) do { if (!(test)) return mu_fileline(__LINE__) ": error MU001: " mu_stringify(test)  "\n"; } while (0)
#define mu_run_test(test) do { const char *message = test(); \
    if (message) { fprintf(stderr, "%s", message);} } while (0)
#else
#define mu_fileline(line) __FILE__ ":" mu_stringify( line )
#define mu_assert(test) do { if (!(test)) return mu_fileline(__LINE__) ": assert: " mu_stringify(test)  "\n"; } while (0)
#define mu_run_test(test) do { const char *message = test(); \
    if (message) { fprintf(stderr, "%s", message);} } while (0)
#endif