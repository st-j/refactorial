#ifndef FUNCTION_RENAME_H
#define FUNCTION_RENAME_H

#include "ClassRename.h"

namespace SampleNameSpace {
  class Foo {
  private:
    static int counter;
    int x;
    Foo *next;
    bool ownsNext;
  public:
    Foo() : x(0), next(0), ownsNext(false) {}
    Foo(Foo *n) : x(0), next(n), ownsNext(false) {}
    Foo(int px) : x(px), next(new Foo()), ownsNext(true) {}

    A::Foo convoluted_member_variable;

    ~Foo() {
      if (ownsNext) {
        delete next;
      }
    }

    int getX() const;
    const Foo* getNext() const { return next; }
    void setX(int newX) {
      x = newX;
    }

    virtual void cycleWasteTest();
    void doNothing();
    void doMore();
  };

  inline int Foo::getX() const {
    return x;
  }
};

#endif /*FUNCTION_RENAME_H*/

