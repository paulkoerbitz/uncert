#ifndef ql_extensions__algorithm__check_hpp__
#define ql_extensions__algorithm__check_hpp__

template <class InputIterator1, class InputIterator2, class BinaryOperator>
bool check(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
		   BinaryOperator f) {
	for ( ; first1!=last1; ++first1, ++first2) {
		if (not f(*first1, *first2) ) {
			return false;
		}
	}
	return true; 
}

#endif
