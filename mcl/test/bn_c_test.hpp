/*
	include from bn_if256_test.cpp and bn_if384_test.cpp
*/
#include <mcl/bn.h>
#include <cybozu/test.hpp>
#include <iostream>

template<size_t N>
std::ostream dump(std::ostream& os, const uint64_t (&x)[N])
{
	for (size_t i = 0; i < N; i++) {
		char buf[64];
		CYBOZU_SNPRINTF(buf, sizeof(buf), "%016llx", (long long)x[i]);
		os << buf;
	}
	return os;
}

CYBOZU_TEST_AUTO(init)
{
	int ret;
	CYBOZU_TEST_EQUAL(sizeof(mclBnFr), sizeof(Fr));
	CYBOZU_TEST_EQUAL(sizeof(mclBnG1), sizeof(G1));
	CYBOZU_TEST_EQUAL(sizeof(mclBnG2), sizeof(G2));
	CYBOZU_TEST_EQUAL(sizeof(mclBnGT), sizeof(Fp12));

	ret = mclBn_setErrFile("stderr");
	CYBOZU_TEST_EQUAL(ret, 0);

#if MCLBN_FP_UNIT_SIZE == 4
	printf("test MCLBN_curveFp254BNb %d\n", MCLBN_FP_UNIT_SIZE);
	ret = mclBn_init(mclBn_CurveFp254BNb, MCLBN_FP_UNIT_SIZE);
#else
	printf("test MCLBN_curveFp382_1 %d\n", MCLBN_FP_UNIT_SIZE);
	ret = mclBn_init(mclBn_CurveFp382_1, MCLBN_FP_UNIT_SIZE);
#endif
	CYBOZU_TEST_EQUAL(ret, 0);
}

CYBOZU_TEST_AUTO(Fr)
{
	mclBnFr x, y;
	memset(&x, 0xff, sizeof(x));
	CYBOZU_TEST_ASSERT(!mclBnFr_isValid(&x));
	memset(&x, 1, sizeof(x));
	CYBOZU_TEST_ASSERT(mclBnFr_isValid(&x));
	CYBOZU_TEST_ASSERT(!mclBnFr_isZero(&x));

	mclBnFr_clear(&x);
	CYBOZU_TEST_ASSERT(mclBnFr_isZero(&x));

	mclBnFr_setInt(&x, 1);
	CYBOZU_TEST_ASSERT(mclBnFr_isOne(&x));

	mclBnFr_setInt(&y, -1);
	CYBOZU_TEST_ASSERT(!mclBnFr_isEqual(&x, &y));

	y = x;
	CYBOZU_TEST_ASSERT(mclBnFr_isEqual(&x, &y));

	mclBnFr_setHashOf(&x, "", 0);
	mclBnFr_setHashOf(&y, "abc", 3);
	CYBOZU_TEST_ASSERT(!mclBnFr_isEqual(&x, &y));
	mclBnFr_setHashOf(&x, "abc", 3);
	CYBOZU_TEST_ASSERT(mclBnFr_isEqual(&x, &y));

	char buf[1024];
	mclBnFr_setInt(&x, 12345678);
	size_t size;
	size = mclBnFr_getStr(buf, sizeof(buf), &x, 10);
	CYBOZU_TEST_EQUAL(size, 8);
	CYBOZU_TEST_EQUAL(buf, "12345678");

	mclBnFr_setInt(&x, -7654321);
	mclBnFr_neg(&x, &x);
	size = mclBnFr_getStr(buf, sizeof(buf), &x, 10);
	CYBOZU_TEST_EQUAL(size, 7);
	CYBOZU_TEST_EQUAL(buf, "7654321");

	mclBnFr_setInt(&y, 123 - 7654321);
	mclBnFr_add(&x, &x, &y);
	size = mclBnFr_getStr(buf, sizeof(buf), &x, 10);
	CYBOZU_TEST_EQUAL(size, 3);
	CYBOZU_TEST_EQUAL(buf, "123");

	mclBnFr_setInt(&y, 100);
	mclBnFr_sub(&x, &x, &y);
	size = mclBnFr_getStr(buf, sizeof(buf), &x, 10);
	CYBOZU_TEST_EQUAL(size, 2);
	CYBOZU_TEST_EQUAL(buf, "23");

	mclBnFr_mul(&x, &x, &y);
	size = mclBnFr_getStr(buf, sizeof(buf), &x, 10);
	CYBOZU_TEST_EQUAL(size, 4);
	CYBOZU_TEST_EQUAL(buf, "2300");

	mclBnFr_div(&x, &x, &y);
	size = mclBnFr_getStr(buf, sizeof(buf), &x, 10);
	CYBOZU_TEST_EQUAL(size, 2);
	CYBOZU_TEST_EQUAL(buf, "23");

	const char *s = "12345678901234567";
	CYBOZU_TEST_ASSERT(!mclBnFr_setStr(&x, s, strlen(s), 10));
	s = "20000000000000000";
	CYBOZU_TEST_ASSERT(!mclBnFr_setStr(&y, s, strlen(s), 10));
	mclBnFr_add(&x, &x, &y);
	size = mclBnFr_getStr(buf, sizeof(buf), &x, 10);
	CYBOZU_TEST_EQUAL(size, 17);
	CYBOZU_TEST_EQUAL(buf, "32345678901234567");

	mclBnFr_setInt(&x, 1);
	mclBnFr_neg(&x, &x);
	size = mclBnFr_getStr(buf, sizeof(buf), &x, 10);
	CYBOZU_TEST_ASSERT(size > 0);
	CYBOZU_TEST_EQUAL(size, strlen(buf));
	CYBOZU_TEST_ASSERT(!mclBnFr_setStr(&y, buf, size, 10));
	CYBOZU_TEST_ASSERT(mclBnFr_isEqual(&x, &y));
}

