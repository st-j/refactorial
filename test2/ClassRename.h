#ifndef CLASS_RENAME_H
#define CLASS_RENAME_H

#include <iostream>

namespace A {
  class XY {
  public:
    virtual ~XY() {}

    virtual void doFooter() = 0;
  };

  class Foo : public XY {
  protected:
    int x;
  public:
    Foo() : x(0) {}
    Foo(int px) : x(px) {}

    virtual ~Foo() {
    }

    int doSomeComplicatedStuff();

    virtual void doFooter() {
        std::cerr << "this is a string containing the word Foo and the word FooBar\n";
    }

    Foo getZeroFoo() {
      return Foo(0);
    }

    Foo *createNewFoo() {
      return new Foo(0);
    }

    int add(const Foo& f) {
      return x + f.x;
    }

    int sub(Foo *f) {
      return x - f->x;
    }

    int get() {
      return x;
    }
  };
};

namespace B {
  class Blah {
  protected:
    A::Foo f;
  public:
    Blah() : f(0) {}
    Blah(int x) : f(A::Foo(x)) {}
    const A::Foo* get() { return &f; }
    void set(A::Foo p = A::Foo(5)) {
      f = p;
    }
  };
};

namespace C {
  using namespace A;
  class ElNana {
  public:
    Foo f;
    int add(int x);
  };

  class ElNada {
  public:
    Foo *f;
    ElNada() : f(new Foo) {}
    ~ElNada() { delete f; }
  };
};

#endif /*CLASS_RENAME_H*/

