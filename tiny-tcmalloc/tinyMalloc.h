
class TinyMalloc
{
public:
    static void* malloc(long long size);
    static void free(void* p);
};