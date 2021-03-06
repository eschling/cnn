#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "CNNEdges"
#include <boost/test/unit_test.hpp>

#include <vector>

#include "cnn/tests/test_utils.h"
#include "cnn/tensor.h"
#include "cnn/edges.h"

using namespace std;
using namespace cnn;

BOOST_GLOBAL_FIXTURE(TestTensorSetup)

BOOST_AUTO_TEST_CASE(ESqrL2)
{
  Tensor U = Ccm({2,1}, {4,5});
  Tensor V = Ccm({2,1}, {1,1});
  cerr << U << endl;
  SquaredEuclideanDistance e;
  vector<const Tensor*> xs = {&U, &V};
  Tensor W = e.forward(xs); 
  cerr << "Norm^2:" << W << endl;
  double eps = 1e-5;
  BOOST_CHECK_CLOSE(t(W,0,0),25., eps);
  Tensor dEdf = Ccm({1,1}, {1});
  Tensor d1 = e.backward(xs, W, dEdf, 0);
  Tensor d2 = e.backward(xs, W, dEdf, 1);
  cerr << d1 << endl;
  cerr << d2 << endl;
  BOOST_CHECK_CLOSE(t(d1,0,0), 6., eps);
  BOOST_CHECK_CLOSE(t(d1,1,0), 8., eps);
  BOOST_CHECK_CLOSE(t(d2,0,0), -6., eps);
  BOOST_CHECK_CLOSE(t(d2,1,0), -8., eps);
}

BOOST_AUTO_TEST_CASE(EMatrixMultiply) {
  Tensor U = Ccm({2,3}, {1,2,3,4,5,6});
  Tensor V = Ccm({3,2}, {7,8,9,10,11,12});
  MatrixMultiply mm;
  vector<const Tensor*> xs = {&U, &V};
  Tensor W = mm.forward(xs);
  BOOST_REQUIRE_EQUAL(Dim({2,2}),size(W));
  double eps = 1e-5;
  BOOST_CHECK_CLOSE(t(W,0,0), 76., eps);
  BOOST_CHECK_CLOSE(t(W,1,0), 100., eps);
  BOOST_CHECK_CLOSE(t(W,0,1), 103., eps);
  BOOST_CHECK_CLOSE(t(W,1,1), 136., eps);
  cerr << U << endl;
  cerr << V << endl;
  cerr << W << endl;
}

BOOST_AUTO_TEST_CASE(EColumnConcat)
{
  Tensor u1 = Ccm({2,1}, {1, 2});
  Tensor u2 = Ccm({2,1}, {3, 4});
  Tensor u3 = Ccm({2,1}, {5, 6});
  cerr << u1 << endl;
  cerr << u2 << endl;
  cerr << u3 << endl;
  vector<const Tensor*> xs = {&u1, &u2, &u3};
  ConcatenateColumns cc;
  Tensor U = cc.forward(xs);
  cerr << U << endl;
  Tensor V = Ccm({3,2}, {7,8,9,10,11,12});
  MatrixMultiply mm;
  vector<const Tensor*> xs2 = {&U, &V};
  Tensor W = mm.forward(xs2);
  cerr << W << endl;
  BOOST_REQUIRE_EQUAL(Dim({2,2}),size(W));
  double eps = 1e-5;
  BOOST_CHECK_CLOSE(t(W,0,0), 76., eps);
  BOOST_CHECK_CLOSE(t(W,1,0), 100., eps);
  BOOST_CHECK_CLOSE(t(W,0,1), 103., eps);
  BOOST_CHECK_CLOSE(t(W,1,1), 136., eps);
  Tensor b1 = cc.backward(xs, U, U, 0);
  Tensor b2 = cc.backward(xs, U, U, 1);
  Tensor b3 = cc.backward(xs, U, U, 2);
  cerr << b1 << endl;
  cerr << b2 << endl;
  cerr << b3 << endl;
  BOOST_CHECK_EQUAL(t(u1,0,0), t(b1,0,0));
  BOOST_CHECK_EQUAL(t(u1,1,0), t(b1,1,0));
  BOOST_CHECK_EQUAL(t(u2,0,0), t(b2,0,0));
  BOOST_CHECK_EQUAL(t(u2,1,0), t(b2,1,0));
  BOOST_CHECK_EQUAL(t(u3,0,0), t(b3,0,0));
  BOOST_CHECK_EQUAL(t(u3,1,0), t(b3,1,0));
}

