#ifndef COLOR_HEADER
	#define COLOR_HEADER
	#ifndef NOCOLOR
		#ifndef __linux__
			#define NOCOLOR
		#endif
	#endif
	#ifndef NOCOLOR
		#define RED   "\033[31m"
		#define GREEN "\033[32m"
		#define BLUE  "\033[34m"
		#define BOLD  "\033[1m"
		#define UND   "\033[4m"
		#define ENDS  "\033[0m"
		#define CERR(x)    std::cerr << RED << x      << " (" << __FILE__ << ":" << __LINE__ << ")" << ENDS << std::endl
		#define CERR2(x,y) std::cerr << RED << x << y << " (" << __FILE__ << ":" << __LINE__ << ")" << ENDS << std::endl
		#define CHECK(x) do { if(x!=0) { std::cerr << RED << "Failed at " << __FILE__ << ":" << __LINE__ << ENDS << std::endl; } } while(0)
		#define CHECKEQ(x,y) do { if(x!=y) { std::cerr << RED << "Failed at " << __FILE__ << ":" << __LINE__ << ENDS << std::endl; } } while(0)
		#define CHECKUNEQ(x,y) do { if(x==y) { std::cerr << RED << "Failed at " << __FILE__ << ":" << __LINE__ << ENDS << std::endl; } } while(0)
	#else
		#define RED   ""
		#define GREEN ""
		#define BLUE  ""
		#define BOLD  ""
		#define UND   ""
		#define ENDS  ""
		#define CERR(x)    std::cerr << x      << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl
		#define CERR2(x,y) std::cerr << x << y << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl
		#define CHECK(x) do { if(x!=0) { std::cerr << "Failed at " << __FILE__ << ":" << __LINE__ << std::endl; } } while(0)
		#define CHECKEQ(x,y) do { if(x!=y) { std::cerr << "Failed at " << __FILE__ << ":" << __LINE__ << std::endl; } } while(0)
		#define CHECKUNEQ(x,y) do { if(x==y) { std::cerr << "Failed at " << __FILE__ << ":" << __LINE__ << std::endl; } } while(0)
	#endif
	
#endif
