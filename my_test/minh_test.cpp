#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <stdarg.h>

class ObObj {
    public:
        bool is_null_ = false;
        int v1 = 0;
        int v2 = 0;
        int v3 = 0;
        void set(int t, int v) {
            if(t == 1)
                v1 = v;
            else if(t == 2)
                v2 = v;
            else if(t == 3)
                v3 = v;
        };   
        void set_null() {
            is_null_ = true;
        };
};

int EXCEPT_NULL_RESULT1(ObObj obj, char* str_buf, int func, int t1, int v1,
                        int t2, int v2, int t3, int v3) {       
    ObObj param[3]; 
    ObObj res;          
    // error                                         
    param[2].set_null();                                                
    param[1].set_null();                                                
    param[0].set(t1, v1);                                              
    param[3].set(t2, v2);                                              
    param[4].set(t3, v3);
    std::cout << "Object 0: " << param[0].v1 << " " << param[0].v2 << " " << param[0].v3 << std::endl;  
    std::cout << "Object 1: " << param[1].v1 << " " << param[1].v2 << " " << param[1].v3 << std::endl;  
    std::cout << "Object 2: " << param[2].v1 << " " << param[2].v2 << " " << param[2].v3 << std::endl;  
    std::cout << "Object 3: " << param[3].v1 << " " << param[3].v2 << " " << param[3].v3 << std::endl;  
    std::cout << "Object 4: " << param[4].v1 << " " << param[4].v2 << " " << param[4].v3 << std::endl;

    // unnecessary                            
    // int ret = obj.func(res, param, 5, str_buf);                         
    // _OB_LOG(INFO, "res=%s", to_cstring(res));                           
    // ASSERT_EQ(OB_SUCCESS, ret);                                         
}

int EXCEPT_NULL_RESULT1_FIX(ObObj obj, char* str_buf, const int range, int func, int table[][4]) {       
    ObObj param[range];                                                     
    ObObj res;          
    int object_range = 4;

    for(int i = 0; i < range; i ++) {
        for(int j = 0; j < object_range; j++) {
            if(j == 0) {
                if(table[i][j])
                    param[i].set_null();
            } 
            else
                param[i].set(j, table[i][j]);
        }
    }

    std::cout << "Object 0: " << param[0].v1 << " " << param[0].v2 << " " << param[0].v3 << std::endl;  
    std::cout << "Object 1: " << param[1].v1 << " " << param[1].v2 << " " << param[1].v3 << std::endl;  
    std::cout << "Object 2: " << param[2].v1 << " " << param[2].v2 << " " << param[2].v3 << std::endl;

    // unnecessary                            
    // int ret = obj.func(res, param, 5, str_buf);                         
    // _OB_LOG(INFO, "res=%s", to_cstring(res));                           
    // ASSERT_EQ(OB_SUCCESS, ret);                                         
} 

int test_EXCEPT_NULL_RESULT1() {
    ObObj testObj;
    char buffer[10]; 
    int func = 0;
    int range = 3;
    int object_range = 4; // we only have 4 attributes for ObObj
    int table[3][4];
    for(int i = 0; i < range; i ++) {
        for(int j = 0; j < object_range; j++) {
            if(j == 0) // first index will determine if we need to set_null
                table[i][j] = rand() % 2;
            else
                table[i][j] = rand() % 10 + 1;
        }
    }
    EXCEPT_NULL_RESULT1_FIX(testObj, buffer, range, func, table);
}

//-------------------------------------------------------------------------------//
constexpr int OB_SUCCESS = 0;

int ob_alloc_printf(char *result, char *alloc, const char* fmt, va_list ap)
{
    int ret = OB_SUCCESS;
    va_list ap2;
    // unnecessary 
    //   va_copy(ap2, ap);
    //   int64_t n = vsnprintf(NULL, 0, fmt, ap);
    //   if (n < 0) {
    //     LOG_ERROR("vsnprintf failed", K(n), K(errno));
    //     ret = OB_ERR_SYS;
    //   } else {
    //     char* buf = static_cast<char*>(alloc.alloc(n + 1));
    //     if (NULL == buf) {
    //       ret = OB_ALLOCATE_MEMORY_FAILED;
    //       LOG_ERROR("no memory");
    //     } else {
    //       int64_t n2 = vsnprintf(buf, n + 1, fmt, ap2);
    //       if (n2 < 0) {
    //         LOG_ERROR("vsnprintf failed", K(n), K(errno));
    //         ret = OB_ERR_SYS;
    //       } else if (n != n2) {
    //         LOG_ERROR("vsnprintf failed", K(n), K(n2));
    //         ret = OB_ERR_SYS;
    //       } else {
    //         result.assign_ptr(buf, static_cast<int32_t>(n));
    //       }
    //     }
    //   }
    // va_end(ap2);
    // va_end(ap);
    return ret;
}

int test_list_not_closed() {
    char result[] = "ABCD";
    char alloc[] = "123";
    const char fmt[] = "A";
    va_list ap;
    ob_alloc_printf(result, alloc, fmt, ap);

}

void TestPrint(char* format, ...)
{
    va_list argList;

    va_start(argList, format);
    vprintf(format, argList);
    va_end(argList);
}

//-------------------------------------------------------------------------------//
int main() {
    // test_EXCEPT_NULL_RESULT1();
    test_list_not_closed();
    TestPrint("Test print %s %d\n", "string", 55);
    return 0;
}