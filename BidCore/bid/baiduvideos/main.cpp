#include <string>
#include <iostream>
#include "../../common/type.h"
#include "../../common/local_log.h"
#include "baiduvideos_bid.h"
#include "baiduvideos_context.h"

using namespace std;


BidContext *g_context = NULL;


int main()
{
	cout << "baiduvideos fast cgi start\n";
	g_context = new BaiduvideosContext;
	if (!g_context->Init(PRIVATE_CONF))
	{
		cout << "g_context->Init failed\n";
		return 1;
	}

	g_context->StartWork();
	g_context->Monitor();

	cout << "g_context->Uninit()" << endl;
	g_context->Uninit();
	cout << "end\n";
	return 0;
}