CYBOZU_TEST_AUTO(G1)
{
	mclBnG1 x, y, z;
	memset(&x, 0xff, sizeof(x));
	CYBOZU_TEST_ASSERT(!mclBnG1_isValid(&x));
	mclBnG1_clear(&x);
	CYBOZU_TEST_ASSERT(mclBnG1_isValid(&x));
	CYBOZU_TEST_ASSERT(mclBnG1_isZero(&x));

	CYBOZU_TEST_ASSERT(!mclBnG1_hashAndMapTo(&y, "abc", 3));

	char buf[1024];
	size_t size;
	size = mclBnG1_getStr(buf, sizeof(buf), &x, 10);
	CYBOZU_TEST_ASSERT(size > 0);
	CYBOZU_TEST_EQUAL(size, strlen(buf));
	CYBOZU_TEST_ASSERT(!mclBnG1_setStr(&y, buf, strlen(buf), 10));
	CYBOZU_TEST_ASSERT(mclBnG1_isEqual(&x, &y));

	mclBnG1_neg(&x, &x);
	mclBnG1_add(&x, &x, &y);
	CYBOZU_TEST_ASSERT(mclBnG1_isZero(&x));

	mclBnG1_dbl(&x, &y); // x = 2y
	mclBnG1_add(&z, &y, &y);
	CYBOZU_TEST_ASSERT(mclBnG1_isEqual(&x, &z));
	mclBnG1_add(&z, &z, &y); // z = 3y
	mclBnFr n;
	mclBnFr_setInt(&n, 3);
	mclBnG1_mul(&x, &y, &n); //  x = 3y
	CYBOZU_TEST_ASSERT(mclBnG1_isEqual(&x, &z));
	mclBnG1_sub(&x, &x, &y); // x = 2y

	mclBnFr_setInt(&n, 2);
	mclBnG1_mul(&z, &y, &n); //  z = 2y
	CYBOZU_TEST_ASSERT(mclBnG1_isEqual(&x, &z));
}

