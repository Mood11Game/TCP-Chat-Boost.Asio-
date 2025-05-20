#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <memory>
#include <mutex>

using namespace boost::asio;
using namespace std;

class Client
{
private:
	io_context io;
	ip::tcp::socket socket;

	void readMess()
	{
		try
		{
			char data[2048];
			while (true)
			{
				memset(data, 0, sizeof(data));
				size_t len = socket.read_some(buffer(data));
				if (len == 0) break;

				string msg(data, len);

				cout << '\n' << "you maybe friend > " << msg;
				cout << '\n' << "you > ";
			}
		}
		catch (...)
		{
			cout << "[Disconnected]\n";
		}
	}

	void writeMess()
	{
		try
		{
			string msg;
			while (getline(cin, msg))
			{
				write(socket, buffer(msg));
				cout << "you >";
			}
		}
		catch (...)
		{
			cout << "[Error while sending]\n";
		}
	}

public:
	Client(short port) : socket(io)
	{
		ip::tcp::endpoint endpoint(ip::make_address("127.0.0.1"), port);
		socket.connect(endpoint);

		cout << "[Connected to server]\n";
		cout << "you >";
		thread reader(&Client::readMess, this);
		thread writer(&Client::writeMess, this);

		reader.join();
		writer.join();
	}
};

int main()
{
  
	try
	{
		Client obj(5000);
	}
	catch (const exception& e)
	{
		cout << "[Client Error]: " << e.what() << '\n';
	}
}
