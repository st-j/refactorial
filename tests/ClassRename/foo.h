#ifndef FOO_H
#define FOO_H

// #include <string>

namespace A {
  class XY {
  public:
    virtual ~XY() {}
  };

  class Foobar : public XY {
  protected:
    int x;
  public:
    Foobar() : x(0) {}
    Foobar(int px) : x(px) {}

    virtual ~  Foobar() {


    }

    Foobar getZeroFoo() {
      return Foobar(0);
    }

    Foobar *createNewFoo() {
      return new Foobar(0);
    }

    int add(const Foobar& f) {
      return x + f.x;
    }

    int sub(Foobar *f) {
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
    A::Foobar f;
  public:
    Blah() : f(0) {}
    Blah(int x) : f(A::Foobar(x)) {}
    const A::Foobar* get() { return &f; }
    void set(A::Foobar p = A::Foobar(5)) {
      f = p;
    }
  };
};

namespace C {
  using namespace A;
  class ElNana {
  public:
    Foobar f;
    int add(int x);
  };

  class ElNada {
    Foobar *f;
  public:
    ElNada() : f(new Foobar) {}
    ~ElNada() { delete f; }
  };
};

#endif /*FOO_H*/
