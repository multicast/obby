#include "rendezvous.hpp"

#include <iostream>

using namespace std;
using namespace obby;

void on_discover(const string& name, const string& ip,
	unsigned int port)
{
	cout << std::time(0) << " " << name << " " << ip << " " << port << endl;
}

int main()
{
	rendezvous rendezvous;

	rendezvous.discover_event().connect(sigc::ptr_fun(&on_discover));
	rendezvous.discover();
	
	while(true)
		rendezvous.select();
}

