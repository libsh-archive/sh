#include "NextKSubset.hpp"

NextKSubset::NextKSubset() {
	ksub_mtc = false;
	m_a = NULL;
	m_k = 0;
	m_n = 0;
}

/***
 * source: Nijenhuis & Wilf, Combinatorial Algorithms, Academic Press, 1975.
 * changed for range 0...n-1 (instead of 1...n)
 *		n: number of elements in universe
 *		k: number of elements in subset
 *		*a:	a(i) is ith element in subset
 */
int *NextKSubset::next() {
	if (!ksub_mtc) {
		ksub_m2 = -1;
		ksub_h = m_k;
	}
	else {
		if (ksub_m2 < m_n - ksub_h - 1) {
			ksub_h = 0;
		}
		ksub_h++;
		ksub_m2 = m_a[m_k - ksub_h];
	}

	for(int j = 1; j <= ksub_h; j++) {
		m_a[m_k + j - ksub_h - 1] = ksub_m2 + j;
	}

	ksub_mtc = (m_a[0] != (m_n - m_k));

	return m_a;
}

void NextKSubset::init(int n, int k) {
	// ksubset init stuff
	m_a = (int *) malloc(sizeof(int) * (n + 1));

	for (int j = 0; j <= k; j++) {
		m_a[j] = j;
	}

	ksub_mtc = false;
	m_n = n;
	m_k = k;
}
