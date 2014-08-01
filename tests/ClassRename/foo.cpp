#include "foo.h"
#include <iostream>
#include <vector>

int C::ElNana::add(int x) {
  A::Foobar p(x);
  return f.add(p.get());
}

#define FX(x)   A::Foo x;

typedef std::vector<const A::Foobar*> fvcp;
typedef std::vector<A::Foobar*> fvp;

A::Foobar test(A::Foobar &a) {
  A::Foobar b = a;
  return b;
}

int main()
{
  C::ElNana n;
  std::cout << n.add(10);

  A::Foobar *f = new A::Foobar();

  A::Foobar a;
  std::vector<A::Foobar> vf;
  vf.push_back(a);

  // FX(b);
  // vf.push_back(b);

  fvcp c;
  c.push_back(f);

  fvp d;
  d.push_back(f);

  delete f;

  return 0;
}