CYBOZU_TEST_AUTO(G2)
{
	mclBnG2 x, y, z;
	memset(&x, 0xff, sizeof(x));
	CYBOZU_TEST_ASSERT(!mclBnG2_isValid(&x));
	mclBnG2_clear(&x);
	CYBOZU_TEST_ASSERT(mclBnG2_isValid(&x));
	CYBOZU_TEST_ASSERT(mclBnG2_isZero(&x));

	CYBOZU_TEST_ASSERT(!mclBnG2_hashAndMapTo(&x, "abc", 3));

	char buf[1024];
	size_t size;
	size = mclBnG2_getStr(buf, sizeof(buf), &x, 10);
	CYBOZU_TEST_ASSERT(size > 0);
	CYBOZU_TEST_EQUAL(size, strlen(buf));
	CYBOZU_TEST_ASSERT(!mclBnG2_setStr(&y, buf, strlen(buf), 10));
	CYBOZU_TEST_ASSERT(mclBnG2_isEqual(&x, &y));

	mclBnG2_neg(&x, &x);
	mclBnG2_add(&x, &x, &y);
	CYBOZU_TEST_ASSERT(mclBnG2_isZero(&x));

	mclBnG2_dbl(&x, &y); // x = 2y
	mclBnG2_add(&z, &y, &y);
	CYBOZU_TEST_ASSERT(mclBnG2_isEqual(&x, &z));
	mclBnG2_add(&z, &z, &y); // z = 3y
	mclBnFr n;
	mclBnFr_setInt(&n, 3);
	mclBnG2_mul(&x, &y, &n); //  x = 3y
	CYBOZU_TEST_ASSERT(mclBnG2_isEqual(&x, &z));
	mclBnG2_sub(&x, &x, &y); // x = 2y

	mclBnFr_setInt(&n, 2);
	mclBnG2_mul(&z, &y, &n); //  z = 2y
	CYBOZU_TEST_ASSERT(mclBnG2_isEqual(&x, &z));
}

CYBOZU_TEST_AUTO(GT)
{
	mclBnGT x, y, z;
	memset(&x, 1, sizeof(x));
	CYBOZU_TEST_ASSERT(!mclBnGT_isZero(&x));

	mclBnGT_clear(&x);
	CYBOZU_TEST_ASSERT(mclBnGT_isZero(&x));

	char buf[2048];
	const char *s = "1 2 3 4 5 6 7 8 9 10 11 12";
	size_t size;
	CYBOZU_TEST_ASSERT(!mclBnGT_setStr(&x,s , strlen(s), 10));
	size = mclBnGT_getStr(buf, sizeof(buf), &x, 10);
	CYBOZU_TEST_ASSERT(size > 0);
	CYBOZU_TEST_EQUAL(size, strlen(buf));
	CYBOZU_TEST_EQUAL(buf, s);

	y = x;
	CYBOZU_TEST_ASSERT(mclBnGT_isEqual(&x, &y));

	s = "-1 -2 -3 -4 -5 -6 -7 -8 -9 -10 -11 -12";
	CYBOZU_TEST_ASSERT(!mclBnGT_setStr(&z, s, strlen(s), 10));
	size = mclBnGT_getStr(buf, sizeof(buf), &z, 10);
	CYBOZU_TEST_ASSERT(size > 0);
	CYBOZU_TEST_EQUAL(size, strlen(buf));
	CYBOZU_TEST_ASSERT(!mclBnGT_setStr(&y, buf, size, 10));

	mclBnGT_neg(&z, &y);
	CYBOZU_TEST_ASSERT(mclBnGT_isEqual(&x, &z));

	mclBnGT_add(&y, &x, &y);
	CYBOZU_TEST_ASSERT(mclBnGT_isZero(&y));

	s = "2 0 0 0 0 0 0 0 0 0 0 0";
	CYBOZU_TEST_ASSERT(!mclBnGT_setStr(&y, s, strlen(s), 10));
	mclBnGT_mul(&z, &x, &y);
	size = mclBnGT_getStr(buf, sizeof(buf), &z, 10);
	CYBOZU_TEST_ASSERT(size > 0);
	CYBOZU_TEST_EQUAL(size, strlen(buf));
	CYBOZU_TEST_EQUAL(buf, "2 4 6 8 10 12 14 16 18 20 22 24");

	mclBnGT_div(&z, &z, &y);
	size = mclBnGT_getStr(buf, sizeof(buf), &x, 10);
	CYBOZU_TEST_ASSERT(size > 0);
	CYBOZU_TEST_EQUAL(size, strlen(buf));
	CYBOZU_TEST_ASSERT(mclBnGT_isEqual(&x, &z));

	mclBnFr n;
	mclBnFr_setInt(&n, 3);
	mclBnGT_pow(&z, &x, &n);
	mclBnGT_mul(&y, &x, &x);
	mclBnGT_mul(&y, &y, &x);
	CYBOZU_TEST_ASSERT(mclBnGT_isEqual(&y, &z));
}

