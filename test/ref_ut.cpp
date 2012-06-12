#include <iostream>
#include <proton/base.hpp>
#include <proton/ref.hpp>
#include <proton/detail/unit_test.hpp>
#include "pool_types.hpp"

using namespace std;
using namespace proton;

struct obj_test{
    string a;
    int b;
    obj_test()
    {}

    obj_test(const string& a1, int b1):a(a1),b(b1)
    {}

    virtual void output(ostream& s)const
    {
        s << a << ","<< b << std::endl;
    }
};

typedef ref_<obj_test> test;

struct obj_derived:obj_test{

    string c;

    obj_derived():obj_test()
    {}

    obj_derived(const string& a1, int b1, const string& c1):obj_test(a1,b1), c(c1)
    {}

    void output(ostream& s)const
    {
        s << a << ","<< b << "," << c << std::endl;
    }

    void copy_to(void* p)const
    {
        new (p) obj_derived(*this);
    }
};

typedef ref_<obj_derived> derived;

typedef ref_<obj_derived> der;

struct obj_de{
    string a;
    int b;
    string c;
    void copy_to(void* p)const
    {
        new (p) obj_de(*this);
    }
};
typedef ref_<obj_de> de;

int ref_ut()
{
    std::cout << "-> ref_ut" << std::endl;

    test t("c",10);
    std::cout << t;

    derived d("a",1,"daf"), e(copy(d));
    std::cout << d << e;

    bool k1=false;
#if 0
    try{
        e=t;
    }
    catch(err&){
        k1=true;
    }
    PROTON_THROW_IF(!k1, "no cast err detected!");
#endif

    der f(alloc);
    f->a="abc"; f->b=2; f->c="def";
    std::cout << f->a << ", " << f->b << ", " << f->c << std::endl;

    de g(alloc);
    g->a="dkf"; g->b=3; g->c="dfe";
    de j(g), k(copy(g));
    std::cout << g->a << ", " << g->b << ", " << g->c << std::endl;
    std::cout << j->a << ", " << j->b << ", " << j->c << std::endl;
    std::cout << k->a << ", " << k->b << ", " << k->c << std::endl;

    return 0;
}

volatile int refc_count=0;

struct obj_refc_test{
    int a;
    obj_refc_test(int x):a(x)
    {
        refc_count++;
    }

    obj_refc_test():a(0)
    {
        refc_count++;
    }

    ~obj_refc_test()
    {
        refc_count--;
    }
};

typedef ref_<obj_refc_test> ref_test;

int ref_test_ut()
{
    std::cout << "-> ref_test_ut" << std::endl;
    {
        tvector(ref_test) as;
        for(int i=0; i<10;i++){
            ref_test a(2);
            as.push_back(a);
        }
        //std::cout << refc_count << std::endl;
        PROTON_THROW_IF(refc_count!=10, "bad refc_count");
        ref_test b;
        b=as[0];
        as.clear();
        //std::cout << refc_count << std::endl;
        PROTON_THROW_IF(refc_count!=1, "bad refc_count");
        reset(b);
        PROTON_THROW_IF(refc_count!=0, "bad refc_count");
    }
    //std::cout << "refc_count:"<<refc_count << std::endl;
    return 0;
}

int reset_ut()
{
    cout << "-> reset_ut" << endl;
    test t(alloc);
    PROTON_THROW_IF(is_null(t), "err");
    PROTON_THROW_IF(ref_count(t)!=1, "err");
    reset(t);
    PROTON_THROW_IF(is_valid(t), "err");
    PROTON_THROW_IF(ref_count(t)!=0, "err");
    PROTON_THROW_IF(&t.__o()!=NULL, "err");

    return 0;
}

int main()
{
    proton::debug_level=1;
    proton::wait_on_err=0;
    std::vector<proton::detail::unittest_t> ut=
        {ref_ut, ref_test_ut, reset_ut};
    proton::detail::unittest_run(ut);
    return 0;
}
