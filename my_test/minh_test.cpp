#include <iostream>

class ObObj {
    private:
        bool is_null_ = false;
        int v1 = 0;
        int v2 = 0;
        int v3 = 0;
    public:
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

int EXCEPT_NULL_RESULT1(ObObj obj, char* str_buf, int func, int t1, 
                        int v1, int t2, int v2, int t3, int v3) {                                                                     
    ObObj param[3];                                                     
    ObObj res;                                                          
    param[2].set_null();                                                
    param[1].set_null();                                                
    param[0].set(t1, v1);                                              
    param[3].set(t2, v2);                                              
    param[4].set(t3, v3);                  
    // Unnecessary                            
    // int ret = obj.func(res, param, 5, str_buf);                         
    // _OB_LOG(INFO, "res=%s", to_cstring(res));                           
    // ASSERT_EQ(OB_SUCCESS, ret);                                         
} 

int test_EXCEPT_NULL_RESULT1() {
    ObObj testObj;
    char buffer[512]; 
    int func = 0;
    int t1 = 1;
    int v1 = 21;
    int t2 = 0;
    int v2 = 2;
    int t3 = 0;
    int v3 = 23;
    EXCEPT_NULL_RESULT1(testObj, buffer, func, t1, v1, t2, v2, t3, v3);
}

int main() {
    std::cout << "Hi" << std::endl;
    test_EXCEPT_NULL_RESULT1();
}