CYBOZU_TEST_AUTO(pairing)
{
	mclBnFr a, b, ab;
	mclBnFr_setInt(&a, 123);
	mclBnFr_setInt(&b, 456);
	mclBnFr_mul(&ab, &a, &b);
	mclBnG1 P, aP;
	mclBnG2 Q, bQ;
	mclBnGT e, e1, e2;

	CYBOZU_TEST_ASSERT(!mclBnG1_hashAndMapTo(&P, "1", 1));
	CYBOZU_TEST_ASSERT(!mclBnG2_hashAndMapTo(&Q, "1", 1));

	mclBnG1_mul(&aP, &P, &a);
	mclBnG2_mul(&bQ, &Q, &b);

	mclBn_pairing(&e, &P, &Q);
	mclBnGT_pow(&e1, &e, &a);
	mclBn_pairing(&e2, &aP, &Q);
	CYBOZU_TEST_ASSERT(mclBnGT_isEqual(&e1, &e2));

	mclBnGT_pow(&e1, &e, &b);
	mclBn_pairing(&e2, &P, &bQ);
	CYBOZU_TEST_ASSERT(mclBnGT_isEqual(&e1, &e2));
}

CYBOZU_TEST_AUTO(precomputed)
{
	mclBnG1 P1, P2;
	mclBnG2 Q1, Q2;
	CYBOZU_TEST_ASSERT(!mclBnG1_hashAndMapTo(&P1, "1", 1));
	CYBOZU_TEST_ASSERT(!mclBnG1_hashAndMapTo(&P2, "123", 3));
	CYBOZU_TEST_ASSERT(!mclBnG2_hashAndMapTo(&Q1, "1", 1));
	CYBOZU_TEST_ASSERT(!mclBnG2_hashAndMapTo(&Q2, "2", 1));

	const int size = mclBn_getUint64NumToPrecompute();
	std::vector<uint64_t> Q1buf, Q2buf;
	Q1buf.resize(size);
	Q2buf.resize(size);
	mclBn_precomputeG2(Q1buf.data(), &Q1);
	mclBn_precomputeG2(Q2buf.data(), &Q2);

	mclBnGT e1, e2, f1, f2, f3;
	mclBn_pairing(&e1, &P1, &Q1);
	mclBn_precomputedMillerLoop(&f1, &P1, Q1buf.data());
	mclBn_finalExp(&f1, &f1);
	CYBOZU_TEST_ASSERT(mclBnGT_isEqual(&e1, &f1));

	mclBn_pairing(&e2, &P2, &Q2);
	mclBn_precomputedMillerLoop(&f2, &P2, Q2buf.data());
	mclBn_finalExp(&f2, &f2);
	CYBOZU_TEST_ASSERT(mclBnGT_isEqual(&e2, &f2));

	mclBn_precomputedMillerLoop2(&f3, &P1, Q1buf.data(), &P2, Q2buf.data());
	mclBn_finalExp(&f3, &f3);

	mclBnGT_mul(&e1, &e1, &e2);
	CYBOZU_TEST_ASSERT(mclBnGT_isEqual(&e1, &f3));
}

CYBOZU_TEST_AUTO(end)
{
	int ret = mclBn_setErrFile("bn_if.log");
	CYBOZU_TEST_EQUAL(ret, 0);
}