BOOST_AUTO_TEST_CASE(ERowConcat)
{
  Tensor u1 = Ccm({2,1}, {1, 4});
  Tensor u2 = Ccm({2,1}, {2, 5});
  Tensor u3 = Ccm({2,1}, {3, 6});
  vector<const Tensor*> xs = {&u1, &u2, &u3};
  Concatenate cr;
  Tensor U = cr.forward(xs);
  cerr << U << endl;
  BOOST_REQUIRE_EQUAL(Dim({6,1}),size(U));
  double eps = 1e-5;
  BOOST_CHECK_CLOSE(t(U,0,0), 1., eps);
  BOOST_CHECK_CLOSE(t(U,1,0), 4., eps);
  BOOST_CHECK_CLOSE(t(U,2,0), 2., eps);
  BOOST_CHECK_CLOSE(t(U,3,0), 5., eps);
  BOOST_CHECK_CLOSE(t(U,4,0), 3., eps);
  BOOST_CHECK_CLOSE(t(U,5,0), 6., eps);

  Tensor b1 = cr.backward(xs, U, U, 0);
  Tensor b2 = cr.backward(xs, U, U, 1);
  Tensor b3 = cr.backward(xs, U, U, 2);
  cerr << b1 << endl;
  cerr << b2 << endl;
  cerr << b3 << endl;
  BOOST_CHECK_EQUAL(t(u1,0,0), t(b1,0,0));
  BOOST_CHECK_EQUAL(t(u1,1,0), t(b1,1,0));
  BOOST_CHECK_EQUAL(t(u2,0,0), t(b2,0,0));
  BOOST_CHECK_EQUAL(t(u2,1,0), t(b2,1,0));
  BOOST_CHECK_EQUAL(t(u3,0,0), t(b3,0,0));
  BOOST_CHECK_EQUAL(t(u3,1,0), t(b3,1,0));
}

BOOST_AUTO_TEST_CASE(EMultilinear) {
  Tensor b = Ccm({3,1},{1,2,3});
  Tensor W = Ccm({3,2},{2,4,6,3,5,7});
  Tensor x = Ccm({2,1},{-1,1});
  Multilinear ml;
  vector<const Tensor*> mlxs = {&b, &W, &x};
  Tensor r1 = ml.forward(mlxs);
  Sum se;
  MatrixMultiply mm;
  Tensor p = mm.forward(vector<const Tensor*>({&W, &x}));
  Tensor r2 = se.forward(vector<const Tensor*>({&p, &b}));
  BOOST_REQUIRE_EQUAL(size(r1), size(r2));
  double eps = 1e-5;
  cerr << r1 << endl;
  cerr << r2 << endl;
  BOOST_CHECK_CLOSE(t(r1,0,0), 2., eps);
  BOOST_CHECK_CLOSE(t(r1,1,0), 3., eps);
  BOOST_CHECK_CLOSE(t(r1,2,0), 4., eps);
  BOOST_CHECK_CLOSE(t(r2,0,0), 2., eps);
  BOOST_CHECK_CLOSE(t(r2,1,0), 3., eps);
  BOOST_CHECK_CLOSE(t(r2,2,0), 4., eps);
  Tensor dEdf = Ccm({3,1}, {1., 0.5, 0.25});
  Tensor dEdx = ml.backward(mlxs, r1, dEdf, 0);
  BOOST_CHECK_EQUAL(size(dEdx), size(b));
  dEdx = ml.backward(mlxs, r1, dEdf, 1);
  BOOST_CHECK_EQUAL(size(dEdx), size(W));
  cerr << dEdx << endl;
  dEdx = ml.backward(mlxs, r1, dEdf, 2);
  BOOST_CHECK_EQUAL(size(dEdx), size(x));
  cerr << r2 << endl;
  cerr << r1 << endl;
}

