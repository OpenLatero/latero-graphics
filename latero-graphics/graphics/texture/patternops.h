#pragma once

#include <cairomm/context.h>

namespace latero::graphics {

class PatternOps
{
public:
	static Cairo::RefPtr<Cairo::Pattern> Add(Cairo::RefPtr<Cairo::Context> cr, 
							Cairo::RefPtr<Cairo::Pattern> a, 
							Cairo::RefPtr<Cairo::Pattern> b);

	static Cairo::RefPtr<Cairo::Pattern> Multiply(Cairo::RefPtr<Cairo::Context> cr, 
							Cairo::RefPtr<Cairo::Pattern> a, 
							Cairo::RefPtr<Cairo::Pattern> b);

	static Cairo::RefPtr<Cairo::Pattern> Multiply(Cairo::RefPtr<Cairo::Context> cr, 
							Cairo::RefPtr<Cairo::Pattern> a, 
							double b);

	static Cairo::RefPtr<Cairo::Pattern> Max(Cairo::RefPtr<Cairo::Context> cr, 
							Cairo::RefPtr<Cairo::Pattern> a, 
							Cairo::RefPtr<Cairo::Pattern> b);

	static Cairo::RefPtr<Cairo::Pattern> Invert(Cairo::RefPtr<Cairo::Context> cr, 
							Cairo::RefPtr<Cairo::Pattern> a);

};

} // namespace

