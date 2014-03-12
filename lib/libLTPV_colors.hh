/*
 * (C) Copyright 2013 - Thales SA (author: Simon DENEL - Thales Research & Technology)
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
*/
#ifndef LTPV_COLOR_H
	#define LTPV_COLOR_H
	#ifndef LTPV_NOCOLOR
		#ifndef __linux__
			#define LTPV_NOCOLOR
		#endif
	#endif
	#ifndef LTPV_NOCOLOR
		#define LTPV_RED   "\033[31m"
		#define LTPV_GREEN "\033[32m"
		#define LTPV_BLUE  "\033[34m"
		#define LTPV_BOLD  "\033[1m"
		#define LTPV_UND   "\033[4m"
		#define LTPV_ENDS  "\033[0m"
		#define LTPV_CERR(x)    std::cerr << RED << x      << " (" << __FILE__ << ":" << __LINE__ << ")" << ENDS << std::endl
		#define LTPV_CERR2(x,y) std::cerr << RED << x << y << " (" << __FILE__ << ":" << __LINE__ << ")" << ENDS << std::endl
		#define LTPV_CHECK(x) do { if(x!=0) { std::cerr << RED << "Failed at " << __FILE__ << ":" << __LINE__ << ENDS << std::endl; } } while(0)
		#define LTPV_CHECKEQ(x,y) do { if(x!=y) { std::cerr << RED << "Failed at " << __FILE__ << ":" << __LINE__ << ENDS << std::endl; } } while(0)
		#define LTPV_CHECKUNEQ(x,y) do { if(x==y) { std::cerr << RED << "Failed at " << __FILE__ << ":" << __LINE__ << ENDS << std::endl; } } while(0)
	#else
		#define LTPV_RED   ""
		#define LTPV_GREEN ""
		#define LTPV_BLUE  ""
		#define LTPV_BOLD  ""
		#define LTPV_UND   ""
		#define LTPV_ENDS  ""
		#define LTPV_CERR(x)    std::cerr << x      << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl
		#define LTPV_CERR2(x,y) std::cerr << x << y << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl
		#define LTPV_CHECK(x) do { if(x!=0) { std::cerr << "Failed at " << __FILE__ << ":" << __LINE__ << std::endl; } } while(0)
		#define LTPV_CHECKEQ(x,y) do { if(x!=y) { std::cerr << "Failed at " << __FILE__ << ":" << __LINE__ << std::endl; } } while(0)
		#define LTPV_CHECKUNEQ(x,y) do { if(x==y) { std::cerr << "Failed at " << __FILE__ << ":" << __LINE__ << std::endl; } } while(0)
	#endif
	
#endif
