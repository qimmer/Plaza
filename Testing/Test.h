#ifndef TEST_H
#define TEST_H


    typedef const char * TestResult;
}

#define Verify(message, test) if (!(test)) { static char buf[1024]; sprintf(buf, "%s (%s : %d)", message, __FILE__, __LINE__); return buf; } while (0)

#define Success 0

#define TestMain(...) \
    int main(int argc, char** argv) { \
        TestResult none = (TestResult)0xffffffff; \
        TestResult results[] = { __VA_ARGS__, none };\
        const char *names[] = { #__VA_ARGS__ }; \
        int tests_passed = 0; \
        int i = 0; \
        printf("Testing ...\n\n");\
        for(; results[i] != none; ++i) { \
           if(results[i]) printf("Test '%s' failed: %s\n", names[i], results[i]); else tests_passed++; \
        } \
        printf("%d Tests Passed\n%d Tests Failed", tests_passed, i - tests_passed); \
    }

#endif