BOOST_AUTO_TEST_CASE(ELogisticSigmoid) {
  Tensor x = Ccm({5,1},{-6.f,-logf(3),0.f,logf(3),6.f});
  LogisticSigmoid ls;
  vector<const Tensor*> xs = {&x};
  Tensor r = ls.forward(xs);
  BOOST_REQUIRE_EQUAL(size(r), size(x));
  double eps = 1e-2;
  BOOST_CHECK_CLOSE(t(r,0,0), 1. /(1. + exp(6.)), eps);
  BOOST_CHECK_CLOSE(t(r,1,0), 0.25, eps);
  BOOST_CHECK_CLOSE(t(r,2,0), 0.5, eps);
  BOOST_CHECK_CLOSE(t(r,3,0), 0.75, eps);
  BOOST_CHECK_CLOSE(t(r,4,0), 1. - t(r,0,0), eps);
  Tensor dEdf = Ccm({5,1},{1.,1.,1.,1.,1.});
  Tensor dEdx = ls.backward(xs, r, dEdf, 0);
  BOOST_CHECK_CLOSE(t(dEdx,1,0), 0.1875, eps);
  BOOST_CHECK_CLOSE(t(dEdx,2,0), 0.25, eps);
  BOOST_CHECK_CLOSE(t(dEdx,3,0), t(dEdx,1,0), eps);
  BOOST_CHECK_CLOSE(t(dEdx,4,0), t(dEdx,0,0), eps);
}

BOOST_AUTO_TEST_CASE(ETanh) {
  Tensor x = Ccm({5,1},{-6.f,-logf(3),0.f,logf(3),6.f});
  Tanh th;
  vector<const Tensor*> xs = {&x};
  Tensor r = th.forward(xs);
  BOOST_REQUIRE_EQUAL(size(r), size(x));
  double eps = 1e-2;
  BOOST_CHECK_CLOSE(t(r,1,0), -0.8, eps);
  BOOST_CHECK_CLOSE(t(r,2,0), 0, eps);
  BOOST_CHECK_CLOSE(t(r,3,0), 0.8, eps);
  BOOST_CHECK_CLOSE(t(r,4,0), -t(r,0,0), eps);
  Tensor dEdf = Ccm({5,1},{1.,1.,1.,1.,1.});
  Tensor dEdx = th.backward(xs, r, dEdf, 0);
  BOOST_CHECK_CLOSE(t(dEdx,1,0), 0.36, eps);
  BOOST_CHECK_CLOSE(t(dEdx,2,0), 1.0, eps);
  BOOST_CHECK_CLOSE(t(dEdx,3,0), t(dEdx,1,0), eps);
  BOOST_CHECK_CLOSE(t(dEdx,4,0), t(dEdx,0,0), eps);
}

BOOST_AUTO_TEST_CASE(ESoftmaxUnif) {
  for (float v = -12.; v < 12.; v += 1.) { 
    Tensor u = Ccm({4,1}, {v, v, v, v});
    Softmax sm;
    vector<const Tensor*> xs = {&u};
    Tensor m = sm.forward(xs);
    BOOST_REQUIRE_EQUAL(Dim({4,1}),size(m));
    double eps = 1e-5;
    for (unsigned i = 0; i < 4; ++i)
      BOOST_CHECK_CLOSE(t(m, i, 0), 0.25, eps);
    Tensor dEdf = Ccm({4,1}, {1., 0., 0., 0.});
    Tensor d = sm.backward(xs, m, dEdf, 0);
    BOOST_CHECK_CLOSE(t(d,0,0), 0.1875, eps);
    BOOST_CHECK_CLOSE(t(d,1,0), -0.0625, eps);
    BOOST_CHECK_CLOSE(t(d,2,0), -0.0625, eps);
    BOOST_CHECK_CLOSE(t(d,3,0), -0.0625, eps);
//    cerr << d << endl;

    LogSoftmax lsm;
    Tensor lm = lsm.forward(xs);
    BOOST_REQUIRE_EQUAL(Dim({4,1}),size(lm));
    for (unsigned i = 0; i < 4; ++i)
      BOOST_CHECK_CLOSE(log(t(m, i, 0)), t(lm, i, 0), eps);
    Tensor b = lsm.backward(xs, lm, dEdf, 0);
    BOOST_CHECK_CLOSE(t(b, 0, 0), 0.75, eps);
    BOOST_CHECK_CLOSE(t(b, 1, 0), -0.25, eps);
    BOOST_CHECK_CLOSE(t(b, 2, 0), -0.25, eps);
    BOOST_CHECK_CLOSE(t(b, 3, 0), -0.25, eps);
  }
}


