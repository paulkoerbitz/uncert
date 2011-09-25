#ifndef ql_extensions__algorithm__devector_copy_hpp
#define ql_extensions__algorithm__devector_copy_hpp

#include <flens/flens.h>

namespace QuantLibExt {

	typedef flens::DenseVector<flens::Array<double> > DEVector;

	template <class OutputIterator>
	void copy_from_devector(const DEVector& in, OutputIterator out) {
	 	for (int i=in.firstIndex(); i <= in.lastIndex(); ++i, ++out) {
	 		*out = in(i);
	 	}
	}
	 
	template <class InputIterator>
	void copy_to_devector(InputIterator in, DEVector& out) {
	 	for (int i=out.firstIndex(); i <= out.lastIndex(); ++i, ++in) {
	 		out(i) = *in;
	 	}
	}
}

#endif
